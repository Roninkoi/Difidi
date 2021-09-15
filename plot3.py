import numpy as np
import matplotlib.pyplot as plt

# line + scatter plot of multiple data sets a
# with different labels and styles
def linescatter(a, titles=["", "", ""], labels=[], styles=[], fpath=""):
    f = plt.figure(figsize=(10, 10))
    plt.rcParams.update({'font.size': 22})

    i = 0
    for ai in a:
        alabel = str(i+1)
        amarker = ""
        alinestyle = "-"
        
        if (len(labels) > 0):
            alabel = labels[i]
            
        if (len(styles) > 0):
            if (styles[i] == 1):
                amarker = "o"
                alinestyle = ""
                
        plt.plot(ai[:, 0], ai[:, 1], label=alabel, marker=amarker, linestyle=alinestyle, alpha=0.7)
        i += 1
        
    plt.title(titles[0])
    plt.xlabel(titles[1])
    plt.ylabel(titles[2])
    
    plt.grid(True)
    plt.legend(fontsize='xx-small')

    if (len(fpath) > 0):
        f.savefig(fpath, bbox_inches='tight') # save to file

    plt.show()
    plt.close()

da = np.loadtxt("run/diode0.5mic.out")
db = np.loadtxt("run/diode1mic.out")
dc = np.loadtxt("run/diode2mic.out")

data1a = np.array([da[:, 0], da[:, 1]]).T
data2a = np.array([da[:, 0], da[:, 2]]).T
data3a = np.array([da[:, 0], da[:, 3]]).T
data4a = np.array([da[:, 0], da[:, 4]]).T

data1b = np.array([db[:, 0], db[:, 1]]).T
data2b = np.array([db[:, 0], db[:, 2]]).T
data3b = np.array([db[:, 0], db[:, 3]]).T
data4b = np.array([db[:, 0], db[:, 4]]).T

data1c = np.array([dc[:, 0], dc[:, 1]]).T
data2c = np.array([dc[:, 0], dc[:, 2]]).T
data3c = np.array([dc[:, 0], dc[:, 3]]).T
data4c = np.array([dc[:, 0], dc[:, 4]]).T

linescatter([data1a, data1b, data1c], ["Charge density", "$z$ (nm)", r"$\rho$ (nm$^{-3}$)"], ["$0.5 \mu m$", "$1 \mu m$", "$2 \mu m$"], fpath="rho.pdf")
linescatter([data2a, data2b, data2c], ["Potential", "$z$ (nm)", r"$\phi$ (V)"], ["$0.5 \mu m$", "$1 \mu m$", "$2 \mu m$"], fpath="phi.pdf")
linescatter([data3a, data4a, data3b, data4b, data3c, data4c], ["Charge carrier concentration", "$z$ (nm)", "$n$ (nm$^{-3}$)"], ["$n, 0.5 \mu m$", "$p, 0.5 \mu m$", "$n, 1 \mu m$", "$p, 1 \mu m$", "$n, 2 \mu m$", "$p, 2 \mu m$"], fpath="np.pdf")

