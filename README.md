# Difidi - diode finite differences

<p align="center">
<img src="https://user-images.githubusercontent.com/12766039/202579712-53da04ec-326b-469b-9a24-effe43711151.png" width=40% height=40%>
<img src="https://user-images.githubusercontent.com/12766039/202579720-01250d9e-66e9-41a7-9a86-767bc99ac90e.png" width=40% height=40%>
</p>

Simulates an MOS (metal-oxide semiconductor) diode with a voltage applied through it. Solves the 1D Poisson equation

$$
\phi''(z) = -\frac{\rho(z)}{\epsilon(z)} = f(z)
$$

using the finite difference method and computes Fermi-Dirac integrals

$$
n(z) = \frac{\sqrt{2} m_e^{3/2}}{\pi^2 \hbar^3} \int_{E_c(z)}^\infty \frac{\sqrt{E - E_c(z)}}{1 + \exp(E / k_B T)} \ \text{d} E,
$$

$$
p(z) = \frac{\sqrt{2} m_e^{3/2}}{\pi^2 \hbar^3} \int_{-\infty}^{E_v(z)} \frac{\sqrt{E_v(z) - E}}{1 + \exp(E / k_B T)} \ \text{d} E,
$$

to determine the charge density distribution $\rho(z) = e (-N_A - n(z) + p(z))$. A solution satisfying all equations is found by an iterative algorithm using damping.

## Compilation and running

Compilation:
```
make
```

Compilation requires the LAPACK and GSL libraries.

Usage:

```
./difidi diode.in diode.out > out.log
```

The file diode.in contains the simulation parameters. The file diode.out contains the solution in the format:
```
<z position> <rho(z)> <phi(z)> <n(z)> <p(z)> <Ev(z)> <Ec(z)>
```

## Plotting

Plotting:
```
./plot.py [diode.out]
```

This produces plots for potential, charge distribution, charge carrier distributions and the band diagram.

Solver residual can be monitored in real time using:
```
gnuplot residual.gp
```

