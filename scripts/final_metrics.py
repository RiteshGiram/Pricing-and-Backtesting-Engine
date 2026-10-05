import pandas as pd
import numpy as np
import glob
import os

print(f"{'Model':<30} | {'Mean Error ($)':<15} | {'Error StdDev':<15} | {'Max Deviation ($)':<15}")
print("-" * 85)

for file in glob.glob("logs/result_*.csv"):
    try:
        df = pd.read_csv(file)
        
        # We focus on HedgingError now
        error = df['HedgingError']
        
        mean_error = error.mean()
        std_error = error.std()
        max_dev = error.abs().max()

        name = os.path.basename(file).replace("result_", "").replace(".csv", "")
        print(f"{name:<30} | {mean_error:>14.4f} | {std_error:>14.4f} | {max_dev:>14.4f}")

    except Exception:
        pass