import numpy as np
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, TensorDataset
from sklearn.preprocessing import MinMaxScaler
import json
import time

# Hyperparameters
input_size = 5
hidden_size = 100
num_layers = 2
output_size = 5
batch_size = 128
seq_length = 200
training_duration = 5
learning_rate = 0.001
epochs = 100
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
    data = [[entry['open'], entry['high'], entry['low'], entry['close'], entry['volume']] for entry in company_data['dataPackage']]
    return np.array(data)

def normalize_data(data):
    scaler = MinMaxScaler(feature_range=(0, 1))
    return scaler.fit_transform(data), scaler

def create_sequences(data, seq_length):
    return np.array([data[i:(i + seq_length)] for i in range(len(data) - seq_length)])

def predict(model, company_data, seq_length):
    data = prepare_company_data(company_data)
    data_normalized, scaler = normalize_data(data)
    sequences = create_sequences(data_normalized, seq_length)
    sequences_tensor = torch.tensor(sequences).float().to(device)
    model.eval()
    with torch.no_grad():
        predictions = model(sequences_tensor).cpu().numpy()
    return scaler.inverse_transform(predictions)[-1].tolist()  # Return only the last prediction

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

def combine_and_train(input_data, model, optimizer, criterion, device, training_duration, seq_length, batch_size):
    all_data = np.concatenate([prepare_company_data(company) for company in input_data])
    data_normalized, _ = normalize_data(all_data)
    sequences = create_sequences(data_normalized, seq_length)
    train_data = torch.tensor(sequences[:-1]).float()
    target_data = torch.tensor(data_normalized[1:len(sequences)]).float()
    train_loader = DataLoader(TensorDataset(train_data, target_data), batch_size=batch_size, shuffle=True)
    train_model(model, train_loader, criterion, optimizer, device, training_duration)
    return model

def main(input_data):
    model = StockPredictorGRU(input_size, hidden_size, num_layers, output_size).to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate)
    criterion = nn.MSELoss()
    trained_model = combine_and_train(input_data, model, optimizer, criterion, device, training_duration, seq_length, batch_size)

    predictions = {company['companyIndex']: predict(trained_model, company, seq_length) for company in input_data}
    
    print("predictions are: ", predictions, flush=True)
    return json.dumps(predictions)
