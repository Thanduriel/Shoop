import numpy as np
import matplotlib.pyplot as plt
import matplotlib

base_dir = "../build/"
base_name = 'fixedDataNet3'
macro_epochs = range(0,6)

#################################################
# IMPLEMENTATION

matplotlib.style.use('tableau-colorblind10')

for i in macro_epochs:
	train_loss = np.genfromtxt(fname="{}{}_{}_trainLoss.txt".format(base_dir, base_name, i),
						   dtype=np.float32,
						   delimiter=' ',
						   skip_header=0)
	valid_loss = np.genfromtxt(fname="{}{}_{}_validLoss.txt".format(base_dir, base_name, i),
						   dtype=np.float32,
						   delimiter=' ',
						   skip_header=0)
					  

	plt.plot(train_loss[:,0], train_loss[:,1], label="train_{}".format(i))
	plt.plot(valid_loss[:,0], valid_loss[:,1], label="valid_{}".format(i), linestyle='dashed')

plt.legend()
plt.xlabel("epoch")
plt.ylabel("loss")
plt.savefig("{}.pdf".format(base_name))
plt.show()