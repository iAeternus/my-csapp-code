import matplotlib
matplotlib.use('TkAgg')
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results/spatial_locality.csv")
plt.figure(figsize=(10, 6))

for col in df.columns[1:]:
    plt.plot(df["size"], df[col], marker='o', label=col)

plt.xscale("log")
plt.yscale("log")

plt.xlabel("Input Size (log scale)")
plt.ylabel("Time (ms, log scale)")
plt.title("Spatial Locality Performance Comparison")

plt.grid(True, which="both", linestyle="--", linewidth=0.5)
plt.legend()

plt.savefig("results/spatial_locality.png", dpi=300)
plt.show()
