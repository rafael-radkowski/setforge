import os
import time

res = [128, 256, 512, 1024]
num = [1000, 5000, 10000, 20000]

count = 0
for r in res:
	for n in num:

		start_time = time.time()
		os.system('DatasetRenderer.exe ../data/stanford_bunny_02_rot.obj -o debug_out -img_w '+str(r)+ ' -img_h '+str(r)+ ' -m POSE -num ' + str(n) + ' -sub 4 -limx 0.6 -limy 0.6 -lim_near 1.0 -lim_far 2.5')
		duration = time.time() - start_time
		print(duration)

		f = open("perf_results.txt","a+")
		f.write(str(count) + ", " + str(r) + ", " + str(n) + ", " + str(duration) + "\n")
		f.close()
		count = count + 1
