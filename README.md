# Setforge
Setforge is a set of helper tools for depth neural network research. The tools allow on to generate artificial training data
from models and to augment this data. The intend is to train a convolutional neural networks.

![Title figure](https://github.com/rafael-radkowski/DNNHelpers/blob/master/doc/images/setforge_social_logo_small.png)

This repository provides two tools:  
1. setforge_r(enderer)
2. setforge_g(enerator)

Additionally, the Python folder python_src includes a script *Image2Pickle.py*, which packs all image into a dictionary and saves it as a .pickle file.

Standard usage:
1. Find the 3D model you intend to train.
2. Create artificial datasets with setforge_r.
3. Combine them with random background images and augment the data with setforge_g.
4. Create a pickle file and  
start training.

### Prerequisites
All code runs (tested) on Windows 10, Version 1803.
It requires the following 3rd party tools:
 * [OpenCV v3.4.5](https://opencv.org)
 * [Eigen3 v3.3.7](http://eigen.tuxfamily.org)
 * [GLEW v2.1.0](http://glew.sourceforge.net)
 * [GLFW v3.2.1](https://www.glfw.org)
 * [glm, v0.9.9.3](https://glm.g-truc.net/0.9.9/index.html)

 The code was prepared with Visual Studio 2017.

## Setforge_r
Allows one to render RGB images, normal maps, and depth images from 3D models.
The 3D model is automatically rotated using a polyhedron to control the angle.
A documentation can be found on Google Docs (ongoing work): [Setforge_g Introduction](https://docs.google.com/document/d/1aDyw3eXaGTcLeBlfpghiZg230qUybzcgZsdz7eNEcqg/edit?usp=sharing)

Further details about specific functions are also available on Google Docs: [Setforge_g Manual](https://docs.google.com/document/d/1xMMXdtCLFCR4rh1uQc7IpIVlAw9hsM409nWJOY54mX0/edit?usp=sharing)

![Figure 1: a) a RGB image (RGB 8bit), b) a normal map image (xyz short, 16 bit), c) and a depth map image (depth, short 16 bit).](https://github.com/rafael-radkowski/DNNHelpers/blob/master/doc/images/Dataset_renderer_result.png)
Figure 1: a) a RGB image (RGB 8bit), b) a normal map image (xyz short, 16 bit), c) and a depth map image (depth, short 16 bit). The application also provides a image mask for semantic segmentation, etc. (not in this image).


### Usage
 The software uses command line options to control its features. Example:

```
 DatasetRenderer.exe ../data/stanford_bunny_02_rot.obj -o output  -img_w 1280 -img_h 1024 -m POLY -sub 2  -rad 1.3 -verbose
```
See help ``` DatasetRenderer.exe  -h  ``` or the manual for further instructions.

Note that the software has two essential options, which one can control via the -m option: ```-m POLY``` and ```-m POSE```; additionally, one manual mode is available with ```-m USER```. 
The first option (```POLY```) keeps the 3D model at its centroid. Only the camera position changes per rendering. Thus, the 3D model is rendered from different orientations only. The number of images to generate depends on the polyhedron level (```-sub``` ).  
The option ```POSE``` also generates a random orientation of the 3D model. Additinally, this mode place the 3D model at a random position in front of the camera. The number of images to be generated can be set with ```-num```. The position limits can be set via command line options, see the manual or use ```-h``` for instructions.  

![Figure 2: Option -m POSE generates a random pose for each object. ).](https://github.com/rafael-radkowski/DNNHelpers/blob/master/doc/images/pose_renderer_result.png)
Figure 2: Option -m POSE generates a random pose for each object.

```
 DatasetRenderer.exe ../data/stanford_bunny_02_rot.obj -o debug_pose -img_w 512 -img_h 512 -m POSE -num 5000 -sub 4 -limx 0.6 -limy 0.6 -lim_near 1.0 -lim_far 2.5 -verbose
```
The parameters limx, limy, lim_near, lim_far control the size of the box in which the object can move in front of the camera. 

The ```USER``` mode allows a user to select the camera position iteractively. Press the 'w' key to store the image set to the selected folder. s

### Output
 * RGB rendering (CV_8UC3, 24 bit)
 * Normal map (CV_16UC3, 48 bit)
 * Depth map (CV_16UC1, 16 bit)
 * Image mask (CV_8UC1, 8bit)
 * Pose (quaternion, translation)

## Setforge_g
ImageGen generates training data by selecting random background images, e.g. from ImageNet,
and by combining these images with a rendering that DatasetRenderer generated.
Documentation can be found on Google Docs (currently incomplete)  [ImageGen Documentation](https://docs.google.com/document/d/11cj3LnpoDvg_DU1jRfriQeV9HwE6V29O1h4cRDKk1tA/edit?usp=sharing)

Features:
 * Selects images by random.
 * Estimates a normal map from RGB data.
 * Determines the region of interest of the rendered object.
 * Changes image size if necessary.
 * Writes a log file.


![Figure 3: Left, a rendered Stanford bunny appears on top of a random background image. Right: The tool estimates normal vectors from the RGB image and renders the Stanford bunny normals on top of this normal map.](https://github.com/rafael-radkowski/DNNHelpers/blob/master/doc/images/ImageGen_result.png)  
Figure 3: Left, a rendered Stanford bunny appears on top of a random background image. Right: The tool estimates normal vectors from the RGB image and renders the Stanford bunny normals on top of this normal map.

### Usage  
The software uses command line options to control its features. Example:
```
ImageGen.exe -ipath ../data/imagenet -itype jpeg -rlog ./output/render_log.csv -img_w 512 -img_h 512 -o ./batch
```

## Example dataset

Figure 3 shows some rendered results combinded with random background images. Some datasets (as pickle files) are available on [http://public.vrac.iastate.edu/~rafael/dataset/](http://public.vrac.iastate.edu/~rafael/dataset/)

![Figure 3: An example dataset. ](https://github.com/rafael-radkowski/DNNHelpers/blob/master/doc/images/datasets_example.png)

Figure 3: An example dataset.

Rafael Radkowski  
Iowa State University  
rafael@iastate.edu  
MIT license  

Last edited: April 21, 2020
