import pandas as pd
import numpy as np
import os
import yfinance as yf
import time

# Ensure data directory exists
if not os.path.exists("data"):
    os.makedirs("data")

print("--- Data Manager Started ---")

def create_backup_data(filename, start_price, vol, name):
    """Generates synthetic data so the C++ engine has something to read."""
    print(f"[FALLBACK] Generating BACKUP data for {name}...")
    np.random.seed(42)
    dates = pd.date_range(start="2023-01-01", periods=252)
    spots = [start_price]
    for _ in range(251):
        spots.append(spots[-1] * (1 + np.random.normal(0, vol/np.sqrt(252))))
    
    df = pd.DataFrame({
        'Date': dates,
        'Spot': spots,
        'Rate': 0.045,
        'Vol': vol
    })
    df.to_csv(filename, index=False)
    print(f"      -> Success: Created {filename}")

# --- 1. YAHOO FINANCE (With Safety Net) ---
print("[1/3] Fetching SPY from Yahoo Finance...")
try:
    # Attempt download
    df = yf.download("SPY", start="2023-01-01", end="2024-01-01", progress=False)
    
    # Check if empty
    if df.empty:
        raise ValueError("Yahoo returned empty data (Rate Limit likely).")

    # Handle Multi-Index (New Yahoo Format)
    if isinstance(df.columns, pd.MultiIndex):
        try:
            # Try to get 'Adj Close'
            spots = df.xs('Adj Close', level=0, axis=1)
            if isinstance(spots, pd.DataFrame): spots = spots.iloc[:, 0]
        except:
            # Fallback to first column
            spots = df.iloc[:, 0]
    else:
        # Old Format
        spots = df['Adj Close'] if 'Adj Close' in df.columns else df.iloc[:, 0]

    # Save Real Data
    output = pd.DataFrame({
        'Date': spots.index,
        'Spot': spots.values,
        'Rate': 0.045,
        'Vol': 0.15
    }).dropna()
    
    output.to_csv("data/market_data_yfinance.csv", index=False)
    print("      -> Success: Real Data Fetched.")

except Exception as e:
    print(f"      [WARNING] Yahoo Failed ({e}). Switching to Backup...")
    # THIS GUARANTEES THE FILE EXISTS
    create_backup_data("data/market_data_yfinance.csv", 400.0, 0.15, "Yahoo Backup")

# --- 2. SYNTHETIC DATA ---
print("[2/3] Generating Synthetic Data...")
create_backup_data("data/market_data_synthetic.csv", 100.0, 0.2, "Synthetic")

# --- 3. BLOOMBERG MOCK DATA ---
print("[3/3] Generating Bloomberg Mock Data...")
dates = pd.date_range(start="2023-06-01", periods=100)
df_bbg = pd.DataFrame({
    'DATE': dates,
    'PX_LAST': np.random.uniform(150, 160, 100),
    'US_TREASURY_3M': 5.0,
    'VOLATILITY_30D': 20.0
})
df_bbg.to_csv("data/bloomberg_export.csv", index=False)
print("      -> Success: Created bloomberg_export.csv")