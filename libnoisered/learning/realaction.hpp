#ifndef REALACTION_H
#define REALACTION_H

#include "action.hpp"
#include "realstate.hpp"

// It's about moving a parameter.
class RealAction: public Action
{
	protected:
		RealAction(State::Parameter parameter, bool direction);

		State::Parameter parameter;
		bool direction; // 0 for decrement, 1 for increment
};

#endif // REALACTION_H
