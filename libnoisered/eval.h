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
/**
 * @brief
 *
 * @param original
 * @param reduced
 * @param length
 * @return double
 */
double NRR(double* original, double* reduced, unsigned int length);

// Computes speech distortion quantity
/**
 * @brief
 *
 * @param original
 * @param reduced
 * @param fft_size
 * @return double
 */
double SDR(double* original, double* reduced, unsigned int fft_size);

#endif
