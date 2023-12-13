import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split
import json
import sys
import ast

input_size = 1
hidden_size = 64
num_layers = 1
num_epochs = 100
learning_rate = 0.001
sequence_length = 5
eval_epochs = 50

# Define the Neural Network Model
class StockPredictor(nn.Module):
    def __init__(self, input_size, hidden_size, num_layers=1):
        super(StockPredictor, self).__init__()
        self.hidden_size = hidden_size
        self.num_layers = num_layers
        self.lstm = nn.LSTM(input_size, hidden_size, num_layers, batch_first=True)
        self.fc = nn.Linear(hidden_size, 1)

    def forward(self, x):
        h0 = torch.zeros(self.num_layers, x.size(0), self.hidden_size).to(x.device)
        c0 = torch.zeros(self.num_layers, x.size(0), self.hidden_size).to(x.device)
        
        out, _ = self.lstm(x, (h0, c0))
        out = self.fc(out[:, -1, :])
        return out

def prepare_data(data, sequence_length, scaler):
    scaled_data = scaler.fit_transform(np.array(data).reshape(-1, 1))

    X, y = [], []
    for i in range(sequence_length, len(scaled_data)):
        X.append(scaled_data[i-sequence_length:i, 0])
        y.append(scaled_data[i, 0])

    return np.array(X), np.array(y)

def train_model(model, train_loader, criterion, optimizer, num_epochs):
    model.train()
    for epoch in range(num_epochs):
        for sequences, labels in train_loader:
            optimizer.zero_grad()
            outputs = model(sequences)
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()
            if epoch % eval_epochs == 0 or epoch == num_epochs - 1:            
                print(f'Epoch [{epoch+1}/{num_epochs}], Loss: {loss.item():.4f}', flush=True)

def main(data):
    #print("Received data from C++:", data, flush=True)
    predictions = {}
    scalers = {}  # Dictionary to store scalers for each company

    for companyIndex, stock_numbers in data:
        scaler = MinMaxScaler(feature_range=(0, 1))
        X, y = prepare_data(stock_numbers, sequence_length, scaler)

        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
        scalers[companyIndex] = scaler  # Store the scaler

        # Convert to PyTorch tensors
        X_train = torch.tensor(X_train).float().unsqueeze(-1)
        y_train = torch.tensor(y_train).float().unsqueeze(-1)

        # Training and model code...

        train_data = torch.utils.data.TensorDataset(X_train, y_train)
        train_loader = torch.utils.data.DataLoader(dataset=train_data, batch_size=2, shuffle=True)

        model = StockPredictor(input_size, hidden_size, num_layers)
        criterion = nn.MSELoss()
        optimizer = optim.Adam(model.parameters(), lr=learning_rate)

        train_model(model, train_loader, criterion, optimizer, num_epochs)
        
        # Making predictions and inverse scaling
        model.eval()
        with torch.no_grad():
            test_data = torch.tensor(X_test).float().unsqueeze(-1)
            predicted_stock_numbers = model(test_data).cpu().numpy()

            # Inverse transform the predictions
            predicted_stock_numbers = scalers[companyIndex].inverse_transform(predicted_stock_numbers.reshape(-1, 1))
            predictions[companyIndex] = predicted_stock_numbers.flatten().tolist()
            print(f"Predictions for Company {companyIndex}: {predictions[companyIndex]}", flush=True)
    
    # Convert predictions to a string representation
    predictions_str = json.dumps({str(k): v for k, v in predictions.items()})
    #print("Returning from Python script:", predictions_str, flush=True)

    return predictions_str
