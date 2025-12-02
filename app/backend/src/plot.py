import numpy as np
import matplotlib.pyplot as plt

spec = np.loadtxt("spectrogram.csv", delimiter=",")

# Clip to a dynamic range
spec = np.clip(spec, -80, 0)  # keep between -80 dB and 0 dB

plt.imshow(spec.T, 
           origin="lower", 
           aspect="auto", 
           cmap="inferno")  # dark background, bright highlights
plt.colorbar(label="dB")
plt.xlabel("Time frames")
plt.ylabel("Frequency bins")
plt.title("Spectrogram")
plt.show()
