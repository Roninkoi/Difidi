import numpy as np
import matplotlib.pyplot as plt

# line + scatter plot of multiple data sets a
# with different labels and styles
def linescatter(a, titles=["", "", ""], labels=[], styles=[], fpath=""):
    f = plt.figure(figsize=(10, 10))
    plt.rcParams.update({'font.size': 22})

    i = 0
    for ai in a:
        alabel = str(i + 1)
        amarker = ""
        alinestyle = "-"
        
        if (len(labels) > 0):
            alabel = labels[i]
            
        if (len(styles) > 0):
            if (styles[i] == 1):
                amarker = "o"
                alinestyle = ""
                
        plt.plot(ai[:, 0], ai[:, 1], label=alabel, marker=amarker, linestyle=alinestyle)
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

data = np.loadtxt("diode.out")

data1 = np.array([data[:, 0], data[:, 1]]).T
data2 = np.array([data[:, 0], data[:, 2]]).T
data3 = np.array([data[:, 0], data[:, 3]]).T
data4 = np.array([data[:, 0], data[:, 4]]).T

linescatter([data1], ["Charge density", "$z$ (nm)", r"$\rho$ (nm$^{-3}$)"], fpath="rho.pdf")
linescatter([data2], ["Potential", "$z$ (nm)", r"$\phi$ (V)"], fpath="phi.pdf")
linescatter([data3, data4], ["Charge carrier concentration", "$z$ (nm)", "$n$ (nm$^{-3}$)"], ["$n$", "$p$"], fpath="np.pdf")

