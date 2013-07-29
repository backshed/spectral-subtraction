#ifndef REALSTATE_HPP
#define REALSTATE_HPP

#include "state.hpp"
/**
 * @brief
 *
 */
class RealState : public State
{
	public:
		/**
		 * @brief
		 *
		 * @param parameter
		 */
		virtual void increase(unsigned int parameter) = 0;
		/**
		 * @brief
		 *
		 * @param parameter
		 */
		virtual void decrease(unsigned int parameter) = 0;
};

#endif // REALSTATE_HPP
