import numpy as np
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, TensorDataset
from sklearn.preprocessing import MinMaxScaler
import json
import time

input_size = 5
hidden_size = 100
num_layers = 2
output_size = 5
batch_size = 128
seq_length = 200
training_duration = 20
general_training_duration = 30
learning_rate = 0.001
epochs = 1000
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

class StockPredictorGRU(nn.Module):
    def __init__(self, input_size, hidden_size, num_layers, output_size):
        super(StockPredictorGRU, self).__init__()
        self.hidden_size = hidden_size
        self.num_layers = num_layers
        self.gru = nn.GRU(input_size, hidden_size, num_layers, batch_first=True)
        self.fc = nn.Linear(hidden_size, output_size)

    def forward(self, x):
        h0 = torch.zeros(self.num_layers, x.size(0), self.hidden_size).to(x.device)
        out, _ = self.gru(x, h0)
        out = self.fc(out[:, -1, :])
        return out

def prepare_company_data(company_data):
    data = []
    for entry in company_data['dataPackage']:
        data.append([entry['open'], entry['high'], entry['low'], entry['close'], entry['volume']])
    return np.array(data)

def normalize_data(data):
    scaler = MinMaxScaler(feature_range=(0, 1))
    return scaler.fit_transform(data), scaler

def create_sequences(data, seq_length):
    xs = []
    for i in range(len(data) - seq_length):
        xs.append(data[i:(i + seq_length)])
    return np.array(xs)

def predict(model, company_data, seq_length):
    # Prepare and normalize data
    data = prepare_company_data(company_data)
    data_normalized, scaler = normalize_data(data)
    sequences = create_sequences(data_normalized, seq_length)

    sequences_tensor = torch.tensor(sequences).float().to(device)

    model.eval()
    with torch.no_grad():
        predictions = model(sequences_tensor).cpu().numpy()

    predictions = scaler.inverse_transform(predictions)
    return predictions

def train_general_model(input_data, model, criterion, optimizer, device, training_duration, seq_length, batch_size):
    combined_data = combine_all_company_data(input_data)
    data_normalized, _ = normalize_data(combined_data)
    sequences = create_sequences(data_normalized, seq_length)

    train_data = torch.tensor(sequences[:-1]).float()
    target_data = torch.tensor(data_normalized[1:len(sequences)]).float()

    train_loader = DataLoader(TensorDataset(train_data, target_data), batch_size=batch_size, shuffle=True)
    train_model(model, train_loader, criterion, optimizer, device, training_duration)
    return model

def combine_all_company_data(input_data):
    all_data = []

    for company in input_data:
        company_data = prepare_company_data(company)
        all_data.extend(company_data)

    return np.array(all_data)

def normalize_data_separately(data):
    prices = data[:, :4]  # Open, High, Low, Close
    volumes = data[:, 4].reshape(-1, 1)  # Volume

    price_scaler = MinMaxScaler(feature_range=(0, 1))
    volume_scaler = MinMaxScaler(feature_range=(0, 1))
    prices_scaled = price_scaler.fit_transform(prices)
    volumes_scaled = volume_scaler.fit_transform(volumes)

    data_scaled = np.concatenate((prices_scaled, volumes_scaled), axis=1)

    return data_scaled, price_scaler, volume_scaler

def fine_tune_for_company(company, general_model, criterion, optimizer, device, fine_tune_duration, seq_length, batch_size):
    data = prepare_company_data(company)
    data_normalized, scaler = normalize_data(data)
    sequences = create_sequences(data_normalized, seq_length)

    if len(sequences) > 0:
        train_data = torch.tensor(sequences[:-1]).float()
        target_data = torch.tensor(data_normalized[1:len(sequences)]).float()

        if len(target_data) > 0:
            train_loader = DataLoader(TensorDataset(train_data, target_data), batch_size=batch_size, shuffle=True)
            train_model(general_model, train_loader, criterion, optimizer, device, fine_tune_duration)

    return general_model, scaler

def train_model(model, train_loader, criterion, optimizer, device, training_duration):
    start_time = time.time()
    model.train()
    
    for epoch in range(epochs):
        for sequences, targets in train_loader:
            if time.time() - start_time > training_duration:
                print(f"Training stopped after {training_duration} seconds.", flush=True)
                return
            sequences, targets = sequences.to(device), targets.to(device)
            optimizer.zero_grad()
            outputs = model(sequences)
            loss = criterion(outputs, targets)
            loss.backward()
            optimizer.step()

        if epoch % 10 == 0:
            print(f'Epoch {epoch}, Loss: {loss.item()}', flush=True)

def print_min_max_for_company(company_data):
    data = []
    for entry in company_data['dataPackage']:
        data.append([entry['open'], entry['high'], entry['low'], entry['close'], entry['volume']])
    data = np.array(data)
    feature_names = ['Open', 'High', 'Low', 'Close', 'Volume']
    print(f"Company Index: {company_data['companyIndex']}")
    for i, feature in enumerate(feature_names):
        min_val = np.min(data[:, i])
        max_val = np.max(data[:, i])
        print(f"  {feature}: Lowest = {min_val}, Highest = {max_val}")

def main(input_data):
    predictions = {}
    
    general_model = StockPredictorGRU(input_size=input_size, hidden_size=hidden_size, num_layers=num_layers, output_size=output_size).to(device)
    optimizer = torch.optim.Adam(general_model.parameters(), lr=learning_rate)
    criterion = nn.MSELoss()
    general_model = train_general_model(input_data, general_model, criterion, optimizer, device, training_duration, seq_length, batch_size)

    # Fine-tuning for each company
    for company in input_data:
        print_min_max_for_company(company)
        fine_tuned_model, scaler = fine_tune_for_company(company, general_model, criterion, optimizer, device, training_duration, seq_length, batch_size)
        company_predictions = predict(fine_tuned_model, company, seq_length)
        predictions[company['companyIndex']] = company_predictions[-1].tolist()
        print(f"Predictions for company {company['companyIndex']}: {company_predictions[-1]}", flush=True)

    return json.dumps(predictions)