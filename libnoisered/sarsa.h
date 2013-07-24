#ifndef SARSA_H
#define SARSA_H

#include <algorithm>
class State
{
	public:
		enum Parameter { };
		virtual auto get(unsigned int parameter);
};

class RealState : public State
{
	public:
		virtual void increase(unsigned int parameter);
		virtual void decrease(unsigned int parameter);
};

class StandardSSState : public RealState
{
	public:
		enum Parameter { Alpha, Beta };

		virtual auto get(unsigned int parameter) final override
		{
			switch(parameter)
			{
				case Alpha:
					return alpha;
				case Beta:
					return beta;
				default:
					throw; //InvalidParameterException;
			}

		}

		virtual void increase(unsigned int parameter) final override
		{
			switch(parameter)
			{
				case Alpha:
					alpha = std::min(alpha + alpha_increment, alpha_max);
				case Beta:
					beta = std::min(beta + beta_increment, beta_max);
				default:
					throw;
			}
		}

		virtual void decrease(unsigned int parameter) final override
		{
			switch(parameter)
			{
				case Alpha:
					alpha = std::max(alpha - alpha_increment, alpha_min);
				case Beta:
					beta = std::max(beta - beta_increment, beta_min);
				default:
					throw;
			}
		}

	private:
		double alpha;
		double beta;

		double alpha_increment = 0.1;
		double beta_increment = 0.01;

		double alpha_min = 0.1;
		double alpha_max = 100;

		double beta_min = 0.01;
		double beta_max = 1;
};

class Action
{
	public:
		virtual void execute(State& s) = 0;
		virtual double reward() = 0;
};

// It's about moving a parameter.
class RealAction
{
	public:
		RealAction(State::Parameter parameter, bool direction) :
			Action(),
			parameter(parameter),
			direction(direction)
		{

		}

		virtual RealState& execute(RealState& s)
		{
			if(direction)
				s.increase(parameter);
			else
				s.decrease(parameter);

			return s;
		}

	private:
		State::Parameter parameter;
		bool direction; // 0 for decrement, 1 for increment
};

// Dans notre cas on en aura 4
// Une pour augmenter alpha, une pour diminuer alpha
// Une pour augmenter beta, une pour diminuer beta
class StandardSSAction : public RealAction
{
	public:
		StandardSSAction(State::Parameter parameter, bool direction) :
			RealAction(parameter, direction)
		{

		}

		virtual double reward()
		{
			// Effectuer SS

			// Calculer NRR, SDR
			double nrr = 0;
			double sdr = 0;

			// Calculer reward
			return x ;
		}

	private:
		double x, y;
};

// Dans la classe Sarsa, on va avoir un handler qui va prendre un état, une action, et calculer le reward.


class Sarsa
{
	public:
		Sarsa();
		void exec()
		{


		}

	private:
		Q
		gamma
		alpha
		maxReward
};

#endif // SARSA_H
