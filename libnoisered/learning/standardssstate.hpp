#ifndef STANDARDSSSTATE_HPP
#define STANDARDSSSTATE_HPP

#include "realstate.hpp"
#include <parallel/algorithm>

/**
 * @brief
 *
 */
class StandardSSState : public RealState
{
	public:
		/**
		 * @brief
		 *
		 */
		enum Parameter { Alpha, Beta };

		/**
		 * @brief
		 *
		 * @param parameter
		 * @return double
		 */
		virtual double get(unsigned int parameter) final override
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

		/**
		 * @brief
		 *
		 * @param parameter
		 */
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

		/**
		 * @brief
		 *
		 * @param parameter
		 */
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
		double alpha; /**< TODO */
		double beta; /**< TODO */

		double alpha_increment = 0.1; /**< TODO */
		double beta_increment = 0.01; /**< TODO */

		double alpha_min = 0.1; /**< TODO */
		double alpha_max = 100; /**< TODO */

		double beta_min = 0.01; /**< TODO */
		double beta_max = 1; /**< TODO */
};
#endif // STANDARDSSSTATE_HPP
