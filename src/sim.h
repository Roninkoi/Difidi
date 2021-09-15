#ifndef SIM_H
#define SIM_H

#include <fstream>

#include "solver.h"

/*
 * Diode finite difference code, Roni Koitermaa 2021
 *
 * Solves the Poisson equation using the finite difference method and computes
 * Fermi-Dirac integrals to determine the charge density distribution rho(z).
 * A self-consistent solution is found by an iterative algorithm using damping.
 */

class Sim {
public:
	Sim(string ipath, string opath); // load settings from file
	int run(); // main loop of simulation
	
private:
	ifstream infile; // in / out files
	ofstream outfile;
	string outpath;

	int ticks = 0; // simulator cycle
	bool rtWrite = false; // write to file in real time?

	int N = 0; // number of elements
	double w = 0.5; // damping factor
	double C = 0.; // convergence criterion
	double rho0 = 0.; // initial guess
	double T = 300.; // temperature

	double tB = 1.; // thicknesses
	double tOx = 1.;
	double VG = 1.; // gate voltage
	double Vstep = 0.; // voltage step for VI calculation, enabled if > 0

	double NA = 1.; // doping concentration

	double EgSi = 0.; // band gap of Si
	double EgOx = 0.; // band gap of oxide

	double epsSi = 1.; // dielectric constants
	double epsOx = 1.;

	double meSi = 1.; // effective masses
	double mhSi = 1.;

	vector<double> rho; // current iteration density
	vector<double> rhoi; // previous iteration density
	vector<double> f; // -rho(z) / eps(z)

	PoissonSolver psolver; // solver for Poisson equation

	void write(); // write to file
	
	double getZ(int i); // get position from index
	double getMe(double z); // get effective mass
	double getMh(double z);
	double getEps(double z); // dielectric function
	double getEg(double z); // band gap

	double n(double z); // charge carrier concentrations from Fermi-Dirac integral
	double p(double z);

	double Ev(double z); // valence band
	double Ec(double z); // conduction band
};

#endif
