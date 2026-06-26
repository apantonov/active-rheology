#ifndef __CoulombABP_H__
#define __CoulombABP_H__

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#include "MersenneTwister.h"
#include "Vector.hpp"
#include "Helper.hpp"

/* Compile time switches to (de)activate some program features */

// If defined, the trajectories are logged into a file
#define OUTPUT_TRAJECTORIES

// If defined, the center of mass is logged into a file
//#define OUTPUT_CENTER

// If defined, the tracer is not logged into a file
//#define NO_TRACER_OUTPUT

// If defined, the particles are confined in a torus with periodic boundary conditions
//#define PERIODIC_BOUNDARIES

// If defined, the p(v) is defined in the same coarse-grained way as in the experimental realization
//#define EXPERIMENTAL_VELOCITY

// If defined, the tracer is subjected to Stokes friction
//#define STOKES_TRACER

// If defined, the Bristebots current is logged into a file
#define BBOT_CURRENT

// If defined, the Bristebots current is logged around the tracer
//#define TRACER_CURRENT

/* Data structure for transfering simulation parameters from the main program */
class CoulombABPOptions {
public:
	/* Total number of particles in the system */
	int N;
	/* Major axis length of the particles */
	double sigma1;
	/* Minor axis length of the particles */
	double sigma2;
	/* Translational diffusion coefficient */
	double D;
	/* Rotational diffusion coefficient */
	double Dr;
	/* Total simulation time (excluding equilibration time) */
	double tf;
	/* Duration of the equilibration period */
	double teq;
	/* Time step for the simulation algorithm */
	double dt;
	/* Radius of the arena */
	double R;
	/* Energy scale for particle-wall interactions */
	double epsWall;
	/* Energy scale for particle-particle interactions */
	double epsPart;
	/* Time interval between output data */
	double tfTrajectory;
	/* Size of the tracer */
	double R0;
	/* Drag force acting on the particles */
	double f;
	/* Bristebot mobility coefficient */
	double mu;
	/* Bristebot rotational mobility */
	double Mu;
	/* Tracer mass (in Bristebot units) */
	double M;
	/* Dry friction coefficient */
	double delta;
	/* Chirality of the particles */
	double omega;
	/* Activity level of the particles (set to zero for passive particles) */
	double f0;
	/* Self-alignment parameter for the particles */
	double kappa;

	/* This prefix is prepended before the filenames of all output file */
	std::string outputPrefix;

	CoulombABPOptions() {
		/* Set some basic default values */
		N = 20;
		sigma1 = 2.25;
		sigma2 = 0.75;
		D = 0.001;
		Dr = 0.001;
		f = 1.25;
		omega = 0.05;
		delta = 5.773;
		epsWall = 1.0;
		epsPart = 1.0;
		kappa = 1.0;
		f0 = 1.0;
		R = 19.0;
		R0 = 4.0;
		mu = 10.0;
		M = 2.0;
	}
};

class CoulombABP {

public:

		/* Total number of particles in the system */
	int N;
	/* Major axis length of the particles */
	double sigma1;
	/* Minor axis length of the particles */
	double sigma2;
	/* Translational diffusion coefficient */
	double D;
	/* Rotational diffusion coefficient */
	double Dr;
	/* Current simulation time */
	double t;
	/* Total simulation time (excluding equilibration time) */
	double tf;
	/* Duration of the equilibration period */
	double teq;
	/* Time step for the simulation algorithm */
	double dt;
	/* Radius of the arena */
	double R;
	/* Energy scale for particle-wall interactions */
	double epsWall;
	/* Energy scale for particle-particle interactions */
	double epsPart;
	/* Time interval between output data */
	double tfTrajectory;
	/* x-coordinate of the particles */
	Vector<double> *x;
	/* y-coordinate of the particles */
	Vector<double> *y;
	#ifdef PERIODIC_BOUNDARIES
	/* x-coordinate of the particles without considering periodic boundaries */
	Vector<double> *xreal;
	/* y-coordinate of the particles without considering periodic boundaries */
	Vector<double> *yreal;
	#endif
	#ifdef EXPERIMENTAL_VELOCITY
	/* Previous x-coordinate of the particles without considering periodic boundaries */
	Vector<double> *xold;
	/* Previous y-coordinate of the particles without considering periodic boundaries */
	Vector<double> *yold;
	#endif
	#ifdef BBOT_CURRENT
	/* To store the Bristebots current */
	Vector<double> *xcurrent;
	Vector<double> *ycurrent;
	Vector<double> *Ncurrent;
	#endif
	/* x-velocity of the particles */
	Vector<double> *vx;
	/* y-velocity of the particles */
	Vector<double> *vy;
	/* x-force acting on the particles */
	Vector<double> *fx;
	/* y-force acting on the particles */
	Vector<double> *fy;
	/* Orientation of the particles */
	Vector<double> *theta;
	/* Angular velocity of the particles */
	Vector<double> *Omega;
	/* Variance of translational white noise in the Langevin equation */
	double varNoise;
	/* Variance of rotational white noise in the Langevin equation */
	double rotNoise;
	/* Size of the tracer */
	double R0;
	/* Drag force acting on the particles */
	double f;
	/* Bristebot mobility coefficient */
	double mu;
	/* Bristebot rotational mobility */
	double Mu;
	/* Tracer mass (in Bristebot units) */
	double M;
	/* Dry friction coefficient */
	double delta;
	/* Chirality of the particles */
	double omega;
	/* Activity of the particles (set to zero for passive particles) */
	double f0;
	/* Self-alignment parameter for the particles */
	double kappa;

