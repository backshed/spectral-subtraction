#ifndef ACTION_HPP
#define ACTION_HPP

#include "state.hpp"
/**
 * @brief
 *
 */
class Action
{
	public:
		/**
		 * @brief
		 *
		 * @param s
		 */
		virtual void execute(State& s) = 0;
		/**
		 * @brief
		 *
		 * @return double
		 */
		virtual double reward() = 0;
};

#endif // ACTION_HPP
