#ifndef _EVAL_H
#define _EVAL_H

/*******************************
 * This file is the header
 * for the different functions
 * used for evaluating quality
 * of noise-reduction algorithms.
 *
 * Written by Jean-Mickael Celerier
 * for the ASPL.
 *******************************/

// Computes noise-reduction rate
double NRR(double* original, double* reduced, int length);

// Computes speech distortion quantity
double SDR(double* original, double* reduced, int fft_size);

#endif
