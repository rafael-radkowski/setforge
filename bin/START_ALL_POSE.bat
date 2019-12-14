:: This batch file allows you to create a training dataset from scratch. 
:: It creates rendered images at a random pose and stackes them with a background image. 
:: Follow the instructions. 
:: The result is a Python .pickle file with a dictionary containg the image data.
:: Note that the pickle file only contains
:: 
:: rgb images
:: normal maps
:: pose data (translation, quaterion)
:: stored in a dictionary as Xtr, Xtr_norm, Ytr_pose, Ytr_roi, Xte, Xte_norm, Yte_pose, Yte_roi

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
SET cam_distance=3.0

:: 2. Set the temporary data folder and the output folder
::  Also, set the final pickle filename and path
SET temp_folder="./debug_pose"
:: Note that two different folders are required. 
SET output_folder="./debug_pose_4_512_512"

SET final_pickle_path="./debug_pose/final.pickle"

:: 3. Set the path to all background RGB images.
:: WRITE 'NONE' IF YOU DO NOT LIKE TO COMBINE THE RENERING WITH RANDOM BACKGROUND IMAGES 
SET bg_image_path="../data/imagenet"

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
set method=POSE

:: if set to 1, the object will only be rendered in its upright position. 
set upright=1

:: 6. Set the polyhedron sub-level. This will define the number of images to render
:: The higher the number, the more images. I tested to 6
set sublevel=5

:: 7. Set the number of rendered images you like to generate
set ren_num=2000

:: 8. Set the boundaries for the object to be moved around in front of the camera.
:: The position of the model is selected by random within the given range. 
set limx=0.6
set limy=0.6
set near=2.0
set far=4.0

:: 9. Set the number of final images you like to generate
set num=10000

:: 10. Activate noise by setting the variable to 1. 
set with_noise=1
set noise_dev=0.15

:: 11. Enable chromatic adaptation
set with_chromatic=1

:: 12. Set the percentage of images that should go into the cross-validation test file
:: as a percentage of num, in a range from [0,1] with 0->0%, 0.1-> 10%, 1.0 -> 100%
set x_test=0.1

:: THAT'S IT
::----------------------------------------------------------------------------------

set arg1=%model% -o %temp_folder% -img_w %img_w% -img_h %img_h% -m %method% -sub %sublevel%  -rad %cam_distance% -num %ren_num% -limx %limx% -limy %limy% -lim_near %near% -lim_far %far% 
if %upright% == 1 set arg1=%arg1% -up

set arg2=-n %num% -ipath %bg_image_path% -itype jpeg -rlog %temp_folder%/render_log.csv -img_w %img_w% -img_h %img_h% -o %output_folder% 
IF %with_noise% == 1 set arg2=%arg2% -noise %noise_dev%
IF %with_chromatic% == 1 set arg2=%arg2% -chromatic 

set arg3=-i %output_folder%/render_log.csv -o %final_pickle_path% -d ../bin -r %rows% -c %cols% -x %x_test%


setforge_r.exe %arg1%
setforge_g.exe %arg2%
Python.exe ../python_src/Image2Pickle.py %arg3%

pause
:: setforge_r.exe ../data/stanford_bunny_02_rot.obj -o output_4  -img_w 1280 -img_h 1024 -m POLY -sub 4  -rad 1.3 -verbose
:: setforge_g.exe  -n 10000 -ipath ../data/imagenet -itype jpeg -rlog ./output_4/render_log.csv -img_w 512 -img_h 512 -o ./batch_4_512_512 
:: Python.exe ../python_src/Image2Pickle.py -i batch_4_256_256/batch.csv -o batch_4_256_256.pickle -d ../bin -r 128 -c 128 -x 0.1