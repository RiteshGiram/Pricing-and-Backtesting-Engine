import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

log_files = glob.glob("logs/result_*.csv")

plt.figure(figsize=(12, 6))

for file in log_files:
    df = pd.read_csv(file)
    label = os.path.basename(file).replace("result_", "").replace(".csv", "")
    
    # Plot Hedging Error directly
    plt.plot(df['HedgingError'], label=label)

plt.axhline(0, color='black', linestyle='--') # Theoretical Zero Line
plt.title("Hedging Error Distribution (Delta Replication)")
plt.xlabel("Trading Days")
plt.ylabel("Error ($)")
plt.legend()
plt.grid(True)
plt.savefig("logs/hedging_error.png")
print("Chart saved: logs/hedging_error.png")