	/* Density data */
	int bin; // Number of bins in the interval [0, 1[
	int length_bin; // Interval [-l, l] for the probability distribution p(v)
	Vector<double> *rho_x;
	Vector<double> *rho_y;

	/* MersenneTwister Random Number Generator */
	MTRand randy;
	#endif
	FILE *trajectoryFile;
	FILE *xdensityFile;
	FILE *ydensityFile;
	#ifdef BBOT_CURRENT
	FILE *currentFile;
	#endif
	double tTrajectory;
	std::string filePrefix;

	/* Constructor that initializes memory and sets parameters given by CoulombABPOptions object */
	CoulombABP(const CoulombABPOptions &opt) {
		N = opt.N;
		x = new Vector<double>(N+1);
		y = new Vector<double>(N+1);
		#ifdef PERIODIC_BOUNDARIES
		xreal = new Vector<double>(N+1);
		yreal = new Vector<double>(N+1);
		#endif
		#ifdef EXPERIMENTAL_VELOCITY
		xold = new Vector<double>(N+1);
		yold = new Vector<double>(N+1);
		#endif
		vx = new Vector<double>(N+1);
		vy = new Vector<double>(N+1);
		fx = new Vector<double>(N+1);
		fy = new Vector<double>(N+1);
		theta = new Vector<double>(N+1);
		Omega = new Vector<double>(N+1);

		bin = 10; // Number of bins in the interval [0, 1[
		length_bin = 10; // Interval [-l, l] for the probability distribution p(v)
		rho_x = new Vector<double>(2 * bin * length_bin);
		rho_y = new Vector<double>(2 * bin * length_bin);

		dt = opt.dt;
		D = opt.D;
		Dr = opt.Dr;
		sigma1 = opt.sigma1;
		sigma2 = opt.sigma2;
		R = opt.R;
		epsPart = opt.epsPart;
		epsWall = opt.epsWall;
		mu = opt.mu;
		Mu = 0.2 * mu;
		M = opt.M;
		delta = opt.delta;
		f = opt.f;
		f0 = opt.f0;
		kappa = opt.kappa;
		tf = opt.tf;
		teq = opt.teq;
		R0 = opt.R0;
		omega = opt.omega;
		
		#ifdef BBOT_CURRENT
		xcurrent = new Vector<double>(2*(std::lround(2.0*R)+1)*std::lround(R));
		ycurrent = new Vector<double>(2*(std::lround(2.0*R)+1)*std::lround(R));
		Ncurrent = new Vector<double>(2*(std::lround(2.0*R)+1)*std::lround(R));
		#endif

		filePrefix = opt.outputPrefix;
		tfTrajectory = opt.tfTrajectory;

	}
	
		
		double getRandomInRange(double scale) {
			return randy.rand() * scale; 
		}

		double normalize_angle(double angle) {
			angle = fmod(angle, 2 * M_PI);  // Reduce the angle to the range [-2π, 2π]
			if (angle < 0) {
				angle += 2 * M_PI;  // Adjust negative angles to [0, 2π]
			}
			return angle;
		}

		double distance(double x, double y) {
		return std::sqrt(x * x + y * y);
		}
			
		// Calculate effective distance sigma_eff for ellipses based on orientation and r
		double calculateEffectiveDistance(double dx, double dy, double theta_i, double theta_j, double a, double b) {
			
			double phi = std::atan2(dy, dx);	// Angle of vector r

			// Project semi-axes along the line of centers
			double A_i = a * std::sin(theta_i - phi);
			double B_i = b * std::cos(theta_i - phi);
			double A_j = a * std::sin(theta_j - phi);
			double B_j = b * std::cos(theta_j - phi);

			// Effective interaction distance sigma_eff
			double sq_i = A_i * A_i + B_i * B_i;
			double sq_j = A_j * A_j + B_j * B_j;
			double sigma_eff = 2.0 * a * b / std::sqrt(std::sqrt(sq_i * sq_j));
			
			return sigma_eff;
		}
		
