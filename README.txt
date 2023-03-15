Readme:

For paper:"Computer-assisted quantitative framework for whole slide cervical image grading driven by time series features"

This repository includes code and a test dataset including various deep learning models: proposed attention module, lstm-fcn, MicroScope, pix2pix, and ScreeningCells (quantitative analysis as well as Matthew effect). These models have different use cases in the proposed two-layer detection framework:

Attention: This model is used to improve the performance of neural networks by allowing the network to focus on specific parts of the input.

LSTM-FCN: This model is used for time-series classification tasks by combining Long Short-Term Memory (LSTM) and Fully Convolutional Networks (FCN) to capture both temporal and spatial features.

MicroScope: This model is used for smear collecting and other microorganisms in microscope images. It uses a combination of deep learning and image processing techniques.

Pix2pix: This model is used for image-to-image translation tasks, such as converting a grayscale image to a color image.

ScreeningCells: This model is used for the automated screening of cells in medical images, such as identifying cancerous cells.

The test dataset includes several files for testing the performance of the proposed models. Please refer to the specific model's structure for details on how to use the code and test the model on the provided dataset. Due to the confidential principles for raw data sharing, detailed data could be available by requesting the corresponding author (dongyaojia1974@163.com).