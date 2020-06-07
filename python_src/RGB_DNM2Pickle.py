"""
class RGBNM2Pickle

This file prepares a pickle dataset using the results the program ImageGen produces.
This script bundles
- RGB images, uint8 [0,255]
- Normal maps, uint16 [0,1]
- Depth maps, uint16 [0,1]
- Image masks, uint16 [0,1]
- The image pose
- and the roi

The pickle contains a dict with the following labels:
    Xtr - the training data set, rgb images of shape (num_images, width, height, channels = 3), uint8 [0,255]
    Xtr_depth - depth images for training of shape (num_images, width, height, channels = 3), uint16
    Xtr_norm - the training data set, normal maps images of shape (num_images, width, height, channels = 3). Values are stored of type uint16 [0,1]
    Xtr_mask - the training mask, each pixel in indicates the location of the object.
    Ytr_pose - the training poses of each object stored as (num_images, x, y, z, qx, qy, qz, qw)
    Ytr_roi - the training region of interest for each object stored as (num_images, rx, ry, dx, dy)
    Xte - the test data set, rgb images of shape (num_images, width, height, channels = 3)
    Xte_depth - depth images for testing of shape (num_images, width, height, channels = 3), uint16
    Xte_norm - the test data set, normal map images of shape (num_images, width, height, channels = 3). Values are stored of type uint16 [0,1]
    Xte_mask - the test mask, with each pixel indicating the location of the object.
    Yte_pose - the test poses of each object stored as (num_images, x, y, z, qx, qy, qz, qw)
    Yte_roi - the test region of interest for each object stored as (num_images, rx, ry, dx, dy)


Note that this script works with temporary pickle files to store data while process. All temp files will be deleted when done.

This script was developed and tested with Python 3.7 on Windows 10

The script needs to be configured via command line options:
-i  the path and file describing the input data. This file can be generated with ImageGen.exe
-o  the output pickle file path and name
-d  the working directory. This is suppose the start directory from which ImageGen.exe (the file in -i) points to the source images
-r  the dst number of image rows
-c  the dst number of image columns
-x  the cross-validation ratio between training and test files. Num test files = x all_files

Example:
    Python.exe ../python_src/Image2Pickle.py -i batch_4_256_256/batch.csv -o batch_4_256_256.pickle -d ../bin -r 128 -c 128 -x 0.1

Rafael Radkowski
Iowa State University
rafael@iastate.edu
May 3rd, 2019
MIT license
-------------------------------------
Last edited:

June 6, 2020, RR
- Added the new log file entry to the data. The log file also contains the filename to a file that stores projected corner points. 

"""

import cv2
import numpy as np
import pickle
import csv
import sys
import getopt
import random
import os