		double calculateTracerDistance(double dx, double dy, double theta_i, double a, double b) {
			
			double phi = std::atan2(dy, dx);	// Angle of vector r

			// Project semi-axes along the line of centers
			double A_i = a * std::sin(theta_i - phi);
			double B_i = b * std::cos(theta_i - phi);

			// Effective interaction distance sigma_eff
			double sq = A_i * A_i + B_i * B_i;
			double sigma_eff = 2.0 * std::sqrt(a * b * R0) / std::sqrt(std::sqrt(sq));

			return sigma_eff;
		}
		
		#include <cmath>
#include <iostream>

double elastic(double x, double y, double theta, double x0, double y0)
{
	double dx1 = std::cos(theta);
	double dy1 = std::sin(theta);
	double dx2 = x0 - x;
	double dy2 = y0 - y;
	double norm2 = distance(dx2, dy2);
	if (norm2 == 0.0) return 1.0;
	dx2 /= norm2;
	dy2 /= norm2;

	double dot = dx1*dx2 + dy1*dy2;

	if (dot > 1.0) dot = 1.0;
	if (dot < 0.0) dot = 0.0;

	// Return elasticity, always in [0,1]
	return 1.0;
}


		
		double calculateArenaContact(double xi, double yi, double xc, double yc, double r0) {
			
			if (xi == 0) {
				double delta = r0 * r0 - xc * xc;
				double sqrtDelta = std::sqrt(delta);
				double val1 = yc * yc + delta + 2 * yc * sqrtDelta;
				double val2 = yc * yc + delta - 2 * yc * sqrtDelta;
				return std::sqrt(std::max(val1, val2));
			}
			else	{
				double k = yi / xi;
				double A = 1 + k * k;
				double B = xc + k * yc;
				double C = xc * xc + yc * yc - r0 * r0;

				double sqrtDisc = std::sqrt(B * B - A * C);

				// Two possible x values
				double x1 = (B + sqrtDisc) / A;
				double x2 = (B - sqrtDisc) / A;

				// Compute x^2 + y^2 = (1 + k^2) * x^2
				double val1 = A * x1 * x1;
				double val2 = A * x2 * x2;

				return std::sqrt(std::max(val1, val2));
			}
		}

