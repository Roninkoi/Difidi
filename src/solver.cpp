#include <lapacke/lapack.h>

#include "solver.h"

void PoissonSolver::create(int N)
{
	this->N = N;
	this->M = N - 2;
	A.create(this->M, this->M);
	pivot = (int *) malloc(sizeof(int) * this->M);
}

void PoissonSolver::destroy()
{
	A.destroy();
	free(pivot);
}

void PoissonSolver::construct()
{
	A.clear(); // LAPACK puts LU factorization into A

	// three point stencil
	for (int ij = 0; ij < M; ++ij)
		A.set(ij, ij, -2. / h / h);

	for (int ij = 0; ij < M - 1; ++ij)
		A.set(ij, ij + 1, 1. / h / h);

	for (int ij = 0; ij < M - 1; ++ij)
		A.set(ij + 1, ij, 1. / h / h);
}

void PoissonSolver::solve()
{
	construct(); // construct matrix A

	bx.first() -= phia / h / h; // Neumann boundary conditions
	bx.last() -= phib / h / h;

	int info, n1 = M, n2 = 1;

	// solve A x = b using LAPACK, result goes into bx
	// matrix A is symmetric, so no need to transpose
	dgesv_(&n1, &n2, A.m, &n1, pivot, bx.m, &n1, &info);

	if (info != 0)
		cout << "LAPACK: " << info << endl;
}

double PoissonSolver::eval(double z)
{
	double zz = z - ba;
	int i = (int) floor(zz / h); // left element index
	int j = i + 1; // right element index

	if (i < 0 && j < 0) // boundaries
		return phia;
	if (i >= M && j >= M)
		return phib;
	if (i < 0 && j >= 0)
		return bx.get(j);
	if (j >= M && i < M)
		return bx.get(i);

	return bx.get(i) * fabs(j * h - zz) / h + bx.get(j) * fabs(i * h - zz) / h; // weighted average
}
