#include <math.h>

#include <stdio.h>
#include <complex.h>
#define complex _Complex
/**
 implement a couple of useful calculus operations;
 namely a numerical integration and numerical
 differential equation evaluator
 **/
double complex cintegrate(double complex (*f)(double), double xmin, double xmax,
    double step_size) {
	double complex sum = 0;
	for (double i = xmin; i <= xmax; i = i + step_size) {
		sum = sum + f(i);
	}
	return sum;
}
double integrate(double (*f)(double), double xmin, double xmax,
    double step_size) {
	double sum = 0;
	for (double i = xmin; i <= xmax; i = i + step_size) {
		sum = sum + f(i);
	}
	return sum;
}
class differential {
	/**
	 Superclass for differential equations. Really just sets out a
	 lose template
	 **/
public:
	int eval_steps(int step_ct, double step_size) {
		for (int i = 0; i < step_ct * 10; i = i + 1) {
			step(step_size / 10);
		}
	}
	;
public:
	virtual void step(double step_size) {

	}
};

