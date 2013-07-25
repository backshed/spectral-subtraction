#ifndef SARSA_H
#define SARSA_H

#include <algorithm>
#include "../spectral_subtractor.h"
#include "state.hpp"
#include "action.hpp"
// Dans la classe Sarsa, on va avoir un handler qui va prendre un état, une action, et calculer le reward, ainsi que l'état modifié.

#include <random>
#include <chrono>
class Sarsa
{
	public:
		Sarsa():
			dist(0, 1)
		{
			auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			rng.seed(time);
		}

		void exec()
		{


		}

		Action& epsilonGreedy()
		{

		}

		double reward(State& state, Action& action)
		{
			// Effectuer SS

			// Calculer NRR, SDR
			double nrr = 0;
			double sdr = 0;

			// Calculer reward
			return x ;
		}

	private:
		std::mt19937 rng;
		std::uniform_real_distribution<double> dist;

		double eps = 0.2;
		double x, y;
//		Q
//		gamma
//		alpha42
//		maxReward
};

#endif // SARSA_H
