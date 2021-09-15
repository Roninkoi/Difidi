#include <gsl/gsl_sf_fermi_dirac.h>
#include <signal.h>

#include "sim.h"

bool running = true; // simulator running?

void ihandler(int sn)
{
	if (sn == SIGINT) {
		running = false;
	}
}

// load integer from string s with variable name of v
#define loadi(v) \
	if (s.compare(#v) == 0) { \
		infile >> s; \
		v = stoi(s); \
		continue; \
	}

// load double from string s with variable name of v
#define loadd(v) \
	if (s.compare(#v) == 0) { \
		infile >> s; \
		v = stod(s); \
		continue; \
	}

Sim::Sim(string ipath, string opath) :
	infile(ipath), outpath(opath)
{
	if (!infile.is_open()) { 
		cout << "Can't open infile!" << endl;
		exit(EXIT_FAILURE);
	}

	string s;
	
	while (infile >> s) { // parse settings
		if (s.front() == '#') { // comment
			getline(infile, s);
			continue;
		}

		loadi(N);
		loadi(rtWrite);
		loadd(C);
		loadd(w);
		loadd(rho0);
		loadd(T);

		loadd(tB);
		loadd(tOx);
		loadd(VG);
		loadd(Vstep);

		loadd(NA);

		loadd(EgSi);
		loadd(EgOx);

		loadd(epsSi);
		loadd(epsOx);

		loadd(meSi);
		loadd(mhSi);
	}

	psolver.create(N);

	rho.resize(N);
	rhoi.resize(N);
	f.resize(N);

	signal(SIGINT, ihandler);

	cout << "N: " << N << ", C: " << C << ", w: " << w << ", rho0: " << rho0 << ", T: " << T << endl;
	cout << "tB: " << tB << ", tOx: " << tOx << ", VG: " << VG << endl;
	cout << "NA: " << NA << ", EgSi: " << EgSi << ", EgOx: " << EgOx << ", epsSi: " << epsSi << ", epsOx: " << epsOx << endl;
	cout << "meSi: " << meSi << ", mhSi: " << mhSi << endl;
}

void Sim::write()
{
	outfile.open(outpath, ios::out | ios::trunc);
	if (!outfile.is_open()) { 
		cout << "Can't open outfile!" << endl;
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < N; ++i) { // write to file
		double z = getZ(i);
		// z position, rho(z), phi(z), n(z), p(z)
		outfile << z << " " << rho[i] << " " << psolver.eval(z) << " " << n(z) << " " << p(z) << endl;
	}
	outfile.close();
}

#define DIVCRIT 1.e6 // criterion to stop when diverging

int Sim::run()
{
	for (auto &r : rho) // initial guess
		r = rho0;

	for (auto &r : rhoi)
		r = rho0;

	// set up Poisson solver
	psolver.setBounds(0., tB + tOx);
	psolver.setBC(0., VG);

	cout << endl;
	cout << "Starting to solve..." << endl;

	while (running) {
		for (int i = 0; i < N; ++i) { // calculate RHS
			f[i] = -rho[i] / getEps(getZ(i));
		}
		psolver.setRHS(&f[0]); // b = f

		psolver.solve(); // solve Poisson equation

		bool converged = true;
		double residual = 0., progress = 1.;
		
		for (int i = 0; i < N; ++i) { // calculate new rho(z)
			rhoi[i] = rho[i];

			double z = getZ(i);
			double rhoNew = CONST.e * (p(z) - n(z));
			if (z <= tB)
				rhoNew -= CONST.e * NA;

			rho[i] = w * rhoNew + (1. - w) * rhoi[i]; // damping

			if (rhoNew == 0.)
				continue;

			double res = abs(rhoNew - rhoi[i]) / abs(rhoNew); // residual for element

			if (res > C) // convergence not reached?
				converged = false;

			if (C / res < progress)
				progress = C / res;

			residual += res;
		}
		residual /= (double) N;
		
		cout << "iteration: " << ticks << ", residual: " << residual << " (" << progress << ")" << endl;

		++ticks;

		if (residual > DIVCRIT) {
			cout << "Diverging, halting..." << endl;
			return 2;
		}

		if (Vstep > 0. && converged) { // calculate inversion voltage VI by increasing VG
			bool nlarger = false;
			for (int i = 0; i < N; ++i) {
				double z = getZ(i);
				if (z > tB) // only bulk
					break;

				if (n(z) > p(z)) {
					nlarger = true;
					cout << "n > p, z = " << z << endl;
					break;
				}
			}
			if (nlarger) {
				cout << "VI: " << VG << endl;
			}
			else {
				VG += Vstep;
				psolver.setBC(0., VG);
				cout << "VG: " << VG << endl;
				converged = false;
			}
		}

		if (converged)
			running = false;

		if (rtWrite)
			write();
	}

	write();

	psolver.destroy();

	return 0;
}

double Sim::getZ(int i)
{
	return (double) i / (double) (N - 1) * (tB + tOx);
}

double Sim::getMe(double z)
{
	if (z <= tB) { // in silicon?
		return meSi * CONST.me0;
	}

	return CONST.me0;
}

double Sim::getMh(double z)
{
	if (z <= tB) { // in silicon?
		return mhSi * CONST.me0;
	}

	return CONST.me0;
}

double Sim::getEg(double z)
{
	if (z <= tB) // in silicon?
		return EgSi;
	else if (z <= tOx + tB) // in oxide?
		return EgOx;

	return 0.;
}

double Sim::getEps(double z)
{
	if (z <= tB) // in silicon?
		return epsSi * CONST.eps0;
	else if (z <= tOx + tB) // in oxide?
		return epsOx * CONST.eps0;

	return CONST.eps0;
}

double Sim::n(double z)
{
	if (z > tB) // insulator
		return 0.;

	double b = -Ec(z) / CONST.kB / T; // EF = 0

	// 1 / gamma(3/2) * F
	double I = 2. / sqrt(M_PI) * gsl_sf_fermi_dirac_half(b); // if b is too extreme, we get abort

	return sqrt(2.) / sqrt(CONST.kB * T) * pow(getMe(z), 3. / 2.) /
		(M_PI * M_PI * CONST.hbar * CONST.hbar * CONST.hbar) * I;
}

double Sim::p(double z)
{
	if (z > tB) // insulator
		return 0.;

	double b = Ev(z) / CONST.kB / T;

	double I = 2. / sqrt(M_PI) * gsl_sf_fermi_dirac_half(b);

	return sqrt(2.) / sqrt(CONST.kB * T) * pow(getMh(z), 3. / 2.) /
		(M_PI * M_PI * CONST.hbar * CONST.hbar * CONST.hbar) * I;
}

double Sim::Ev(double z)
{
	double mh = getMh(z);

	double el = mh * CONST.kB * T / (2. * M_PI * CONST.hbar * CONST.hbar);
	el = pow(el, 3. / 2.);
	el *= 2. / NA;

	double E = -CONST.kB * T * log(el);
	E -= CONST.e * psolver.eval(z);

	return E;
}

double Sim::Ec(double z)
{
	return Ev(z) + getEg(z);
}
