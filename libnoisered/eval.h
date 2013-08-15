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

//! Useful evaluation functions, like NRR and SDR.
namespace Eval
{
	/**
	* @brief Computes noise-reduction rate.
	*
	* Cf. paper on EL-SS.
	*
	* @param original Array that holds the noisy signal.
	* @param reduced Array that holds the subtracted signal.
	* @param length Length of the array.
	*
	* @return double NRR.
	*/
	double NRR(double *original, double *reduced, unsigned int length);

	// Computes speech distortion quantity
	/**
	* @brief Computes Speech distortion ratio.
	*
	* Cf. paper on EL-SS.
	*
	* @param original Array that holds the noiseless original signal.
	* @param reduced Array that holds the subtracted signal.
	* @param length Length of the array.
	*
	* @return double SDR.
	*/
	double SDR(double *original, double *reduced, unsigned int length);
}
#endif
