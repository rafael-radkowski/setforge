"""
class DatasetReader


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


class DatasetReader:

    # the dataset as read from file
    dataset = []

    # the image data
    image_data = []

    # rgb volume
    rgb_count = 0
    rgb_volume = []
    normal_volume = []
    depth_volume = []

    # input dict
    input_dict = {}
    N = 0

    def __init__(self):
        pass

    def preprocess(self, path_and_filename, path_offset, dst_file, dst_height, dst_width):
        """

        :param path_and_filename:
        :return:
        """

        with open(path_and_filename, mode='r') as csv_file:
            csv_reader = csv.DictReader(csv_file)
            line_count = 0
            for row in csv_reader:
                if line_count == 0:
                    print(f'Column names are {", ".join(row)}')
                    line_count += 1

                print(row['index'], ":", row['rgb_file'], ", ", row['normals_file'], ", ", row['depth_file'])
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
                }

                self.dataset.append(data)

            print(f'Read {line_count} lines.')


        # Read all images
        for each in self.dataset:

            print("Process set", each['index'], " with", each['rgb_file'])

            path_rgb = path_offset + each['rgb_file']
            path_norm = path_offset + each['normals_file']
            path_depth = path_offset + each['depth_file']

            # read the images
            rgb = cv2.imread(path_rgb)
            normals = cv2.imread(path_norm, cv2.IMREAD_COLOR | cv2.IMREAD_ANYDEPTH ) # uint16
            depth = cv2.imread(path_depth, cv2.IMREAD_GRAYSCALE | cv2.IMREAD_ANYDEPTH) # uint16

            resized_rgb = cv2.resize(rgb, (dst_height, dst_width))
            resized_normals = cv2.resize(normals, (dst_height, dst_width))
            resized_depth = cv2.resize(depth, (dst_height, dst_width))

            #print(normals.dtype)
            #print(depth.dtype)

            #cvuint8 = cv2.convertScaleAbs(normals)
            #cv2.imshow("16bit", cvuint8)
            #cv2.waitKey(1)

            (h, w, c) = resized_rgb.shape
            resized_rgb = resized_rgb.reshape([1, h, w, c])
            resized_normals = resized_normals.reshape([1, h, w, c])
            resized_depth = resized_depth.reshape([1, h, w, 1])

            if self.rgb_count == 0:
                self.rgb_volume = resized_rgb
                self.normal_volume = resized_normals
                self.depth_volume = resized_depth
            else:
                self.rgb_volume = np.concatenate((self.rgb_volume, resized_rgb), axis=0)
                self.normal_volume = np.concatenate((self.normal_volume, resized_normals), axis=0)
                self.depth_volume = np.concatenate((self.depth_volume, resized_depth), axis=0)
            self.rgb_count = self.rgb_count + 1

        print("RGB volume shape: ", self.rgb_volume.shape)
        print("Normal volume shape: ", self.normal_volume.shape)
        print("Depth volume shape: ", self.depth_volume.shape)

        # Write pickle data to file
        self.input_dict = dict()
        self.input_dict["rgb"] = self.rgb_volume
        self.input_dict["depth"] = self.depth_volume
        self.input_dict["normals"] = self.normal_volume
        self.input_dict["rows"] = self.rgb_volume.shape[1]
        self.input_dict["cols"] = self.rgb_volume.shape[2]
        self.input_dict["num"] = self.rgb_volume.shape[0]
        self.input_dict["rgb_type"] = self.rgb_volume.dtype
        self.input_dict["depth_type"] = self.depth_volume.dtype
        self.input_dict["normal_type"] = self.normal_volume.dtype

        self.N = self.input_dict["num"]

        pickle_out = open(dst_file, "wb")
        pickle.dump(self.input_dict, pickle_out)
        pickle_out.close()

        print("Wrote data set into file", dst_file)

    def read_pickle(self, path_and_filename):
        pickle_in = open(path_and_filename, "rb")
        self.input_dict = pickle.load(pickle_in)

        pickle_in.close()

        print("\nRead data from ",path_and_filename)
        print("RGB dataset shape", self.input_dict["rgb"].shape)
        print("Depth dataset shape", self.input_dict["normals"].shape)
        print("Normals dataset shape", self.input_dict["depth"].shape)
        print("RGB dataset type", self.input_dict["rgb"].dtype)
        print("Normals dataset type", self.input_dict["normals"].dtype)
        print("Depth dataset type", self.input_dict["depth"].dtype)
        print("Num images", self.input_dict["num"])

        self.N = self.input_dict["num"]


    def getTrainingBatch(self, batch_size):
        training_batch = zip(range(0, self.N, batch_size), range(batch_size, self.N + 1, batch_size))
        return training_batch

def test():

    filename = "../bin/output/render_log.csv"

    reader = DatasetReader()
    reader.preprocess(filename, "../bin/", "bunny_data.pickle",  128, 128)
    reader.read_pickle("bunny_data.pickle")
    tX = reader.getTrainingBatch(20)

    tx_set = set(tX)
    print(tx_set)



if __name__ == '__main__':
    test()