class RGB_DNM2Pickle:

    # Percentage of the dataset used for validation
    ratio_validation = 0.1


    csv_dataset = [] # list to store the scv file content
    N = 0 # number of lines read from csv

    test_data = []
    train_data = []

    dst_height = 128 # destination size of the image
    dst_width = 128

    def __init__(self, height = 128, width = 128, ratio = 0.1):
        if height > 2:
            self.dst_height = height
        if width > 2:
            self.dst_width = width

        self.setTrain2TestRatio(ratio)


    def process(self, src_file, dst_file, working_directory):
        """
        Process the RGB data.
        This function loads the dataset description from a csv file (src_file),
        splits the data into a training and a test dataset using the  variable ratio_validation to determine the size of both sets.
        It further loads all RGB images and writes everything into a pickle file. The pickle file contains a dict
        with  Xtr, Ytr_pose, Ytr_roi, Xte, Yte_pose, Yte_roi.
        :param src_file: string containing the path and file with the source data description.
        :param dst_file: the destination pickle file for all data
        :param working_directory: the working dir. This is the start dir from which src_file points to the data
        :return: -
        """
        # Read all lines from the csv file
        path = working_directory + "/" + src_file

        try:
            with open(path, mode='r') as csv_file:
                csv_reader = csv.DictReader(csv_file)
                line_count = 0
                print("[INFO] - Reading content from ", path )
                for row in csv_reader:
                    if line_count == 0:
                        #print(f'Column names are {", ".join(row)}')
                        line_count += 1

                    #index, rgb_file, normals_file, depth_file, mask_file, mat_file, tx, ty, tz, qx, qy, qz, qw, roi_x,  roi_y, dx, dy
                    #print(row['index'], ":", row['rgb_file'], ", ", row['normals_file'], ", ", row['depth_file'])
                    line_count += 1

                    data = {
                        "index": row['index'],
                        "rgb_file": row['rgb_file'],
                        "normals_file": row['normals_file'],
                        "depth_file": row['depth_file'],
                        "mask_file": row['mask_file'],
                        "mat_file": row['mat_file'],
                        "tx": float(row['tx']),
                        "ty": float(row['ty']),
                        "tz": float(row['tz']),
                        "qx": float(row['qx']),
                        "qy": float(row['qy']),
                        "qz": float(row['qz']),
                        "qw": float(row['qw']),
                        "rx": float(row['roi_x']),
                        "ry": float(row['roi_y']),
                        "dx": float(row['roi_w']),
                        "dy": float(row['roi_h']),
                        "cp_file" : row['cp_file']
                    }

                    self.csv_dataset.append(data)
                self.N = len(self.csv_dataset)
                print(f'[INFO] - Read {self.N} lines.')  # -1, to not count the header line
        except IOError:
            print(f'[ERROR] - Cannot open csv file {path}. Check the folder')

        if self.N == 0:
            print(f'[ERROR] - No datasets read (Exit 0)')

        # Split the lines by random into two sets
        num_test = int(len(self.csv_dataset) * self.ratio_validation)
        num_train = int(len(self.csv_dataset) - num_test)
        print(f'[INFO] - Selecting {num_test} datasets for validation.')  # -1, to not count the header line

        random.seed()
        te_index = random.sample(range(1, self.N), num_test)

        for i in te_index: # do not remove items from csv_dataset in the same process. The index must be maintained
            self.test_data.append(self.csv_dataset[i])

        for i in self.test_data: # removing items.
            self.csv_dataset.remove(i)

        self.train_data = self.csv_dataset;

        print(f'[INFO] - Found {len(self.train_data)} training data sets.')
        print(f'[INFO] - Found {len(self.test_data)} test data sets.')


        # prepare the training  data set
        Xtr, Xtr_norm, Xtr_depth, Xtr_mask, Ytr_pose, Ytr_roi = self.__prepare_data(self.train_data, working_directory )
        all_data = dict()
        all_data["Xtr"] = Xtr
        all_data["Xtr_norm"] = Xtr_norm
        all_data["Xtr_depth"] = Xtr_depth
        all_data["Ytr_pose"] = Ytr_pose
        all_data["Ytr_roi"] = Ytr_roi
        all_data["Xtr_mask"] = Xtr_mask

        print("[INFO] - Storing data")

        # store the data
        pickle_out = open(dst_file, "wb")
        pickle.dump(all_data, pickle_out)
        pickle_out.close()
        all_data.clear()

        print(f'[INFO] - Stored all data in {dst_file}')

        # prepare the test dataset
        Xte, Xte_norm, Xte_depth, Xte_mask,  Yte_pose, Yte_roi = self.__prepare_data(self.test_data, working_directory)


        pickle_in = open(dst_file, "rb")
        mydata = pickle.load(pickle_in)
        mydata["Xte"] = Xte
        mydata["Xte_norm"] = Xte_norm
        mydata["Xte_depth"] = Xte_depth
        mydata["Yte_pose"] = Yte_pose
        mydata["Yte_roi"] = Yte_roi
        mydata["Xte_mask"] = Xte_mask
        pickle_in.close()

        print("[INFO] - Storing data")

        # store the data
        pickle_out = open(dst_file, "wb")
        pickle.dump(mydata, pickle_out)
        pickle_out.close()

        print(f'[INFO] - Stored all data in {dst_file}')
        print('[DONE]')
        # test the file for the right size
        self.__checkFiles(dst_file, num_test, num_train )

    def setTrain2TestRatio(self, ratio):
        """
        Set the ratio between test and training images
        :param ratio: a value in the range between 0.05 and 0.7
        :return:
        """
        if ratio > 0.01 and ratio < 0.7:
            self.ratio_validation = ratio
        else:
            print(f'[ERROR] - The specified ratio of {ratio} is outside the allowed margin')

    def __prepare_data(self, data_tr, working_directory):
        """
        Prepare a data set. This function
        - loads the images
        - scales them to the destination size
        - concatenates the X data to a volume of size (num items, with, height, channels)
        - prepares the Y data to two volumes of size (num items, 7)  and (num items, 4)
        and returns it.
        Note that the data is temporary stored into multiple pickles in case the script crashes.
        Also, multiple small arrays speed up processing.
        :param data_tr:  a dict with the loaded csv data.
        :param working_directory: the start working directory in which the data is located
        :return: rgb_volume, the rgb image volume of size (num items, with, height, channels) ,
                 normal_volume,
                 mask_volume
                 pose_results, the pose results of size  (num items, 7) ,
                 roi_results, the roi results of size  (num items, 4)
        """
        print("\n[INFO] - Preparing")

        count  = 0
        rgb_volume = []
        normal_volume = []
        depth_volume = []
        mask_volume = []

        pose_results = []
        roi_results = []
        temp_count = 0


        for each in data_tr:
            path_rgb = working_directory + self.__checkFilename(each['rgb_file'])
            path_norm = working_directory + self.__checkFilename(each['normals_file'])
            path_depth = working_directory + self.__checkFilename(each['depth_file'])
            path_mask = working_directory + self.__checkFilename(each['mask_file'])

            # read the images
            rgb = cv2.imread(path_rgb)
            normals = cv2.imread(path_norm, cv2.IMREAD_COLOR | cv2.IMREAD_ANYDEPTH)  # uint16
            depth = cv2.imread(path_depth, cv2.IMREAD_GRAYSCALE | cv2.IMREAD_ANYDEPTH)  # uint16
            mask = cv2.imread(path_mask, cv2.IMREAD_UNCHANGED  | cv2.IMREAD_ANYDEPTH)

            # resize the images
            resized_rgb = cv2.resize(rgb, (self.dst_height, self.dst_width))
            resized_normals = cv2.resize(normals, (self.dst_height, self.dst_width))
            resized_mask = cv2.resize(mask, (self.dst_height, self.dst_width))
            resized_depth = cv2.resize(depth, (self.dst_height, self.dst_width))

            # change normal vector datatype
            # Do not do that. The file becomes too big
            # resized_normals_f = resized_normals.astype('float') / 65534.0

            # reshape them
            (h, w, c) = resized_rgb.shape
            (oh, ow, oc) = rgb.shape
            (mh, mw) = resized_mask.shape
            resized_rgb = resized_rgb.reshape([1, h, w, c])
            resized_normals = resized_normals.reshape([1, h, w, c])
            resized_mask = resized_mask.reshape([1, mh, mw, 1]) # mask has one channel
            resized_depth = resized_depth.reshape([1, h, w, 1])

            #rgb = cv2.rectangle(rgb, (int(each["rx"]), int(each["ry"])), (int(each["rx"] + each["dx"]), int(each["ry"] + each["dy"])), (0, 255, 0), 2)
            #cv2.imshow("out",rgb )
            #cv2.waitKey()

            ratio_x = float(oh) / float(self.dst_height)
            ratio_y = float(ow) / float(self.dst_width)

            # training result
            r0 = np.array([each["tx"], each["ty"], each["tz"], each["qx"], each["qy"], each["qz"], each["qw"]])
            r0 = r0.reshape([1, r0.shape[0]])
            r1 = np.array([each["rx"]/ratio_x, each["ry"]/ratio_y, each["dx"]/ratio_x, each["dy"]/ratio_y])
            r1 = r1.reshape([1, r1.shape[0]])

            if len(rgb_volume) == 0:
                rgb_volume = resized_rgb
                normal_volume = resized_normals
                depth_volume = resized_depth
                mask_volume = resized_mask
                pose_results = r0
                roi_results = r1
            else:
                rgb_volume = np.concatenate((rgb_volume, resized_rgb), axis=0)
                normal_volume = np.concatenate((normal_volume, resized_normals), axis=0)
                depth_volume = np.concatenate((depth_volume, resized_depth), axis=0)
                pose_results =  np.concatenate((pose_results, r0), axis=0)
                roi_results = np.concatenate((roi_results, r1), axis=0)
                mask_volume = np.concatenate((mask_volume, resized_mask), axis=0)
            count = count + 1

            if count%100 == 0:
                print(".", end=" ", flush=True)

            # write the data into temporary pickle file.
            # Python dramatically slows down if the array becomes too large.
            if count % 1000 == 0:
                print(f' {count}/{len(data_tr)}')
                # write a temporary file.
                temp_count = self.__writeTempFile(temp_count, rgb_volume, normal_volume, depth_volume, mask_volume, pose_results, roi_results)

                # empty the data
                rgb_volume = []
                normal_volume = []
                depth_volume = []
                pose_results = []
                roi_results = []
                mask_volume = []

            #cv2.imshow("rgb", rgb)
            #cv2.waitKey(0)

        # write the remaining files into a temporary pickle file.
        # This is necessary at this location if the number of images is not divisible by 1000
        if len(rgb_volume) > 0:
            print(f' {count}/{len(data_tr)}')
            temp_count = self.__writeTempFile(temp_count, rgb_volume, normal_volume, depth_volume, mask_volume, pose_results, roi_results)
            rgb_volume = []
            normal_volume = []
            depth_volume = []
            pose_results = []
            roi_results = []
            mask_volume = []


        # concatenate all temp pickle files
        if temp_count > 0:
            rgb_volume = []
            normal_volume = []
            depth_volume = []
            mask_volume = []
            pose_results = []
            roi_results = []
            for i in range(temp_count):
                path = "temp_" + str(i)
                pickle_in = open(path, "rb")
                data = pickle.load(pickle_in)
                if len(rgb_volume) == 0:
                    rgb_volume = np.array(data["X"])
                    normal_volume = np.array(data["X_norm"])
                    depth_volume = np.array(data["X_depth"])
                    pose_results = np.array(data["Y_pose"])
                    roi_results = np.array(data["y_roi"])
                    mask_volume = np.array(data["X_mask"])
                else:
                    rgb_volume = np.concatenate((rgb_volume, data["X"]), axis=0)
                    normal_volume = np.concatenate((normal_volume, data["X_norm"]), axis=0)
                    depth_volume = np.concatenate((depth_volume, data["X_depth"]), axis=0)
                    pose_results = np.concatenate((pose_results, data["Y_pose"]), axis=0)
                    roi_results = np.concatenate((roi_results, data["y_roi"]), axis=0)
                    mask_volume = np.concatenate((mask_volume, data["X_mask"]), axis=0)
                pickle_in.close()
                data = []
        else:
            rgb_volume = np.array(rgb_volume)
            normal_volume = np.array(normal_volume)
            depth_volume = np.array(depth_volume)
            pose_results = np.array(pose_results)
            roi_results = np.array(roi_results)
            mask_volume = np.array(mask_volume)


        # removing temporary files
        for i in range(temp_count):
            path = "temp_" + str(i)
            os.remove(path)

        print("", end="\n")
        print("[INFO] - Done processing")
        print("[INFO] - RGB volume shape: ", rgb_volume.shape)
        print("[INFO] - Normals volume shape: ", normal_volume.shape)
        print("[INFO] - Depth volume shape: ", depth_volume.shape)
        print("[INFO] - Mask volume shape: ", mask_volume.shape)
        print("[INFO] - Pose results shape: ", pose_results.shape)
        print("[INFO] - RoI results shape: ", roi_results.shape)

        return rgb_volume, normal_volume, depth_volume,  mask_volume, pose_results,  roi_results

    def __checkFilename(self, path):
        """
        Check the path for correct spelling.
        :param path: the imput path
        :return: a correctly formated output path
        """
        c = path[0]
        if c == '/':
            return path
        if c == '.':
            return self.__checkFilename(path[1:len(path)])
        else:
            dst_path = "/"
            dst_path =  dst_path + path
            return dst_path

    def __writeTempFile(self, index, rgb_volume, normal_volume, depth_volume,  mask_volume, pose, roi):
        """
        Write a dictionary to a temporary pickle file.
        :param index:
        :param rgb_volume:
        :param normal_volume:
        :param mask_volume:
        :param pose:
        :param roi:
        :return:
        """
        temp = dict()
        temp["X"] = rgb_volume
        temp["X_norm"] = normal_volume
        temp["X_depth"] = depth_volume
        temp["Y_pose"] = pose
        temp["y_roi"] = roi
        temp["X_mask"] = mask_volume

        path = "temp_" + str(index)
        self.__writeToPickle(temp, path)
        index = index + 1
        temp.clear()

        return index

    def __writeToPickle(self, data_dict, path):
        """
        Write a dictionary into a pickle file
        :param data_dict: the data dictionary
        :param path: relative of absolute path to the file
        :return:
        """
        pickle_out = open(path, "wb")
        pickle.dump(data_dict, pickle_out)
        pickle_out.close()

    def __checkFiles(self, path_and_file, num_test, num_train):

        print("[INFO] - Checking data.")
        pickle_in = open(path_and_file, "rb")
        data = pickle.load(pickle_in)

        error = 0

        if data["Xtr"].shape[0] != num_train:
            error = error + 1
        if data["Xtr_norm"].shape[0] != num_train:
            error = error + 1
        if data["Xtr_mask"].shape[0] != num_train:
            error = error + 1
        if data["Xtr_depth"].shape[0] != num_train:
            error = error + 1
        if data["Ytr_pose"].shape[0] != num_train:
            error = error + 1
        if data["Ytr_roi"].shape[0] != num_train:
            error = error + 1
        if data["Xte"].shape[0] != num_test:
            error = error + 1
        if data["Xte_norm"].shape[0]!= num_test:
            error = error + 1
        if data["Xte_depth"].shape[0] != num_test:
            error = error + 1
        if data["Xte_mask"].shape[0] != num_test:
            error = error + 1
        if data["Yte_pose"].shape[0]!= num_test:
            error = error + 1
        if data["Yte_roi"].shape[0]!= num_test:
            error = error + 1

        pickle_in.close()
        
        if error == 0:
            print("[INFO] - All checks done. Pickle file size matches the expected shape")
        else:
            print("[ERROR] - The pickle file dimensions do not meet the expectations (" , str(error), "). Something went wrong.")

