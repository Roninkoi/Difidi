#include <gsl/gsl_sf_fermi_dirac.h>
#include <signal.h>

#include "sim.h"

int signalCode = 0; // caught signal

void signalHandler(int sn)
{
	if (sn == SIGINT) {
		signalCode = sn;
	}
}

// load integer from string s with variable name of v
#define loadi(v)						\
	if (s.compare(#v) == 0) {			\
		infile >> s;				\
		v = stoi(s);				\
		continue;					\
	}

// load double from string s with variable name of v
#define loadd(v)						\
	if (s.compare(#v) == 0) {			\
		infile >> s;				\
		v = stod(s);				\
		continue;					\
	}

// load string s with variable name of v
#define loads(v)						\
	if (s.compare(#v) == 0) {			\
		infile >> s;				\
		v = s;					\
		continue;					\
	}

Sim::Sim(string ipath, string opath) :
	infile(ipath), outpath(opath)
{
	running = true;
	
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
		loads(rhopath);
		
		loadd(T);

		loadd(tB);
		loadd(tOx);
		loadd(VG);
		loadd(Vmax);
		loadd(Vstep);

		loadd(NA);

		loadd(EgSi);
		loadd(EgOx);

		loadd(epsSi);
		loadd(epsOx);

		loadd(meSi);
		loadd(mhSi);
		loadd(meOx);
		loadd(mhOx);
	}

	psolver.create(N);

	rho.resize(N);
	rhoi.resize(N);
	f.resize(N);

	signal(SIGINT, signalHandler);

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
		// z position, rho(z), phi(z), n(z), p(z), Ev(z), Ec(z)
		outfile << z << " " << rho[i] << " " << psolver.eval(z) << " " << n(z) << " " << p(z) << " " << Ev(z) << " " << Ec(z) << endl;
	}
	outfile.close();
}

#define DIVCRIT 1.e6 // criterion to stop when diverging

int Sim::run()
{
	int result = 0;
	
	for (auto &r : rho) // initial guess
		r = rho0;

	for (auto &r : rhoi)
		r = rho0;
	
	rhofile.open(rhopath);
	if (rhofile.good()) {
		for (int i = 0; i < N; ++i) {
			double z0, rho0, sol0, n0, p0, Ev0, Ec0;
			rhofile >> z0 >> rho0 >> sol0 >> n0 >> p0 >> Ev0 >> Ec0;
			rho[i] = rho0;
			rhoi[i] = rho0;
		}
	}
	rhofile.close();

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
		double residual = 0., sresidual = 0., progress = 1.;
		
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
			//double res = abs(rho[i] - rhoi[i]) / abs(rho[i]);

			if (res > C) // convergence not reached?
			  converged = false;
		
			if (C / res < progress)
				progress = C / res;

			residual += res;

			if (signalCode) {
				result = 1;
				running = false;
				break;
			}
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
			if (nlarger || VG >= Vmax) {
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

	return result;
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
	else if (z <= tOx + tB) // in oxide?
		return meOx * CONST.me0;

	return CONST.me0;
}

double Sim::getMh(double z)
{
	if (z <= tB) { // in silicon?
		return mhSi * CONST.me0;
	}
	else if (z <= tOx + tB) // in oxide?
		return mhOx * CONST.me0;

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

#define MAXB 40.

double Sim::n(double z)
{
	if (z > tB) // insulator
		return 0.;

	double b = -Ec(z) / CONST.kB / T; // EF = 0

	if (abs(b) > MAXB)
		return 0.;

	// gamma(3/2) * F
	double I = sqrt(M_PI) / 2. * gsl_sf_fermi_dirac_half(b); // if b is too extreme, we get abort

	return sqrt(2.) * pow(getMe(z) * CONST.kB * T, 3. / 2.) /
		(M_PI * M_PI * CONST.hbar * CONST.hbar * CONST.hbar) * I;
}

double Sim::p(double z)
{
	if (z > tB) // insulator
		return 0.;

	double b = Ev(z) / CONST.kB / T;

	if (abs(b) > MAXB)
		return 0.;
	
	double I = sqrt(M_PI) / 2. * gsl_sf_fermi_dirac_half(b);

	return sqrt(2.) * pow(getMh(z) * CONST.kB * T, 3. / 2.) /
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
