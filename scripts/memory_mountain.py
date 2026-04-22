import matplotlib
matplotlib.use('TkAgg')
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator

# === 1. 数据加载与清洗 ===
df = pd.read_csv("results/memory_mountain.csv")
df = df.replace([np.inf, -np.inf], np.nan).dropna()
df = df[df["stride"] <= 12]  # 限制 stride 范围，提升可读性

# === 2. 构建二维矩阵（stride × size）===
size_order = sorted(df["size"].unique())      # X轴：工作集大小（从小到大）
stride_order = sorted(df["stride"].unique())  # Y轴：访问步长

pivot = df.pivot(index="stride", columns="size", values="bandwidth_MBps")
pivot = pivot.loc[stride_order, size_order]

# 网格坐标（X=size索引，Y=stride）
X, Y = np.meshgrid(np.arange(len(size_order)), stride_order)
Z = pivot.values

# === 3. 单位格式化（KB / MB）===
def format_size(x):
    return f"{int(x / (1024*1024))}MB" if x >= 1024 * 1024 else f"{int(x / 1024)}KB"

# === 4. 绘制 Memory Mountain ===
fig = plt.figure(figsize=(18, 11))
ax = fig.add_subplot(111, projection="3d")

surf = ax.plot_surface(X, Y, Z, cmap="plasma", edgecolor="none")

# X轴：Working Set Size（降采样避免重叠）
xticks = np.arange(0, len(size_order), 2)
ax.set_xticks(xticks)
ax.set_xticklabels([format_size(size_order[i]) for i in xticks],
                   rotation=30, ha="right")
ax.set_xlabel("Working Set Size", labelpad=25)

# Y轴：Stride（s1, s3, ...）
yticks = np.arange(1, 13, 2)
ax.set_yticks(yticks)
ax.set_yticklabels([f"s{i}" for i in yticks])
ax.set_ylabel("Stride (×8 bytes)", labelpad=20)

# Z轴：带宽
ax.set_zlabel("Read Bandwidth (MB/s)", labelpad=20)
ax.zaxis.set_major_locator(MultipleLocator(2000))

# 视角（保证 size 从左→右）
ax.view_init(elev=28, azim=-120)

ax.set_title("Memory Mountain", pad=30)
fig.colorbar(surf, shrink=0.6, aspect=14, label="MB/s")

plt.tight_layout()
plt.show()