def main(argv):
    inputfile = ''
    outputfile = ''
    workingpath = ""
    image_width = 128
    image_height = 128
    ratio = 0.1

    print("--------------------------------------------------------------------")
    print("RGB-DNM2Pickle")
    print("Packing files into a pickle")

    try:
        opts, args = getopt.getopt(argv,"h:i:o:d:r:c:x:",["ifile=","ofile=","wpath=","imgh=","imgw=", "ratio="])
    except getopt.GetoptError:
        print ('test.py -i <inputfile> -o <outputfile>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print ('test.py -i <inputfile> -o <outputfile> -d <working_directory> -r <image rows> -c <image cols> -x <train to test ratio>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg
        elif opt in ("-o", "--ofile"):
            outputfile = arg
        elif opt in ("-d", "--wpath"):
            workingpath = arg
        elif opt in ("-r", "--imgh"):
            image_height = int(arg)
        elif opt in ("-c", "--imgw"):
            image_width = int(arg)
        elif opt in ("-x", "--ratio"):
            ratio = float(arg)

    print ("[INFO] - Input file is: ", inputfile)
    print ("[INFO] - Output file is: ", outputfile)
    print("[INFO] - Working directory is: ", workingpath)
    print(f'[INFO] - Image width: {image_width}, Image height: {image_height}')
    print(f'[INFO] - Train to test ratio: {ratio}')

    i2p = RGB_DNM2Pickle(image_height, image_width, ratio)
    i2p.process(inputfile, outputfile, workingpath)



if __name__ == '__main__':
    main(sys.argv[1:])