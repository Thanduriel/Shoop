import numpy as np
import matplotlib.pyplot as plt

base_dir = "../build/"
files = [('newSampleNet_0_testScore.txt', '0'),
		('newSampleNet_1_testScore.txt', '1'),
		('newSampleNet_2_testScore.txt', '2'),
		('newSampleNet_3_testScore.txt', '3'),
		('newSampleNet_4_testScore.txt', '4'),
		('newSampleNet_5_testScore.txt', '5'),
		('newSampleNet_6_testScore.txt', '6'),
		('newSampleNet_7_testScore.txt', '7')]
compute_avg = True



#################################################
# IMPLEMENTATION
scores = []
max_period = 0
for (result_file,name) in files:
	score = np.genfromtxt(fname=base_dir + result_file,
                       dtype=np.float32,
                       delimiter=' ',
                       skip_header=0)
	max_period = max(max_period, score[-1,0])
	scores.append(score)
	print("{} - min {}, max {}".format(name, np.min(score[:,1]), np.max(score[:,1])))

if compute_avg:
	score_avg = np.zeros(int(max_period))
	x = np.arange(0,int(max_period))
	for score in scores:
		score_avg += np.interp(x, score[:,0], score[:,1])
	score_avg /= len(scores)
	plt.plot(score_avg)
else:				   
	for i in range(0, len(scores)):
		plt.plot(scores[i][:,0], scores[i][:,1], label=files[i][1])
	plt.legend()


plt.xlabel("epoch")
plt.ylabel("score")
plt.show()