#pragma once

// x for upper, y for lower
/**
 * @brief Represents a pair of ints.
 *
 *TODO replace with std::pair ?
 *
 */
template<typename T>
struct Pair
{
	public:
		/**
		* @brief Constructor.
		*
		* @param x
		* @param y
		*/
		Pair(const T x, const T y):
			_x(x), _y(y)
		{
		}

		virtual ~Pair()
		{

		}


		T _x, _y; /**< TODO */

};

#pragma once
