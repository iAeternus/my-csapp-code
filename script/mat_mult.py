import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("results/mat_mult.csv")

plt.figure()

for col in df.columns[1:]:
    plt.plot(df["n"], df[col], marker='o', label=col)

plt.xlabel("Matrix Size (n)")
plt.ylabel("Time per operation (ns)")
plt.title("Matrix Multiplication Loop Order Performance")
plt.legend()
plt.grid()

plt.show()
