# DNNHelpers
Helper tools for depth neural network research. The tools allow on to generate artificial training data
to train a convolutional neural network.

This repository provides two tools:  
1. DatasetRenderer
2. ImageGen(enerator)

Additionally, the Python folder python_src includes a script *Image2Pickle.py*, which packs all image into a dictionary and saves it as a .pickle file.

Standard usage:
1. Find the 3D model you intend to train.
2. Create artificial datasets with DatasetRenderer.
3. Combine them with random background images.
4. Create a pickle file and  
start training.

### Prerequisites
All code runs (tested) on Windows 10, Version 1803.
It requires the following 3rd party tools:
 * [OpenCV v3.4.5] (https://opencv.org)
 * [Eigen3 v3.3.7] (http://eigen.tuxfamily.org)
 * [GLEW v2.1.0] (http://glew.sourceforge.net)
 * [GLFW v3.2.1] (https://www.glfw.org)
 * [glm, v0.9.9.3] (https://glm.g-truc.net/0.9.9/index.html)

 The code was prepared with Visual Studio 2017.

## DatasetRenderer
Allows one to render RGB images, normal maps, and depth images from 3D models.
The 3D model is automatically rotated using a polyhedron to control the angle.
A documentation can be found on Google Docs (ongoing work): [DatasetRenderer Introduction](https://docs.google.com/document/d/1aDyw3eXaGTcLeBlfpghiZg230qUybzcgZsdz7eNEcqg/edit?usp=sharing)

![Figure 1: a) a RGB image (RGB 8bit), b) a normal map image (xyz short, 16 bit), c) and a depth map image (depth, short 16 bit).](https://github.com/rafael-radkowski/DNNHelpers/blob/master/doc/images/Dataset_renderer_result.png)
Figure 1: a) a RGB image (RGB 8bit), b) a normal map image (xyz short, 16 bit), c) and a depth map image (depth, short 16 bit).


### Usage
 The software uses command line options to control its features. Example:

```
 DatasetRenderer.exe ../data/stanford_bunny_02_rot.obj -o output  -img_w 1280 -img_h 1024 -m POLY -sub 2  -rad 1.3 -verbose
```
See help ``` DatasetRenderer.exe  -h  ``` or the manual for further instructions.


## ImageGen
ImageGen generates training data by selecting random background images, e.g. from ImageNet,
and by combining these images with a rendering that DatasetRenderer generated.
Documentation can be found on Google Docs (currently incomplete)  
[ImageGen Documentation](https://docs.google.com/document/d/11cj3LnpoDvg_DU1jRfriQeV9HwE6V29O1h4cRDKk1tA/edit?usp=sharing)

Features:
 * Selects images by random.
 * Estimates a normal map from RGB data.
 * Determines the region of interest of the rendered object.
 * Changes image size if necessary.
 * Writes a log file.


![Figure 2: Left, a rendered Stanford bunny appears on top of a random background image. Right: The tool estimates normal vectors from the RGB image and renders the Stanford bunny normals on top of this normal map.](https://github.com/rafael-radkowski/DNNHelpers/blob/master/doc/images/ImageGen_result.png =640)  
Figure 2: Left, a rendered Stanford bunny appears on top of a random background image. Right: The tool estimates normal vectors from the RGB image and renders the Stanford bunny normals on top of this normal map.

### Usage  
The software uses command line options to control its features. Example:
```
ImageGen.exe -ipath ../data/imagenet -itype jpeg -rlog ./output/render_log.csv -img_w 512 -img_h 512 -o ./batch
```



Rafael Radkowski  
Iowa State University  
rafael@iastate.edu  
MIT license  

Last edited: Feb 8, 2019
