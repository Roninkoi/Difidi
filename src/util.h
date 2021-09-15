#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <chrono>
#include <ctime>

using namespace std;

static const struct Constants {
	double kB = 8.617333262e-5; // Boltzmann constant (eV / K)
	double me0 = 5.6856301; // electron mass (eV fs^2 / nm^2)
	double e = 1.; // elementary charge [e] = 1
	double hbar = 0.6582119570; // reduced Planck constant (eV fs)
	double eps0 = 5.52634936e-2; // vacuum permittivity (e / nm / V)
} CONST;

// r x c matrix
class mat {
public:
	double *m = nullptr; // data
	int r; // rows
	int c; // columns

	void clear() // set all to 0
	{
		for (int i = 0; i < r * c; ++i)
			m[i] = 0.;
	}

	double &first() // get ref to first element
	{
		return m[0];
	}

	double &last() // get ref to last element
	{
		return m[r * c - 1];
	}

	double get(int i, int j = 0) // get element ij
	{
		if (i < 0 || i >= r || j < 0 || j >= c)
			return 0;

		return m[c * i + j];
	}

	void set(int i, int j, double v) // set element ij
	{
		if (i < 0 || i >= r || j < 0 || j >= c)
			return;

		m[c * i + j] = v;
	}

	double *create(int r, int c)
	{
		this->r = r;
		this->c = c;
		m = (double *) calloc(r * c, sizeof(double));

		return m;
	}

	void destroy()
	{
		free(m);
	}

	void print()
	{
		for (int i = 0; i < r; ++i) {
			for (int j = 0; j < c; ++j) {
				cout << m[i * c + j] << "\t";
			}
			cout << endl;
		}
	}
};

const string currentDateTime();

#endif
