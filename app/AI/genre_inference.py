import numpy as np
import librosa
import pickle
import tensorflow as tf

MODEL_PATH = "/content/drive/MyDrive/genre_cnn_model2.keras"
ENCODER_PATH = "/content/drive/MyDrive/genre_label_encoder2.pkl"

SR = 48000
CHUNK_DURATION = 3
N_MELS = 40
N_FFT = 2048
HOP_LENGTH = 512


model = tf.keras.models.load_model(MODEL_PATH)
print("Model loaded")

with open(ENCODER_PATH, "rb") as f:
    label_encoder = pickle.load(f)

print("Label encoder loaded")

def extract_mel_spectrogram(signal, sr):
    mel = librosa.feature.melspectrogram(
        y=signal,
        sr=sr,
        n_mels=N_MELS,
        n_fft=N_FFT,
        hop_length=HOP_LENGTH
    )
    mel_db = librosa.power_to_db(mel, ref=np.max)
    return mel_db

def predict_genre(audio_path):
    signal, _ = librosa.load(
        audio_path,
        sr=SR,
        mono=True,
        duration=CHUNK_DURATION
    )

    mel_spec = extract_mel_spectrogram(signal, SR)

    mel_spec = mel_spec[np.newaxis, ..., np.newaxis]
  
    predictions = model.predict(mel_spec)
    predicted_index = np.argmax(predictions, axis=1)[0]

    predicted_genre = label_encoder.inverse_transform([predicted_index])[0]

    return predicted_genre

if __name__ == "__main__":

    audio_file = "/content/drive/MyDrive/CustomDatasetForMacAI/country/country4.mp3"

    genre = predict_genre(audio_file)

    print("Predicted genre:", genre)
