import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'

import numpy as np
import pandas as pd
import librosa
import pickle
import tensorflow as tf
import keras
import argparse

import warnings
from sklearn.exceptions import InconsistentVersionWarning
warnings.filterwarnings("ignore", category=InconsistentVersionWarning)

# model paths
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

MODEL_PATH = os.path.join(BASE_DIR, "models", "genre_classification_cnn.keras")
ENCODER_PATH = os.path.join(BASE_DIR, "models", "genre_label_encoder.pkl")

# audio parameters
SR = 48000
CHUNK_DURATION = 3
N_MELS = 40
N_FFT = 2048
HOP_LENGTH = 512

# load models
if not os.path.exists(MODEL_PATH):
    raise FileNotFoundError(f"Model not found at {MODEL_PATH}")

if not os.path.exists(ENCODER_PATH):
    raise FileNotFoundError(f"Encoder not found at {ENCODER_PATH}")

model = keras.models.load_model(MODEL_PATH, compile=False)

with open(ENCODER_PATH, "rb") as f:
    label_encoder = pickle.load(f)

# feature extraction
def extract_mel_spectrogram(signal, sr):
    mel = librosa.feature.melspectrogram(y=signal, sr=sr, n_mels=N_MELS, n_fft=N_FFT, hop_length=HOP_LENGTH)
    mel_db = librosa.power_to_db(mel, ref=np.max)

    return mel_db.astype(np.float32)

# genre classification prediction
def predict_genre(audio_path):

    if not os.path.exists(audio_path):
        raise FileNotFoundError(f"Audio file not found: {audio_path}")

    signal, _ = librosa.load(audio_path, sr=SR, mono=True, duration=CHUNK_DURATION)

    mel_spec = extract_mel_spectrogram(signal, SR)

    mel_spec = mel_spec[np.newaxis, ..., np.newaxis]

    mel_spec = mel_spec.astype(np.float32)

    predictions = model.predict(mel_spec, verbose=0)

    predicted_index = np.argmax(predictions, axis=1)[0]
    confidence = float(np.max(predictions))

    predicted_genre = label_encoder.inverse_transform([predicted_index])[0]

    return predicted_genre, confidence

def csv_to_mel_input(csv_path):
    # read numeric CSV, skip header if present
    df = pd.read_csv(csv_path, header=0)
    mel = df.apply(pd.to_numeric, errors='coerce').to_numpy(dtype=np.float32)

    # transpose: rows = n_mels, cols = time steps
    mel = mel.T  # shape becomes (40, time_steps)

    # optionally, crop/pad to model's expected time_steps
    target_time_steps = 282
    if mel.shape[1] > target_time_steps:
        mel = mel[:, :target_time_steps]  # crop extra frames
    elif mel.shape[1] < target_time_steps:
        # pad with zeros
        pad_width = target_time_steps - mel.shape[1]
        mel = np.pad(mel, ((0, 0), (0, pad_width)), mode='constant', constant_values=0)

    # add batch and channel dimensions
    mel = np.expand_dims(mel, axis=0)   # batch dimension
    mel = np.expand_dims(mel, axis=-1)  # channel dimension

    return mel.astype(np.float32)

def predict_genre_from_csv(csv_path):
    mel_input = csv_to_mel_input(csv_path)
    predictions = model.predict(mel_input, verbose=0)
    
    predicted_index = np.argmax(predictions, axis=1)[0]
    confidence = float(np.max(predictions))
    
    predicted_genre = label_encoder.inverse_transform([predicted_index])[0]
    return predicted_genre, confidence

# application entry point
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", type=str, help="Path to CSV file")
    args = parser.parse_args()

    if args.csv:
        genre, confidence = predict_genre_from_csv(args.csv)
        # line produced to std out is read by C++ through IPC
        print(f"{genre},{confidence:.4f}")