# Research and Development (R&D)

This directory contains the Jupyter Notebook and framework used to validate ideas before being integrated within the application. 

While none of the application's functionality can be found here and the corresponding closed-source dataset is not provided, the general architecture can be followed and replicated to achieve similar results.

## Objective
Our goal was to develop a deep learning pipeline for music genre classification using residual neural networks.

## The Pipeline
1. Standardization: Audio is preprocessed from raw data into standardized chunks, increasing dataset size and capturing musical patterns.
2. Feature Extraction: Mel-scaled spectrograms are then used for feature extraction to represent audio in a way that mimics human perception. This makes it easier for the CNN to "hear" the melody, rhythm, etc.
3. Architecture: The TensorFlow/Keras ResNet architecture tailors results for multi-class genre classification (country, rock, pop, etc.). The model utilizes residual blocks with skip connections. This allows the model to learn "identity mappings," preventing the vanishing gradient problem and allowing the network to become significantly deeper without losing performance.
4. Validation: Performance is verified through stratified 80/20 splitting and batch normalization for training stability.