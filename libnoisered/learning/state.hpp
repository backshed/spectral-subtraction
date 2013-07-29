#ifndef STATE_HPP
#define STATE_HPP

/**
 * @brief
 *
 */
class State
{
	public:
		/**
		 * @brief
		 *
		 */
		enum Parameter { };
		/**
		 * @brief
		 *
		 * @param parameter
		 * @return double
		 */
		virtual double get(unsigned int parameter) = 0;
};

#endif // STATE_HPP
