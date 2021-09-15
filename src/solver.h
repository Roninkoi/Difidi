#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <cstring>

#include "util.h"

using namespace std;

class PoissonSolver {
public:
	void solve(); // solve Poisson equation

	void setBC(double phia, double phib) // set Neumann boundary conditions
	{
		this->phia = phia;
		this->phib = phib;
	}

	void setBounds(double ba, double bb) // set boundaries
	{
		this->ba = ba;
		this->bb = bb;
		h = (bb - ba) / (N - 1);
	}

	void setRHS(double *b) // set b of equation (this is a pointer to f)
	{
		memcpy(bx.m, b, sizeof(double) * M);
		/*bx.m = b;
		bx.c = 1;
		bx.r = M;*/
	}

	double eval(double z); // evaluate phi(z) using linear interpolation

	void create(int N);
	void destroy();
private:
	int N = 0; // number of points
	int M = 0; // system size

	// equation A x = b
	mat A; // stencil matrix
	mat bx; // RHS of equation / solution

	int *pivot; // pivot for LAPACK

	double ba = 0.; // boundaries
	double bb = 1.;
	double phia = 0.; // boundary conditions
	double phib = 1.;
	double h = 1.; // step size

	void construct(); // construct matrix
};

#endif
