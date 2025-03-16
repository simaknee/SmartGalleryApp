# Smart Gallery App
## Overview
This program is an application that allows users to define arbitrary categories and automatically classify the gallery according to those categories using the Few-shot classification method.

It leverages the onnxruntime library provided by the NNE (Neural Network Engine) module in Unreal Engine 5 to load a deep learning model converted to ONNX format. The model is trained and converted to ONNX in the [CustomizableSmartPhotoGallery](https://github.com/simaknee/CustomizableSmartPhotoGallery) repository.

## Development Environment
- Unreal Engine 5.5.3
- Windows 10
- Visual Studio 2022
## How to Use
1. Download the packaged app from the release link and extract the files.
2. Run <b>SmartGalleryApp.exe</b> in the folder.
![Image](https://github.com/user-attachments/assets/5c2cea69-3f92-4954-aa49-37a84d153a2d)
3. Click the <b>Select Folder</b> button in the top left corner and select the gallery folder containing the images you want to classify.
![Image](https://github.com/user-attachments/assets/3573f5bd-18dc-47fb-b958-4e3eb33e99d6)
4. Click the <b>Add Category</b> button, enter the name of the new category, and create the category.
![Image](https://github.com/user-attachments/assets/2a2a2af8-3def-463d-8f6c-c87939fa740e)
5. For each category, click the <b>Add Images</b> button to add one or more example images that represent the category.
![Image](https://github.com/user-attachments/assets/668f66d2-0917-4343-8d62-a15274153f27)
6. Click the <b>Classify</b> button to start the classification process.
![Image](https://github.com/user-attachments/assets/d994469e-a6b9-4493-8a85-75f8e73ff8ca)
7. After the process is complete, visit your gallery folder to check the classification results.
![Image](https://github.com/user-attachments/assets/3516f39d-64ac-4062-9d1f-309bc7c0dcdc)