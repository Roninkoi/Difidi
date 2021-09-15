# Difidi - diode finite differences

Solves the Poisson equation using the finite difference method and computes Fermi-Dirac integrals to determine the charge density distribution rho(z). A self-consistent solution is found by an iterative algorithm using damping.

Compilation:
```
make release
```

Usage:

```
./difidi diode.in diode.out > out.log
```

Plotting:
```
python3 plot.py
```