		// Function to calculate orientation-dependent WCA force
		void calculateWCAForce() {
			double xi, yi, xj, yj, dx, dy, r, sigma, force_magnitude;
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < i; ++j) {
					xi = (*x)[i];
					xj = (*x)[j];
					yi = (*y)[i];
					yj = (*y)[j];
					#ifdef PERIODIC_BOUNDARIES
					if (fabs(xi - xj) > R) {
						if (xi > xj) xj += 2.0 * R;
						else xi += 2.0 * R;
					}
					if (fabs(yi - yj) > R) {
						if (yi > yj) yj += 2.0 * R;
						else yi += 2.0 * R;
					}
					#endif
					dx = xj - xi;
					dy = yj - yi;
					r = distance(dx, dy);
					sigma = calculateEffectiveDistance(dx, dy, (*theta)[i], (*theta)[j], sigma1, sigma2);
					force_magnitude = 0.0;

					if (r < 1.12246204831 * sigma) {
						double inv_r = sigma / r;
						double inv_r2 = inv_r * inv_r;
						double inv_r6 = inv_r2 * inv_r2 * inv_r2;
						double inv_r12 = inv_r6 * inv_r6;

						force_magnitude = 24.0 * epsPart / r * (2.0 * inv_r12 - inv_r6);
					}
					(*fx)[i] -= dx / r * force_magnitude;
					(*fx)[j] += dx / r * force_magnitude;
					(*fy)[i] -= dy / r * force_magnitude;
					(*fy)[j] += dy / r * force_magnitude;
				}
			}
		}

		void calculateArena() {
			
			/* Number of petal segments*/
			int segments = 8;
			int segment;
			/* Radius of petal segments*/
			double r0 = 8.0;
			double xi, yi, xc, yc, rmax, angle;
			xi = 0.0;
			yi = 0.0;
			for (int i = 0; i < N; ++i) {
				rmax = 0.0;
				angle = normalize_angle(std::atan2((*y)[i], (*x)[i]));
				// Petal segment number 
				segment = std::floor((angle + M_PI / segments) / (2.0 * M_PI / segments));
				// Center of the petal segment
				xc = (R - r0) * std::cos(2.0 * M_PI * segment / segments);
				yc = (R - r0) * std::sin(2.0 * M_PI * segment / segments);
				for (int t = 0; t < 100; ++t) {
					double xt = (*x)[i] - xc + sigma1 * std::cos(0.02 * M_PI * t) * std::cos((*theta)[i]) - sigma2 * std::sin(0.02 * M_PI * t) * std::sin((*theta)[i]);
					double yt = (*y)[i] - yc + sigma1 * std::cos(0.02 * M_PI * t) * std::sin((*theta)[i]) + sigma2 * std::sin(0.02 * M_PI * t) * std::cos((*theta)[i]);
					if (((xt * xt + yt * yt) > rmax)) {
						rmax = xt * xt + yt * yt;
						xi = xt;
						yi = yt;
					}
				}
				double sigma = 2.0 * std::sqrt(sigma1 * sigma2);
				double force_magnitude = 0.0;
				if (distance(xi + xc, yi + yc) + 0.12246204831 * sigma >= calculateArenaContact(xi + xc, yi + yc, xc, yc, r0)) {
					xi = xi / std::sqrt(rmax) * (r0 + 0.5 * sigma);
					yi = yi / std::sqrt(rmax) * (r0 + 0.5 * sigma);
					rmax = distance((*x)[i] - xc - xi, (*y)[i] - yc - yi);
					if ((rmax < sigma) || (distance((*x)[i] - xc, (*y)[i] - yc) > r0)) {
						rmax = distance((*x)[i] - xc, (*y)[i] - yc);
						force_magnitude = epsWall * (rmax + sigma - r0); // * pow(sigma - rmax, 1.0);
					}
					(*fx)[i] -= ((*x)[i] - xc) / distance((*x)[i] - xc, (*y)[i] - yc) * force_magnitude;
					(*fy)[i] -= ((*y)[i] - yc) / distance((*x)[i] - xc, (*y)[i] - yc) * force_magnitude;

				}

			}
		}

		void vectorfield() {
			for (int i = 0; i < N; ++i)	{
				#ifdef BBOT_CURRENT
					#ifndef TRACER_CURRENT
					if ((fabs((*x)[i]) < R) && (fabs((*y)[i]) < R)) {
						(*xcurrent)[std::lround((*x)[i]+R)*2*std::lround(R)+std::lround((*y)[i])+std::lround(R)] += (*vx)[i];
						(*ycurrent)[std::lround((*x)[i]+R)*2*std::lround(R)+std::lround((*y)[i])+std::lround(R)] += (*vy)[i];
						(*Ncurrent)[std::lround((*x)[i]+R)*2*std::lround(R)+std::lround((*y)[i])+std::lround(R)] += 1.0;
					}
					#endif

					#ifdef TRACER_CURRENT
					if ((fabs((*x)[i] - (*x)[N]) < R) && (fabs((*y)[i] - (*y)[N]) < R)) {
						(*xcurrent)[ std::lround((*x)[i] - (*x)[N] + R) * 2 * std::lround(R)
									+ std::lround((*y)[i] - (*y)[N] + R) ] += (*vx)[i];

						(*ycurrent)[ std::lround((*x)[i] - (*x)[N] + R) * 2 * std::lround(R)
									+ std::lround((*y)[i] - (*y)[N] + R) ] += (*vy)[i];

						(*Ncurrent)[ std::lround((*x)[i] - (*x)[N] + R) * 2 * std::lround(R)
									+ std::lround((*y)[i] - (*y)[N] + R) ] += 1.0;
					}
					#endif
				#endif
			}
		}

		void calculateTracer() {
			
			double xi, yi, xj, yj, dx, dy, r, sigma, force_magnitude;
			
			for (int i = 0; i < N; ++i) {
				xi = (*x)[i];
				yi = (*y)[i];
				xj = (*x)[N];
				yj = (*y)[N];
				#ifdef PERIODIC_BOUNDARIES
				if (fabs(xi - xj) > R) {
					if (xi > xj) xj += 2.0 * R;
					else xi += 2.0 * R;
				}
				if (fabs(yi - yj) > R) {
					if (yi > yj) yj += 2.0 * R;
					else yi += 2.0 * R;
				}
				#endif
				dx = xj - xi;
				dy = yj - yi;
				r = distance(dx, dy);
				sigma = calculateTracerDistance(dx, dy, (*theta)[i], sigma1, sigma2);
				force_magnitude = 0.0;
				
				if (r < 1.12246204831 * sigma) {
					double inv_r = sigma / r;
					double inv_r2 = inv_r * inv_r;
					double inv_r6 = inv_r2 * inv_r2 * inv_r2;
					double inv_r12 = inv_r6 * inv_r6;

					force_magnitude = 24.0 * epsPart / r * (2.0 * inv_r12 - inv_r6);
				}
				(*fx)[i] -= dx / r * force_magnitude;
				(*fy)[i] -= dy / r * force_magnitude;
				(*fx)[N] += dx / r * force_magnitude / M * elastic(xi, yi, (*theta)[i], xj, yj);
				(*fy)[N] += dy / r * force_magnitude / M * elastic(xi, yi, (*theta)[i], xj, yj);
			}
		}

		void applyPeriodic() {
			for (int i = 0; i < N; ++i) {
				if ((*x)[i] > R) (*x)[i] -= 2 * R;
				if ((*x)[i] < -R) (*x)[i] += 2 * R;
				if ((*y)[i] > R) (*y)[i] -= 2 * R;
				if ((*y)[i] < -R) (*y)[i] += 2 * R;
			}
			if ((*x)[N] > R) (*x)[N] -= 2 * R;
			if ((*x)[N] < -R) (*x)[N] += 2 * R;
			if ((*y)[N] > R) (*y)[N] -= 2 * R;
			if ((*y)[N] < -R) (*y)[N] += 2 * R;
		}

		void WallTracer() {

			/* Number of petal segments*/
			int segments = 8;
			int segment;
			/* Radius of petal segments*/
			double r0 = 8.0;
			
			double angle = normalize_angle(std::atan2((*y)[N], (*x)[N]));
			// Petal segment number 
			segment = std::floor((angle + M_PI / segments) / (2.0 * M_PI / segments));
			// Center of the petal segment
			double xc = (R - r0) * std::cos(2.0 * M_PI * segment / segments);
			double yc = (R - r0) * std::sin(2.0 * M_PI * segment / segments);
			double r = r0 - distance((*x)[N] - xc, (*y)[N] - yc);
			double force_magnitude = 0.0;
			if (fabs(r0 - r) < 1e-8) r = r0 - 1e-8;
			if (distance((*x)[N], (*y)[N]) > distance(xc, yc)) {
				if (r < R0) {
					double inv_r = R0 / r;
					double inv_r2 = inv_r * inv_r;       // R0^2 / r^2
					double inv_r6 = inv_r2 * inv_r2 * inv_r2; // (R0/r)^6
					double inv_r12 = inv_r6 * inv_r6;    // (R0/r)^12

					force_magnitude = 24.0 * epsWall / r * (2.0 * inv_r12 - inv_r6);
				}
				(*fx)[N] -= ((*x)[N] - xc) /(r0 -r) * force_magnitude;
				(*fy)[N] -= ((*y)[N] - yc) /(r0 -r) * force_magnitude;
			}
		}

