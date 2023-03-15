import os
os.environ['KMP_DUPLICATE_LIB_OK']='TRUE'
import torch
import torch.nn as nn
from torchvision import models, transforms
from PIL import Image
import matplotlib.pyplot as plt
import numpy as np
import cv2

# Define the transforms for preprocessing input image
transform = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
])

# Define the model and load the pre-trained weights
model = models.resnet50(pretrained=True)
# Replace the last layer with an identity function
model.fc = nn.Identity()

# Set the model to evaluation mode
model.eval()

# Get the image and preprocess it
img = Image.open('SZ34666.jpg')
img_tensor = transform(img)
img_tensor = img_tensor.unsqueeze(0)

# Generate the feature maps and the attention scores
with torch.no_grad():
    feature_maps = []
    features = model.conv1(img_tensor)
    feature_maps.append(features)
    for module in model.layer1:
        features = module(features)
        feature_maps.append(features)
    for module in model.layer2:
        features = module(features)
        feature_maps.append(features)
    for module in model.layer3:
        features = module(features)
        feature_maps.append(features)
    for module in model.layer4:
        features = module(features)
        feature_maps.append(features)

    # Generate the attention map by averaging the feature maps
    attention_maps = []
    for fmap in feature_maps:
        attention = torch.mean(fmap, dim=1, keepdim=True)
        # Normalize the attention scores
        attention = nn.functional.relu(attention)
        attention /= torch.max(attention)
        attention_maps.append(attention)

# Resize the attention maps to match the input image size
attention_maps_resized = []
for attention_map in attention_maps:
    attention_map_resized = nn.functional.interpolate(attention_map, size=(224, 224), mode='bilinear', align_corners=False)
    # Convert the attention map to a grayscale numpy array
    attention_map_np = attention_map_resized.squeeze().cpu().numpy()
    attention_map_np = (255 * attention_map_np).astype(np.uint8)
    attention_maps_resized.append(attention_map_np)

# Convert the image tensor to a numpy array
img_np = img_tensor.squeeze().permute(1, 2, 0).cpu().numpy()
# Resize the image to match the attention map size
img_resized = cv2.resize(img_np, (224, 224))

# Apply the attention maps to the original image
attention_images = []
for attention_map_np_resized in attention_maps_resized:
    heatmap = cv2.applyColorMap(attention_map_np_resized, cv2.COLORMAP_JET)
    attention_image = heatmap * 0.3 + img_resized * 0.5
    attention_images.append(attention_image)

# Plot the attention maps and the original image
fig, axes = plt.subplots(ncols=6, figsize=(24, 4))
axes[0].imshow(img)
axes[0].set_title('Original Image')
for i in range(5):
    axes[i+1].imshow(attention_images[i].astype(np.uint8))
    axes[i+1].set_title(f'Attention Map {i+1}')
plt.show()
