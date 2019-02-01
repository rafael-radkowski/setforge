"""
class Image2Pickle

This file prepares a pickle dataset using the results the program ImageGen produces.
This file processes RGB data only.

The pickle contains a dict with the following labels:
    Xtr - the training data set, rgb images of shape (num_images, width, height, channels)
    Ytr_pose - the training poses of each object stored as (num_images, x, y, z, qx, qy, qz, qw)
    Ytr_roi - the training region of interest for each object stored as (num_images, rx, ry, dx, dy)
    Xte - the test data set, rgb images of shape (num_images, width, height, channels)
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
Jan 2019
MIT license
-------------------------------------
Last edited:

"""

import cv2
import numpy as np
import pickle
import csv
import sys
import getopt
import random
import os

class Image2Pickle:

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

                    #index, rgb_file, normals_file, depth_file, mat_file, tx, ty, tz, qx, qy, qz, qw, roi_x,  roi_y, dx, dy
                    #print(row['index'], ":", row['rgb_file'], ", ", row['normals_file'], ", ", row['depth_file'])
                    line_count += 1

                    data = {
                        "index": row['index'],
                        "rgb_file": row['rgb_file'],
                        "normals_file": row['normals_file'],
                        "depth_file": row['depth_file'],
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
                        "dx": float(row['dx']),
                        "dy": float(row['dy'])
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
        Xtr, Ytr_pose, Ytr_roi = self.__prepare_data(self.train_data, working_directory )
        all_data = dict()
        all_data["Xtr"] = Xtr
        all_data["Ytr_pose"] = Ytr_pose
        all_data["Ytr_roi"] = Ytr_roi

        # store the data
        pickle_out = open(dst_file, "wb")
        pickle.dump(all_data, pickle_out)
        pickle_out.close()
        all_data.clear()

        # prepare the test dataset
        Xte, Yte_pose, Yte_roi = self.__prepare_data(self.test_data, working_directory)


        pickle_in = open(dst_file, "rb")
        mydata = pickle.load(pickle_in)
        mydata["Xte"] = Xte
        mydata["Yte_pose"] = Yte_pose
        mydata["Yte_roi"] = Yte_roi

        # store the data
        pickle_out = open(dst_file, "wb")
        pickle.dump(mydata, pickle_out)
        pickle_out.close()

        print(f'[DONE] - Stored all data in {dst_file}')

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
                 pose_results, the pose results of size  (num items, 7) ,
                 roi_results, the roi results of size  (num items, 4)
        """
        print("[INFO] - Preparing")

        count  = 0
        rgb_volume = []
        normal_volume = []
        depth_volum = []

        pose_results = []
        roi_results = []
        temp_count = 0


        for each in data_tr:
            path_rgb = working_directory + self.__checkFilename(each['rgb_file'])
           # path_norm = working_directory + self.__checkFilename(each['normals_file'])
           # path_depth = working_directory + self.__checkFilename(each['depth_file'])

            # read the images
            rgb = cv2.imread(path_rgb)
            #normals = cv2.imread(path_norm, cv2.IMREAD_COLOR | cv2.IMREAD_ANYDEPTH)  # uint16
            #depth = cv2.imread(path_depth, cv2.IMREAD_GRAYSCALE | cv2.IMREAD_ANYDEPTH)  # uint16

            # resize the images
            resized_rgb = cv2.resize(rgb, (self.dst_height, self.dst_width))
           # resized_normals = cv2.resize(normals, (self.dst_height, self.dst_width))
            #resized_depth = cv2.resize(depth, (self.dst_height, self.dst_width))

            # reshape them
            (h, w, c) = resized_rgb.shape
            resized_rgb = resized_rgb.reshape([1, h, w, c])
            #resized_normals = resized_normals.reshape([1, h, w, c])
            #resized_depth = resized_depth.reshape([1, h, w, 1])

            #rgb = cv2.rectangle(rgb, (int(each["rx"]), int(each["ry"])), (int(each["rx"] + each["dx"]), int(each["ry"] + each["dy"])), (0, 255, 0), 2)
            #cv2.imshow("out",rgb )
            #cv2.waitKey()

            ratio_x = float(h) / float(self.dst_height)
            ratio_y = float(w) / float(self.dst_width)

            # training result
            r0 = np.array([each["tx"], each["ty"], each["tz"], each["qx"], each["qy"], each["qz"], each["qw"]])
            r0 = r0.reshape([1, r0.shape[0]])
            r1 = np.array([each["rx"]/ratio_x, each["ry"]/ratio_y, each["dx"]/ratio_x, each["dy"]/ratio_y])
            r1 = r1.reshape([1, r1.shape[0]])

            if len(rgb_volume) == 0:
                rgb_volume = resized_rgb
               # normal_volume = resized_normals
               # depth_volume = resized_depth
                pose_results = r0
                roi_results = r1
            else:
                rgb_volume = np.concatenate((rgb_volume, resized_rgb), axis=0)
               # normal_volume = np.concatenate((self.normal_volume, resized_normals), axis=0)
               # depth_volume = np.concatenate((self.depth_volume, resized_depth), axis=0)
                pose_results =  np.concatenate((pose_results, r0), axis=0)
                roi_results = np.concatenate((roi_results, r1), axis=0)
            count = count + 1

            if count%100 == 0:
                print(".", end=" ", flush=True)
                if count % 1000 == 0:
                    print(f' {count}/{len(data_tr)}')
                    # temp file

                    temp = dict()
                    temp["X"] = rgb_volume
                    temp["Y_pose"] = pose_results
                    temp["y_roi"] = roi_results

                    path = "temp_" + str(temp_count)
                    pickle_out = open(path, "wb")
                    pickle.dump(temp, pickle_out)
                    pickle_out.close()
                    temp_count = temp_count + 1
                    temp.clear()
                    rgb_volume = []
                    pose_results = []
                    roi_results = []



            #cv2.imshow("rgb", rgb)
            #cv2.waitKey(0)

        # concatenate all temp pickle files
        rgb_volume = []
        pose_results = []
        roi_results = []
        for i in range(temp_count):
            path = "temp_" + str(i)
            pickle_in = open(path, "rb")
            data = pickle.load(pickle_in)
            if len(rgb_volume) == 0:
                rgb_volume = data["X"]
                pose_results = data["Y_pose"]
                roi_results = data["y_roi"]
            else:
                rgb_volume = np.concatenate((rgb_volume, data["X"]), axis=0)
                pose_results = np.concatenate((pose_results, data["Y_pose"]), axis=0)
                roi_results = np.concatenate((roi_results, data["y_roi"]), axis=0)
            pickle_in.close()
            data = []

        # removing temporary files
        for i in range(temp_count):
            path = "temp_" + str(i)
            os.remove(path)

        print("", end="\n")
        print("[INFO] - Done processing")
        print("[INFO] - RGB volume shape: ", rgb_volume.shape)
        print("[INFO] - Pose results shape: ", pose_results.shape)
        print("[INFO] - RoI results shape: ", roi_results.shape)

        return rgb_volume, pose_results, roi_results

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

def main(argv):
    inputfile = ''
    outputfile = ''
    workingpath = ""
    image_width = 128
    image_height = 128
    ratio = 0.1

    print("--------------------------------------------------------------------")
    print("Image2Pickle")
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

    i2p = Image2Pickle(image_height, image_width, ratio)
    i2p.process(inputfile, outputfile, workingpath)



if __name__ == '__main__':
    main(sys.argv[1:])