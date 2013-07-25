#ifndef ACTION_HPP
#define ACTION_HPP

#include "state.hpp"
class Action
{
	public:
		virtual void execute(State& s) = 0;
		virtual double reward() = 0;
};

#endif // ACTION_HPP
