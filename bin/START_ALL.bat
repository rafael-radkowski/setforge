:: This batch file allows you to create a training dataset from scratch. 
:: Follow the instructions. 
:: The result is a Python .pickle file with a dictionary containg the image data.

:: Note that the path to Python.exe needs to be set in PATH

:: Rafael Radkowski
:: Iowa State University
:: rafael@iastate.edu

:: FOLLOW THE INSTRUCTIONS
:: Note that batch does not tolerate spaces
:: Also, mind spaces behind the lines. 
echo off

:: 1. Set the relative path to the 3D model as well as the distance between camera and model
SET model=../data/stanford_bunny_02_rot.obj
SET cam_distance=1.3

:: 2. Set the temporary data folder and the output folder
::  Also, set the final pickle filename and path
SET temp_folder="./debug_script"
SET output_folder="./debug_script_4_512_512"

SET final_pickle_path="./debug_script/batch.pickle"

:: 3. Set the path to all background RGB images.
:: WRITE 'NONE' IF YOU DO NOT LIKE TO COMBINE THE RENERING WITH RANDOM BACKGROUND IMAGES 
SET bg_image_path=NONE

:: 4. Set the image width and image height in pixel
set img_w=512
set img_h=512

:: 5. Set the image width and height in pixels for the final pickle file
:: Note that this size can vary from the previous one, e.g., if you intend to reuse the images with a different resolution. 
:: (saves time)
set rows=128
set cols=128

:: 6. Set the camera path method. 
:: POLY for polyhedron image
:: POSE if a random pose should be selected. 
set method=POLY

:: 6. Set the polyhedron sub-level. This will define the number of images to render
:: The higher the number, the more images. I tested to 6
set sublevel=4

:: 8. Set the number of final images you like to generate
set num=1000

:: 9. Set the percentage of images that should go into the cross-validation test file
:: as a percentage of num, in a range from [0,1] with 0->0%, 0.1-> 10%, 1.0 -> 100%
set x_test=0.1

:: THAT'S IT
::----------------------------------------------------------------------------------

set arg1=%model% -o %temp_folder% -img_w %img_w% -img_h %img_h% -m %method% -sub %sublevel%  -rad %cam_distance%
set arg2=-n %num% -ipath %bg_image_path% -itype jpeg -rlog %temp_folder%/render_log.csv -img_w %img_w% -img_h %img_h% -o %output_folder% 
set arg3=-i %output_folder%/batch.csv -o %final_pickle_path% -d ../bin -r %rows% -c %cols% -x %x_test%


DatasetRenderer.exe %arg1%
ImageGen.exe %arg2%
Python.exe ../python_src/Image2Pickle.py %arg3%

pause
:: DatasetRenderer.exe ../data/stanford_bunny_02_rot.obj -o output_4  -img_w 1280 -img_h 1024 -m POLY -sub 4  -rad 1.3 -verbose
:: ImageGen.exe  -n 10000 -ipath ../data/imagenet -itype jpeg -rlog ./output_4/render_log.csv -img_w 512 -img_h 512 -o ./batch_4_512_512 
:: Python.exe ../python_src/Image2Pickle.py -i batch_4_256_256/batch.csv -o batch_4_256_256.pickle -d ../bin -r 128 -c 128 -x 0.1