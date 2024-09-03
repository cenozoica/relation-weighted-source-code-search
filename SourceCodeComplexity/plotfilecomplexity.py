import matplotlib.pyplot as plt
import numpy as np

plt.rcParams['font.family'] = 'Courier'
logpath = '/Users/vadimbaitoiu/Downloads/complexitydata.csv'
complexityMap = np.loadtxt(logpath, comments="#", unpack=False)
complexityMap = np.transpose(complexityMap)
complexityMax, fileIndexMax = complexityMap.shape
x = np.arange(0, fileIndexMax, 1)

complexityName = ['Line count', 'Comment energy', 'Relation energy', 'Search result energy']
complexityColor = ['black', 'black', 'black', 'red']
fig, axs = plt.subplots(len(complexityName), 1, sharex=True, layout='constrained')

for i in range(len(complexityName)):
    axs[i].plot(x, complexityMap[i, :], color=complexityColor[i])
    axs[i].set_title(complexityName[i], fontsize=20, color=complexityColor[i], y=0.55, va='top')
    axs[i].grid(False)
    axs[i].axis(False)

fig.suptitle(str(fileIndexMax) + ' files', fontsize=24, color='black')
plt.show()
