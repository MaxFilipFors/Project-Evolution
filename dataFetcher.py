from alpha_vantage.timeseries import TimeSeries
import pandas as pd
import time
import logging
import os

# Initialize logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def file_exists_and_not_empty(file_path):
    return os.path.exists(file_path) and os.stat(file_path).st_size > 0

def fetch_and_append_data(symbol, api_key, output_file):
    try:
        ts = TimeSeries(key=api_key, output_format='pandas')
        data, _ = ts.get_daily(symbol, outputsize='full')
        if data.empty:
            logging.error(f"No data returned for {symbol}")
            return

        data['Symbol'] = symbol  # Add a column for the company symbol

        if file_exists_and_not_empty(output_file):
            data.to_csv(output_file, mode='a', header=False)
        else:
            data.to_csv(output_file, header=True)
        logging.info(f"Data for {symbol} appended successfully.")
    except Exception as e:
        logging.error(f"Error fetching data for {symbol}: {e}")

api_key = 'PUK1L6PM383CZYWZ'
companies = ['AAPL', 'MSFT', 'GOOGL', 'AMZN', 'FB', 'TSLA', 'BRK.A', 'V', 'JNJ', 'WMT']
output_file = 'Data/eval_data.csv'
calls_per_minute = 5
delay_seconds = 60 / calls_per_minute

for company in companies:
    fetch_and_append_data(company, api_key, output_file)
    time.sleep(delay_seconds)
