import matplotlib.pyplot as plt
import numpy as np

logpath = '/Users/vadimbaitoiu/Downloads/complexitydata.csv'
complexityMap = np.loadtxt(logpath, comments="#", unpack=False)
complexityMap = np.transpose(complexityMap)
complexityMax, fileIndexMax = complexityMap.shape
x = np.arange(0, fileIndexMax, 1)
fig, axs = plt.subplots(4, 1, layout='constrained')
axs[0].plot(x, complexityMap[0, :], color='dodgerblue')
axs[0].set_xlabel('File')
axs[0].set_ylabel('Line count')
axs[0].grid(True)
axs[1].plot(x, complexityMap[1, :], color='lime')
axs[1].set_xlabel('File')
axs[1].set_ylabel('Comment energy')
axs[1].grid(True)
axs[2].plot(x, complexityMap[2, :], color='cyan')
axs[2].set_xlabel('File')
axs[2].set_ylabel('Relation energy')
axs[2].grid(True)
axs[3].plot(x, complexityMap[3, :], color='tomato')
axs[3].set_xlabel('File')
axs[3].set_ylabel('Search result energy')
axs[3].grid(True)

'''
cxy, f = axs[1].cohere(s1, s2, 256, 1. / dt)
axs[1].set_ylabel('Coherence')
'''
plt.show()