// Initialize ellipse positions and orientations inside the large circle and outside the tracer circle
		void init() {
			
			// Put all outside the circle first
			for (int i = 0; i < N; ++i) {

				(*x)[i] = R;
				(*y)[i] = R;
				(*vx)[i] = 0.0;
				(*vy)[i] = 0.0;
				(*theta)[i] = 0.0;
				(*Omega)[i] = 0.0;
			}
			for (int i = 0; i < 2 * bin * length_bin; ++i) {
				(*rho_x)[i] = 0.0;
				(*rho_y)[i] = 0.0;
			}
			
			#ifdef BBOT_CURRENT
			for (int i = 0; i < 2*(std::lround(2.0*R)+1)*std::lround(R); ++i) {
				(*xcurrent)[i] = 0.0;
				(*ycurrent)[i] = 0.0;
				(*Ncurrent)[i] = 0.0;
			}
			#endif

			double Yinit = 10.0;
			// Initializing tracer
			(*x)[N] = 0.0; (*y)[N] = Yinit;
			(*vx)[N] = 0.0; (*vy)[N] = 0.0;
			(*theta)[N] = 0.0; (*Omega)[N] = 0.0;
		
			for (int i = 0; i < N; ++i) {
			do {
				(*fx)[i] = 0.0;
				(*fy)[i] = 0.0;


				#ifndef PERIODIC_BOUNDARIES
				double r = std::abs(getRandomInRange(R - sigma1));  // Radial distance within the large circle
				double angle = getRandomInRange(2.0 * M_PI);	 // Random angle in [0, 2π]

				// Convert polar to Cartesian coordinates
				(*x)[i] = r * std::cos(angle);
				(*y)[i] = r * std::sin(angle);
				#else
				(*x)[i] = std::abs(getRandomInRange(2.0 * R)) - R;
				(*y)[i] = std::abs(getRandomInRange(2.0 * R)) - R;
				#endif

				(*theta)[i] = getRandomInRange(2.0 * M_PI); // Random orientation
				calculateWCAForce();
			
			}
			while ((fabs((*fx)[i]) > 1.0) || (fabs((*fy)[i]) > 1.0) || ((*x)[i] * (*x)[i] + ((*y)[i] - Yinit) * ((*y)[i] - Yinit) < (R0 + sigma1) * (R0 + sigma1)));
			}
//			Initial setup can be set manually, e.g: (*x)[0] = 0.0; (*y)[0] = 3.0; (*vy)[0] = 2.0; (*theta)[0]=M_PI*0.5;
						
			/* #ifdef PERIODIC_BOUNDARIES
			(*xreal) = (*x);
			(*yreal) = (*y);
			#endif */

			rotNoise = sqrt(2 * Dr * dt);
			varNoise = sqrt(2 * D * dt);
			t = 0.0;
		}



	void equilibaStep() {
		int i;

		for (i = 0; i < N; ++i) {
			(*fx)[i] = 0.0;
			(*fy)[i] = 0.0;
		}

		#ifndef PERIODIC_BOUNDARIES
		calculateArena();
		#endif
		calculateTracer();
		calculateWCAForce();

		for (i = 0; i < N; ++i) {
			(*x)[i] += dt * (*vx)[i];
			(*y)[i] += dt * (*vy)[i];
			/* #ifdef PERIODIC_BOUNDARIES
			(*xreal)[i] += dt * (*vx)[i];
			(*yreal)[i] += dt * (*vy)[i];
			#endif */
			(*vx)[i] += dt * ((*fx)[i] + f0 * mu * std::cos((*theta)[i]) - mu * (*vx)[i]) + mu * varNoise * randy.randNorm();
			(*vy)[i] += dt * ((*fy)[i] + f0 * mu * std::sin((*theta)[i]) - mu * (*vy)[i]) + mu * varNoise * randy.randNorm();
			if (std::sqrt((*vx)[i] * (*vx)[i] + (*vy)[i] * (*vy)[i]) >= 1e-16) {
				(*theta)[i] += omega * dt - kappa * ((*vx)[i] * std::sin((*theta)[i]) - (*vy)[i] * std::cos((*theta)[i])) / std::sqrt((*vx)[i] * (*vx)[i] + (*vy)[i] * (*vy)[i]) * dt;
			}
			else (*theta)[i] += omega * dt;
//			For inertial underdamped dynamics: (*Omega)[i] += Mu * dt * (omega - (*Omega)[i]) + Mu * rotNoise * randy.randNorm();
		}


		t += dt;	

		#ifdef PERIODIC_BOUNDARIES
		applyPeriodic();
		#endif


	}
	
	void step() {
		int i;
		double V;
		for (i = 0; i < N; ++i) {
			(*fx)[i] = 0.0;
			(*fy)[i] = 0.0;
		}

		for (i = 0; i <= N; ++i) {
			if (distance((*vx)[i], (*vy)[i]) > f0) {
				V = distance((*vx)[i], (*vy)[i]);
				(*vx)[i] /= V;
				(*vy)[i] /= V;
			}
		}
		(*fx)[N] = 0.0;
		(*fy)[N] = -f;

		calculateWCAForce();
		calculateTracer();
		#ifndef PERIODIC_BOUNDARIES
		calculateArena();
		WallTracer();
		#endif
		vectorfield();



		for (i = 0; i < N; ++i) {
			(*x)[i] += dt * (*vx)[i];
			(*y)[i] += dt * (*vy)[i];
			#ifdef PERIODIC_BOUNDARIES
			(*xreal)[i] += dt * (*vx)[i];
			(*yreal)[i] += dt * (*vy)[i];
			#endif
			(*vx)[i] += dt * ((*fx)[i] + f0 * mu * std::cos((*theta)[i]) - mu * (*vx)[i]) + mu * varNoise * randy.randNorm();
			(*vy)[i] += dt * ((*fy)[i] + f0 * mu * std::sin((*theta)[i]) - mu * (*vy)[i]) + mu * varNoise * randy.randNorm();
			if (std::sqrt((*vx)[i] * (*vx)[i] + (*vy)[i] * (*vy)[i]) >= 1e-16) {
				(*theta)[i] += omega * dt - kappa * ((*vx)[i] * std::sin((*theta)[i]) - (*vy)[i] * std::cos((*theta)[i])) / std::sqrt((*vx)[i] * (*vx)[i] + (*vy)[i] * (*vy)[i]) * dt + rotNoise * randy.randNorm();
			}
			else (*theta)[i] += omega * dt + rotNoise * randy.randNorm();
//			For inertial underdamped dynamics: (*Omega)[i] += Mu * dt * (omega - (*Omega)[i]) + Mu * rotNoise * randy.randNorm();
		}

		(*x)[N] += dt * (*vx)[N];
		(*y)[N] += dt * (*vy)[N];
		
		#ifdef PERIODIC_BOUNDARIES
		(*xreal)[N] += dt * (*vx)[N];
		(*yreal)[N] += dt * (*vy)[N];
		#endif
		
		V = std::sqrt((*vx)[N] * (*vx)[N] + (*vy)[N] * (*vy)[N]);
		if (V < 1e-8) V = 1e-8;
		if (std::sqrt((*fx)[N] * (*fx)[N] + (*fy)[N] * (*fy)[N]) > 1e-8)	{
			#ifndef STOKES_TRACER
			(*vx)[N] += dt * ((*fx)[N] - (*vx)[N] / V * delta);
			(*vy)[N] += dt * ((*fy)[N] - (*vy)[N] / V * delta);
			#else
			(*vx)[N] += dt * ((*fx)[N] - (*vx)[N] * mu);
			(*vy)[N] += dt * ((*fy)[N] - (*vy)[N] * mu);
			#endif
		}
		else {
			#ifndef STOKES_TRACER
			if (fabs(V) < delta * dt) {
				(*vx)[N] = 0.0; (*vy)[N] = 0.0;
			}
			else {
				(*vx)[N] -= dt * (*vx)[N] * delta / V;
				(*vy)[N] -= dt * (*vy)[N] * delta / V;
			}
			#else
				(*vx)[N] = (*vx)[N] * std::exp(-mu * dt);
				(*vy)[N] = (*vy)[N] * std::exp(-mu * dt);
			#endif
		}	

		t += dt;
		
		
		#ifndef EXPERIMENTAL_VELOCITY
		if (fabs((*vx)[N]) < length_bin) (*rho_x)[round(((*vx)[N] + length_bin) * bin)] += 1.0;
		if (fabs((*vy)[N]) < length_bin) (*rho_y)[round(((*vy)[N] + length_bin) * bin)] += 1.0;
		#endif

		#ifdef PERIODIC_BOUNDARIES
		applyPeriodic();
		#endif

	}

	/* Do an equilibration run without measurements for the time given by equilibrationTime */
	void equilibrate(double equilibrationTime) {
		t = 0.0;
		while (t < equilibrationTime) {
			equilibaStep();
		}
	}
	
	/* Open all necessary files for logging the measured results */
	void openOutputFiles()  {
		std::string filename = filePrefix + "-trajectories.csv";
		trajectoryFile = fopen(filename.c_str(), "w");
		if (trajectoryFile == NULL) {
			std::cerr << "ERROR: Unable to open trajectory file " << filename << std::endl;
			exit(EXIT_FAILURE);
		}
	filename = filePrefix + "-xdensity.csv";
	xdensityFile = fopen(filename.c_str(), "w");
	if (xdensityFile == NULL) {
		std::cerr << "ERROR: Unable to open xdensityFile file " << filename << std::endl;
		exit(EXIT_FAILURE);
	}
	filename = filePrefix + "-ydensity.csv";
	ydensityFile = fopen(filename.c_str(), "w");
	if (ydensityFile == NULL) {
		std::cerr << "ERROR: Unable to open ydensityFile file " << filename << std::endl;
		exit(EXIT_FAILURE);
	}
	#ifdef BBOT_CURRENT
	filename = filePrefix + "-current.csv";
	currentFile = fopen(filename.c_str(), "w");
	if (currentFile == NULL) {
		std::cerr << "ERROR: Unable to open currentFile file " << filename << std::endl;
		exit(EXIT_FAILURE);
	}
	#endif
		
	}
	

	/* Close all files */
	void closeOutputFiles() {
	fclose(trajectoryFile);
	fclose(xdensityFile);
	fclose(ydensityFile);
	#ifdef BBOT_CURRENT
	fclose(currentFile);
	#endif
	}

	/* Run simulation for length of finalTime */
	void run(double finalTime) {
		tf = finalTime;
		t = 0.0;
		tTrajectory = 0.0;
		int i;
		
		#ifdef PERIODIC_BOUNDARIES
		(*xreal) = (*x);
		(*yreal) = (*y);
		#endif
		
		#ifdef EXPERIMENTAL_VELOCITY
			#ifdef PERIODIC_BOUNDARIES
			(*xold) = (*xreal);
			(*yold) = (*yreal);
			#else
			(*xold) = (*x);
			(*yold) = (*y);
			#endif
		#endif
		
		/* Main simulation loop */
		while (t < tf) {

			
			step();
	  
			tTrajectory += dt;
			if (tTrajectory >= tfTrajectory) {
				tTrajectory = 0.0;
				#ifdef EXPERIMENTAL_VELOCITY
					#ifndef PERIODIC_BOUNDARIES
					if (fabs((*x)[N] - (*xold)[N]) / tfTrajectory < length_bin) (*rho_x)[round((((*x)[N] - (*xold)[N]) / tfTrajectory + length_bin) * bin)] += 1.0;
					if (fabs((*y)[N] - (*yold)[N]) / tfTrajectory < length_bin) (*rho_y)[round((((*y)[N] - (*yold)[N]) / tfTrajectory + length_bin) * bin)] += 1.0;
					(*xold) = (*x);
					(*yold) = (*y);
					#else
					if (fabs((*xreal)[N] - (*xold)[N]) / tfTrajectory < length_bin) (*rho_x)[round((((*xreal)[N] - (*xold)[N]) / tfTrajectory + length_bin) * bin)] += 1.0;
					if (fabs((*yreal)[N] - (*yold)[N]) / tfTrajectory < length_bin) (*rho_y)[round((((*yreal)[N] - (*yold)[N]) / tfTrajectory + length_bin) * bin)] += 1.0;
					(*xold) = (*xreal);
					(*yold) = (*yreal);
					#endif
				#endif
			#ifndef NO_TRACER_OUTPUT
			double xi, yi;

				#ifndef PERIODIC_BOUNDARIES
				xi = (*x)[N];
				yi = (*y)[N];
				#else
				xi = (*xreal)[N];
				yi = (*yreal)[N];
				#endif

			fprintf(trajectoryFile,
					"%f, %d, %f, %f, %f, %f, %f, %f, %f, %f\n",
					t,
					0,
					xi,
					yi,
					(*theta)[N],
					(*Omega)[N],
					(*vx)[N],
					(*vy)[N],
					(*fx)[N],
					(*fy)[N]);
			#endif
			#ifdef OUTPUT_TRAJECTORIES
			for (i = 0; i < N; ++i) {
				fprintf(trajectoryFile,
						"%f, %d, %f, %f, %f, %f, %f, %f, %f, %f\n",
						t,
						i + 1,
						(*x)[i],
						(*y)[i],
						(*theta)[i],
						(*Omega)[i],
						(*vx)[i],
						(*vy)[i],
						(*fx)[i],
						(*fy)[i]);
			}
			#endif
				#ifdef OUTPUT_CENTER
				fprintf(trajectoryFile, "%f, ", t);
					#ifndef NO_TRACER_OUTPUT
					fprintf(trajectoryFile, "%d, ", -1); // this indexing is used to preserve sequential numbering
					#else
					fprintf(trajectoryFile, "%d, ", 0);
					#endif
				double xc = 0.0, yc = 0.0;
				double thetac = 0.0, omegac = 0.0;
				double vxc = 0.0, vyc = 0.0;
				double fxc = 0.0, fyc = 0.0;
				for (i = 0; i < N; ++i) {
					#ifdef PERIODIC_BOUNDARIES
					xc += (*xreal)[i];
					yc += (*yreal)[i];
					#else
					xc += (*x)[i];
					yc += (*y)[i];
					#endif
					thetac += (*theta)[i];
					omegac += (*Omega)[i];
					vxc += (*vx)[i];
					vyc += (*vy)[i];
					fxc += (*fx)[i];
					fyc += (*fy)[i];
				}				
				fprintf(trajectoryFile,
						"%f, %f, %f, %f, %f, %f, %f, %f\n",
						xc / N,
						yc / N,
						thetac / N,
						omegac / N,
						vxc / N,
						vyc / N,
						fxc / N,
						fyc / N);
				#endif
				
			}


		}
	#ifdef BBOT_CURRENT
	for (i = 0; i < 2*(std::lround(2.0*R)+1)*std::lround(R); i++) {
		fprintf(currentFile, "%ld, ", i / std::lround(2.0*R) - std::lround(R));
		fprintf(currentFile, "%ld, ", i % std::lround(2.0*R) - std::lround(R));
		if ((*Ncurrent)[i] > 0.0) {
			fprintf(currentFile, "%f, ", (*xcurrent)[i]/(*Ncurrent)[i]);
			fprintf(currentFile, "%f\n", (*ycurrent)[i]/(*Ncurrent)[i]);
		}
		else {
			fprintf(currentFile, "%f, ", 0.0);
			fprintf(currentFile, "%f\n", 0.0);
		}
			
	}
	#endif

	double sum = 0.0;
	for (i = 0; i < 2 * bin * length_bin; i++)
		  sum += (*rho_x)[i];
		 for (i = 0; i < 2*bin*length_bin; i++) {
			  fprintf(xdensityFile, "%f, ", (i - bin*length_bin) / double(bin));
			  fprintf(xdensityFile, "%e\n", (*rho_x)[i] * bin / sum);
	}
	sum = 0.0;
	
		 for (i = 0; i < 2 * bin * length_bin; i++)
			  sum += (*rho_y)[i];
		 for (i = 0; i < 2 * bin * length_bin; i++) {
			  fprintf(ydensityFile, "%f, ", (i - bin*length_bin) / double(bin));
			  fprintf(ydensityFile, "%e\n", (*rho_y)[i] * bin / sum);
	}
	}

};

