#ifndef STATE_HPP
#define STATE_HPP

class State
{
	public:
		enum Parameter { };
		virtual double get(unsigned int parameter) = 0;
};

#endif // STATE_HPP
