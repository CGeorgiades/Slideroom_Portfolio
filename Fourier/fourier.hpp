//#include <complex.h>
#include <math.h>
#include "calculus.hpp"
#define complex _Complex
#include <time.h>
/**
 This will implement the complex usage of fourier expansions of
 given complex functions
 **/
//Maximum permitted depth
const int MAX_DEPTH = 1000;
//Current 'target' depth, the calc thread will compute up to here
static volatile int depth_goal = 1000;


//double complex (*f)(double);
extern volatile bool isInFourierDrawMode;
double complex fourier_integrate(void (*func)(double, double*, double*),
    double xmin, double xmax, double n, double step)
	{
		double sumx, sumy, tx, ty, t2x, t2y;
		sumx = sumy = tx = ty = t2x = t2y = 0;
		double tmp = -2 * n * M_PI;
		double tmpx = cos(-2 * n * M_PI);
		double tmpy = sin(-2 * n * M_PI);
		for (double i = xmin; i < xmax - step;) {
			//sum += f(i) / 2 + f(i+h) / 2;
			func(i, &tx, &ty);
			func(i + step, &t2x, &t2y);
			double cs1 = cos(tmp * i);
			double sn1 = sin(tmp * i);
			double cs2 = cos(tmp * (i + step));
			double sn2 = sin(tmp * (i + step));
			sumx += ((tx * cs1 - ty * sn1) + (t2x * cs2 - t2y * sn2)) / 2;
			sumy += ((ty * cs1 + tx * sn1) + (t2y * cs2 + t2x * sn2)) / 2;
			i += step;
		}
		return (sumx + sumy * I) * step;
	}
const static double base_step_size = 0.02;
class fourier;
static volatile double step_size = base_step_size;
static void (*drawfunc)(double, double*, double*);
static double complex coefficients[MAX_DEPTH * 2 + 10];
volatile int max_depth_computed = 0;
class fourier {
public:
	double complex eval(double x)
		{
			double complex sum = 0;
			for (int i = MAX_DEPTH - depth_goal; i <= MAX_DEPTH + depth_goal + 1; i = i + 1) {
				sum += coefficients[i]
				    * cpow(M_E, x * 2 * M_PI * I * (i - MAX_DEPTH + 1));
			}
			return sum;
		}
	static void set_coeff_at_depth(void (*func)(double, double*, double*), int i,
	    double step_size)
		{
			coefficients[MAX_DEPTH + 1 - i] = fourier_integrate(func, 0, 1, -i,
			    step_size);
			coefficients[MAX_DEPTH + 1 + i] = fourier_integrate(func, 0, 1, i,
			    step_size);
		}
	static void set_coeffs(void)
		{
			while (max_depth_computed < depth_goal && isInFourierDrawMode) {
				  set_coeff_at_depth(drawfunc, max_depth_computed, step_size);
				  max_depth_computed++;
				  if (max_depth_computed >= depth_goal) max_depth_computed = depth_goal;
			  }
			if (max_depth_computed >= depth_goal) max_depth_computed = depth_goal;
		}
};
