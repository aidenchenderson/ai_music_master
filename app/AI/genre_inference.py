import os
import numpy as np
import librosa
import pickle
import tensorflow as tf
import keras 

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
print("Model loaded")

with open(ENCODER_PATH, "rb") as f:
    label_encoder = pickle.load(f)

print("Label encoder loaded")

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

# application entry point
if __name__ == "__main__":

    audio_file = os.path.join(BASE_DIR, "sample_audio.mp3")

    if not os.path.exists(audio_file):
        print(f"Audio file not found: {audio_file}")
    else:
        genre, confidence = predict_genre(audio_file)
        print(f"Predicted genre: {genre}")
        print(f"Confidence: {confidence:.4f}")