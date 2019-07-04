"""
This script packs recorded images into a pickle file. 
It is prepared to work with ObjectCapture (in-house tool), and combines all the data for 
the pose_cnn network.

Rafael Radkowski
Iowa State University
rafael@iastate.edu
April 2019
MIT License

"""

import cv2
import numpy as np
import pickle
import csv
import sys
import getopt
import random
import os
import math
from NormalMapEstimate import *



class Cam2Pickle:

    _img_width = 640
    _img_height = 490
    _cross_valid_ratio = 0.1

    # Percentage of the dataset used for validation
    ratio_validation = 0.1

    test_data = []
    train_data = []

    bootstrap_type = 0
    counter = 0

    def __init__(self, image_height, image_width, ratio):
        self._img_width = image_width
        self._img_height = image_height
        self._cross_valid_ratio = ratio


    ###------------------------------------------------------------------------
    ##
    ## File handling
    def __quaternion_from_matrix(self, matrix, isprecise=False):
        """Return quaternion from rotation matrix.

        If isprecise is True, the input matrix is assumed to be a precise rotation
        matrix and a faster algorithm is used.

        From: https://www.lfd.uci.edu/~gohlke/code/transformations.py.html
        """

        M = np.array(matrix, dtype=np.float64, copy=False)[:4, :4]
        if isprecise:
            q = np.empty((4,))
            t = np.trace(M)
            if t > M[3, 3]:
                q[0] = t
                q[3] = M[1, 0] - M[0, 1]
                q[2] = M[0, 2] - M[2, 0]
                q[1] = M[2, 1] - M[1, 2]
            else:
                i, j, k = 0, 1, 2
                if M[1, 1] > M[0, 0]:
                    i, j, k = 1, 2, 0
                if M[2, 2] > M[i, i]:
                    i, j, k = 2, 0, 1
                t = M[i, i] - (M[j, j] + M[k, k]) + M[3, 3]
                q[i] = t
                q[j] = M[i, j] + M[j, i]
                q[k] = M[k, i] + M[i, k]
                q[3] = M[k, j] - M[j, k]
                q = q[[3, 0, 1, 2]]
            q *= 0.5 / math.sqrt(t * M[3, 3])
        else:
            m00 = M[0, 0]
            m01 = M[0, 1]
            m02 = M[0, 2]
            m10 = M[1, 0]
            m11 = M[1, 1]
            m12 = M[1, 2]
            m20 = M[2, 0]
            m21 = M[2, 1]
            m22 = M[2, 2]
            # symmetric matrix K
            K = np.array([[m00 - m11 - m22, 0.0, 0.0, 0.0],
                             [m01 + m10, m11 - m00 - m22, 0.0, 0.0],
                             [m02 + m20, m12 + m21, m22 - m00 - m11, 0.0],
                             [m21 - m12, m02 - m20, m10 - m01, m00 + m11 + m22]])
            K /= 3.0
            # quaternion is eigenvector of K that corresponds to largest eigenvalue
            w, V = np.linalg.eigh(K)
            q = V[[3, 0, 1, 2], np.argmax(w)]
        if q[0] < 0.0:
            np.negative(q, q)
        return q
    def __getAllFilesOtType(sefl, path, type):
        files = []
        for file in os.listdir(path):
            if file.endswith(type):
               # print(os.path.join(path, file))
                files.append(os.path.join(path, file))

        return files
    def __sort_images_by_label(self, image_list, pose_list):

        output = {}
        label_rgb = '_rgb_'
        label_mask = 'rendering_rgb'
        label_depth = 'depth'
        label_pose = 'pose'
        label_rendered_depth = 'rendering_d'


        for str in image_list:
            idx = str.find('\\')
            name = str[idx+1:len(str)]
            idx = name.find('_')
            number_str = name[0:idx]
            num = int(number_str)

            x = output.get(num)

            if x == None:
                output[num] = { 'rgb':'', 'mask':'', 'depth':'', 'pose':'', 'rdepth':'' }

            if str.find(label_rgb) != -1: # found it
                output[num]['rgb'] = str
            elif str.find(label_mask) != -1: # found it
                output[num]['mask'] = str
            elif str.find(label_pose) != -1: # found it
                output[num]['pose'] = str
            elif str.find(label_rendered_depth) != -1:  # found it
                output[num]['rdepth'] = str
            elif str.find(label_depth) != -1:  # found it
                output[num]['depth'] = str

        for str in pose_list:
            idx = str.find('\\')
            name = str[idx + 1:len(str)]
            idx = name.find('_')
            number_str = name[0:idx]
            num = int(number_str)

            x = output.get(num)

            if x == None:
                output[num] = {'rgb': '', 'mask': '', 'depth': '', 'pose': '', 'rdepth': ''}
            if str.find(label_pose) != -1:  # found it
                output[num]['pose'] = str

        return output
    def __findLabeledImages(sefl, file_list, label):

        labeled_files = []

        for f in file_list:
            if f.find(label) != -1: # found the label
                #print(f)
                labeled_files.append(f)

        return labeled_files
    def __showImages(self, rgb, normal, depth, roi, mask):

        sx = 100
        sy = 100
        n, rows, cols, chan = rgb.shape
        delta_cols = 40
        delta_rows = 70

        cv2.imshow("img", rgb[0])
        cv2.moveWindow("img", sx, sy)
        cv2.imshow("normal_map", normal[0])
        cv2.moveWindow("normal_map",  sx, sy + cols + delta_cols)

        r1 = roi[0]
        roi_img = cv2.cvtColor( mask[0], cv2.COLOR_GRAY2BGR)
        cv2.rectangle(roi_img, (r1[0], r1[1]), (r1[0] + r1[2], r1[1] + r1[3]), (0, 255, 0), 1)

        cv2.imshow("roi", roi_img)
        cv2.moveWindow("roi", sx + rows + delta_rows, sy)

        cv2.imshow("mask", mask[0])
        cv2.moveWindow("mask", sx + rows + delta_rows, sy + cols + delta_cols)
        cv2.imshow("depth", depth[0])
        cv2.moveWindow("depth",sx + 2* rows + 2* delta_rows , sy + cols +  delta_cols)
        cv2.waitKey(1)

        return

        N = str(self.counter)
        folder = "./results_bagging/"
        file_rgb = folder + N + "_rgb.png"
        file_norm = folder +N + "_norm.png"
        file_depth = folder +N + "_depth.png"
        file_mask = folder +N + "_mask.png"

        #nm = normal[0].copy()
        #nm = nm *  65355
        #normal_short = np.ushort(nm)
        #normal_short = normal_short[:,:,:] * 65355

        cv2.imwrite(file_rgb, rgb[0])
        cv2.imwrite(file_norm,  normal[0])
        cv2.imwrite(file_depth, depth[0])
        cv2.imwrite(file_mask, mask[0])

        self.counter = self.counter + 1



    ###-------------------------------------------------------------------------
    ##
    ## Data processing

    def rotate(self, img, angle):
        s = img.shape
        rows = s[0]
        cols = s[1]
        M = cv2.getRotationMatrix2D((cols / 2, rows / 2), 90, 1)
        dst = cv2.warpAffine(img, M, (cols, rows))
        return dst

    def warp(self, img, shear_up, shear_down, transform, angle):
        s = img.shape
        rows = s[0]
        cols = s[1]

        M = np.float32([[1, 0, transform], [0, 1, 0]])
        dst = cv2.warpAffine(img, M, (cols, rows))

        dst = self.rotate(dst, 90)

        pts1 = np.float32([[0, 0], [rows + shear_up, 0], [rows , cols]])
        pts2 = np.float32([[0, 0], [rows, 0], [rows + shear_down, cols]])

        M = cv2.getAffineTransform(pts1, pts2)

        dst = cv2.warpAffine(dst, M, (cols, rows))
        #cv2.imshow("warp", dst)
       # cv2.waitKey()
        return dst

    def __bootstrap(self, img, type):

        if type == 0:
            return img
        elif type == 1:
            return self.rotate( img, 90)
        elif type == 2:
            return self.rotate(img, -90)
        elif type == 3:
            return self.warp( img, 100, 0, 50, 0)
        elif type == 4:
            return self.warp( img, 50, -100, -50, 0)
        elif type == 5:
            return self.warp( img, 50, -100, -50, 0)


    def __detectRoI(self, img):
        """
        Detect a region of interest in a one-channel mask iamge
        :param img: the image
        :return: roi x, y, w, h
        """
        x, y  = img.shape
        rows, cols = img.shape
        width = 1
        height = 1

        last_pixel = 0
        xr = 0
        yd = 0

        for i in range(rows):
            for j in range(cols):
                col = img[i][j]
                all_zero = False
                if col > 0.0 :
                    all_zero = True
                    if j < x:
                        x = j
                    if i < y:
                        y = i
                 # search for the last not zero pixel
                if last_pixel > 0:
                    if all_zero == True:
                        if j > xr:
                            xr = j
                        if i > height + y:
                            height = i - y

                last_pixel = col
        width = xr - x

        x = max( int(0), min(int(x), int(cols)) )
        y = max(int(0), min(int(y), int(rows)))

        width = max(1, min(width, cols))
        height = max(1, min(height, rows))

        roi_x = x
        roi_y = y
        roi_width = width
        roi_height = height

        return roi_x, roi_y, roi_width, roi_height

    def __processRGBImage(self, img_path, final_w, final_h):

        image = cv2.imread(img_path)
        image = self.__bootstrap(image, self.bootstrap_type)

        rows = image.shape[0]
        cols = image.shape[1]
        chan = image.shape[2]

        aspect = float(cols) / float(rows)
        aspect_need = float(final_w) / float(final_h)

        new_w = cols
        new_h = rows
        cols_offset = 0
        rows_offset = 0

        if aspect_need != aspect:
            if aspect > aspect_need:
                new_w = rows
                cols_offset = int((cols - new_w) / 2)
            elif aspect < aspect_need:
                new_h = cols
                rows_offset = int((rows - new_h) / 2)

        image_c = image[rows_offset:rows_offset + new_h, cols_offset:cols_offset + new_w, :]

        image_resized = cv2.resize(image_c, (final_h, final_w))

       # cv2.imshow("img", image_resized)
        #cv2.moveWindow("img", 100, 100)
       # cv2.waitKey(1)

        return image_resized

    def __processNormalMap(self, img_path, final_w, final_h):
        image = cv2.imread(img_path)
        image = self.__bootstrap(image, self.bootstrap_type)

        rows = image.shape[0]
        cols = image.shape[1]
        chan = image.shape[2]

        aspect = float(cols) / float(rows)
        aspect_need = float(final_w) / float(final_h)

        new_w = cols
        new_h = rows
        cols_offset = 0
        rows_offset = 0

        if aspect_need != aspect:
            if aspect > aspect_need:
                new_w = rows
                cols_offset = int((cols - new_w) / 2)
            elif aspect < aspect_need:
                new_h = cols
                rows_offset = int((rows - new_h) / 2)


        image_c = image[rows_offset:rows_offset + new_h, cols_offset:cols_offset + new_w, :]
        image_resized = cv2.resize(image_c, (final_h, final_w))

        normal_map = estimateNormalMap(image_resized, 3, 25)
        normal_map = normal_map * 65355

        #cv2.imshow("normal_map", normal_map)
        #cv2.moveWindow("normal_map", 380, 100)
        #cv2.waitKey(1)

        return np.ushort(normal_map)

    def __processMaskImage(self, img_path, final_w, final_h):

        image = cv2.imread(img_path)
        image = self.__bootstrap(image, self.bootstrap_type)

        rows = image.shape[0]
        cols = image.shape[1]
        chan = image.shape[2]

        aspect = float(cols) / float(rows)
        aspect_need = float(final_w) / float(final_h)

        new_w = cols
        new_h = rows
        cols_offset = 0
        rows_offset = 0

        if aspect_need != aspect:
            if aspect > aspect_need:
                new_w = rows
                cols_offset = int((cols - new_w) / 2)
            elif aspect < aspect_need:
                new_h = cols
                rows_offset = int((rows - new_h) / 2)

        image_c = image[rows_offset:rows_offset + new_h, cols_offset:cols_offset + new_w, :]
        image_resized = cv2.resize(image_c, (final_h, final_w))
        grayscaleMat = cv2.cvtColor(image_resized, cv2.COLOR_BGR2GRAY)



        # detect the region of interest
        rx, ry, rw, rh = self.__detectRoI(grayscaleMat)
        cv2.rectangle(image_resized, (rx, ry),  (rx + rw, ry + rh), (0, 255, 0), 1)

        #cv2.imshow("roi", image_resized)
        #cv2.moveWindow("roi", 100,260)
        #cv2.waitKey(1)

        #cv2.imshow("mask", grayscaleMat)
        #cv2.moveWindow("mask", 240, 100)
        #cv2.waitKey(1)

        return grayscaleMat,  rx, ry, rw, rh

    def __processDepthImage(self, dimg_path, final_w, final_h):

        image = cv2.imread(dimg_path, cv2.IMREAD_ANYDEPTH | cv2.IMREAD_UNCHANGED)
        image = self.__bootstrap(image, self.bootstrap_type)

        rows = image.shape[0]
        cols = image.shape[1]


        aspect = float(cols) / float(rows)
        aspect_need = float(final_w) / float(final_h)

        new_w = cols
        new_h = rows
        cols_offset = 0
        rows_offset = 0

        if aspect_need != aspect:
            if aspect > aspect_need:
                new_w = rows
                cols_offset = int((cols - new_w) / 2)
            elif aspect < aspect_need:
                new_h = cols
                rows_offset = int((rows - new_h) / 2)

        image_c = image[rows_offset:rows_offset + new_h, cols_offset:cols_offset + new_w]

        image_resized = cv2.resize(image_c, (final_h, final_w))

        #cv2.imshow("depth", image_resized)
        #cv2.moveWindow("depth", 240, 260)
        #cv2.waitKey(1)

        return image_resized

    def __processPose(self, pose_file):

        q = []
        t = np.zeros([3,1])
        matrix = np.zeros([4,4])

        f = open(pose_file, "r")
        fl = f.readlines()
        i = 0
        for x in fl:
            if i>0:
                w = x.split('\t')
                matrix[i-1, 0] = w[0]
                matrix[i-1, 1] = w[1]
                matrix[i-1, 2] = w[2]
                matrix[i-1, 3] = w[3]

            i = i + 1

        t[0,0] = matrix[0, 3]
        t[1, 0] = matrix[1, 3]
        t[2, 0] = matrix[2, 3]


        q = self.__quaternion_from_matrix(matrix, True)

        return t, q

    def __processAllData(self, data_list):

        temp_data = {}
        temp_data['rgb'] = []
        temp_data['norm'] = []
        temp_data['mask'] = []
        temp_data['pose'] = []
        temp_data['depth'] = []
        temp_count = 0

        N = 0
        tempN = 0
        for key, value in data_list.items():

            for i in range(1): # bagging
                self.bootstrap_type = i

                mask_file = value['mask']
                rgb_file = value['rgb']
                pose_file = value['pose']
                depth_file = value['depth']

                img = self.__processRGBImage(rgb_file, self._img_width, self._img_height)
                mask, rx, ry, rw, rh  = self.__processMaskImage(mask_file, self._img_width, self._img_height )
                depth = self.__processDepthImage(depth_file, self._img_width, self._img_height )
                normal_map = self.__processNormalMap(rgb_file, self._img_width, self._img_height)
                t,q = self.__processPose(pose_file)

                # concantenate all images
                img = img.reshape([1,self._img_width, self._img_height, 3 ])
                normal_map = normal_map.reshape([1, self._img_width, self._img_height, 3])
                mask = mask.reshape([1,self._img_width, self._img_height, 1 ])
                depth = depth.reshape([1,self._img_width, self._img_height, 1 ])
                pose = np.array([t[0,0], t[1,0],  t[2,0], q[0], q[1], q[2], q[3]])
                pose = pose.reshape([1, pose.shape[0]])
                roi = np.array([rx, ry,  rw, rh])
                roi = roi.reshape([1, roi.shape[0]])

                self.__showImages(img, normal_map, depth, roi, mask)


                if tempN == 0:
                    temp_data['rgb'] = img
                    temp_data['norm'] = normal_map
                    temp_data['mask'] = mask
                    temp_data['depth'] = depth
                    temp_data['pose'] = pose
                    temp_data['roi'] = roi
                else:
                    temp_data['rgb'] = np.concatenate(( temp_data['rgb'], img), axis=0)
                    temp_data['norm'] = np.concatenate(( temp_data['norm'], normal_map), axis=0)
                    temp_data['mask'] = np.concatenate(( temp_data['mask'], mask), axis=0)
                    temp_data['depth'] = np.concatenate(( temp_data['depth'], depth), axis=0)
                    temp_data['pose'] = np.concatenate(( temp_data['pose'], pose), axis=0)
                    temp_data['roi'] = np.concatenate((temp_data['roi'], roi), axis=0)

                N = N + 1
                tempN = tempN + 1

                if tempN%100 == 0:
                    print(".", end=" ", flush=True)

                # write the data into temporary pickle file.
                # Python dramatically slows down if the array becomes too large.
                if tempN % 1000 == 0:
                    print(f' {N}/{len(data_list) * 5}')
                    # write a temporary file.
                    temp_count = self.__writeTempFile(temp_count, temp_data)

                    # empty the data
                    temp_data = {}
                    tempN = 0



        if len(temp_data) > 0:
            print(f' {N}/{len(data_list) * 5}')
            # write a temporary file.
            temp_count = self.__writeTempFile(temp_count, temp_data)

            # empty the data
            temp_data = []



        # concatenate all temp pickle files
        if temp_count > 0:
            rgb_volume = []
            normal_volume = []
            mask_volume = []
            pose_results = []
            roi_results = []
            depth_volume = []
            for i in range(temp_count):
                path = "temp_" + str(i)
                pickle_in = open(path, "rb")
                data = pickle.load(pickle_in)
                if len(rgb_volume) == 0:
                    rgb_volume = np.array(data["X"])
                    normal_volume = np.array(data["X_norm"])
                    pose_results = np.array(data["Y_pose"])
                    roi_results = np.array(data["Y_roi"])
                    mask_volume = np.array(data["X_mask"])
                    depth_volume = np.array(data["X_depth"])
                else:
                    rgb_volume = np.concatenate((rgb_volume, data["X"]), axis=0)
                    normal_volume = np.concatenate((normal_volume, data["X_norm"]), axis=0)
                    pose_results = np.concatenate((pose_results, data["Y_pose"]), axis=0)
                    roi_results = np.concatenate((roi_results, data["Y_roi"]), axis=0)
                    mask_volume = np.concatenate((mask_volume, data["X_mask"]), axis=0)
                    depth_volume = np.concatenate((depth_volume, data["X_depth"]), axis=0)
                pickle_in.close()
                data = []
        else:
            rgb_volume = np.array(temp_data['rgb'])
            normal_volume = np.array(temp_data['rgb'])
            pose_results = np.array(temp_data['pose'])
            roi_results = np.array(temp_data['roi'])
            mask_volume = np.array(temp_data['mask'])
            depth_volume = np.array(temp_data['depth'])

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


        return {'rgb':rgb_volume, 'norm':normal_volume, 'mask':mask_volume,  'depth':depth_volume, 'pose':pose_results, 'roi':roi_results}


    def process(self, path_to_img, output_file, img_type):

        file_list = self.__getAllFilesOtType(path_to_img, img_type) # get all image file names
        pose_list = self.__getAllFilesOtType(path_to_img, 'txt') # get all txt file names containing pose information
        sorted_list = self.__sort_images_by_label(file_list, pose_list) # sort all file names and return them according to their number id

        # Split the lines by random into two sets
        num_test = int(len(sorted_list) * self.ratio_validation)
        num_train = int(len(sorted_list) - num_test)
        print(f'[INFO] - Selecting {num_test} datasets for validation and {num_train} for training.')  # -1, to not count the header line

        random.seed()
        te_index = random.sample(range(1, len(sorted_list)), num_test)

        self.test_data = {}
        for i in te_index:  # do not remove items from csv_dataset in the same process. The index must be maintained
            self.test_data[i] = sorted_list[i]

        for i in te_index:  # removing items.
            del sorted_list[i]

        self.train_data = sorted_list

        print(f'[INFO] - Found {len(self.train_data)} training data sets.')
        print(f'[INFO] - Found {len(self.test_data)} test data sets.')

        # Organize the training data
        print(f'[INFO] - Preparing training data set.')
        final_train = self.__processAllData(self.train_data)

        all_data = dict()
        all_data["Xtr"] = final_train['rgb']
        all_data["Xtr_norm"] = final_train['norm']
        all_data["Ytr_pose"] = final_train['pose']
        all_data["Ytr_roi"] = final_train['roi']
        all_data["Xtr_mask"] = final_train['mask']
        all_data["Xtr_depth"] = final_train['depth']

        print("[INFO] - Storing data")

        # store the data
        pickle_out = open(output_file, "wb")
        pickle.dump(all_data, pickle_out)
        pickle_out.close()
        all_data.clear()

        print(f'[INFO] - Stored all data in {output_file}')

        # Organize the test data.
        print(f'[INFO] - Preparing test data set.')
        final_test = self.__processAllData(self.test_data)

        pickle_in = open(output_file, "rb")
        mydata = pickle.load(pickle_in)
        mydata["Xte"] = final_test['rgb']
        mydata["Xte_norm"] = final_test['norm']
        mydata["Yte_pose"] = final_test['pose']
        mydata["Yte_roi"] = final_test['roi']
        mydata["Xte_mask"] = final_test['mask']
        mydata["Xte_depth"] = final_test['depth']
        pickle_in.close()

        print("[INFO] - Storing data")

        # store the data
        pickle_out = open(output_file, "wb")
        pickle.dump(mydata, pickle_out)
        pickle_out.close()

        print(f'[INFO] - Stored all data in {output_file}')
        print('[DONE]')




    def __writeTempFile(self, index, data):
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
        temp["X"] = data['rgb']
        temp["X_norm"] = data['rgb']
        temp["X_depth"] = data['depth']
        temp["Y_pose"] = data['pose']
        temp["X_mask"] = data['mask']
        temp["Y_roi"] = data['roi']

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


def main(argv):
    inputpath = '../data/bunny_lab_02'
    outputfile = 'bunny_lab02-64x64_bs_test.pickle'
    image_width = 64
    image_height = 64
    ratio = 0.1

    print("--------------------------------------------------------------------")
    print("Cam2Pickle")
    print("Packing camera recordings and rendered data into a pickle")


    c2p = Cam2Pickle(image_height, image_width, ratio)
    c2p.process(inputpath, outputfile, "png")



if __name__ == '__main__':
    main(sys.argv[1:])