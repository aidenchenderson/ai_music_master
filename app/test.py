import numpy as np
import matplotlib.pyplot as plt

# Load your CSV
mel = np.loadtxt("mel_spectrogram.csv", delimiter=",")

plt.figure(figsize=(10, 4))
plt.imshow(mel.T, origin='lower', aspect='auto', cmap='magma')
plt.colorbar(format="%+2.0f dB")
plt.xlabel("Frame")
plt.ylabel("Mel Bin")
plt.title("Mel Spectrogram")
plt.tight_layout()
plt.savefig("mel_spectrogram.png", dpi=300)
plt.show()