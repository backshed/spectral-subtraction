#ifndef REALSTATE_HPP
#define REALSTATE_HPP

#include "state.hpp"
class RealState : public State
{
	public:
		virtual void increase(unsigned int parameter) = 0;
		virtual void decrease(unsigned int parameter) = 0;
};

#endif // REALSTATE_HPP
