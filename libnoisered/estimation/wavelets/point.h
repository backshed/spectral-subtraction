#pragma once

#include "pair.h"
/**
 * @brief Represents a point in a 2D space.
 *
 * The point can be valid or not. It also is real-valued.
 */
template<typename T>
struct Point : public Pair<T>
{
		/**
		 * @brief Basic constructor.
		 *
		 * Initializes a valid point at (0, 0).
		 */
		Point():
			Pair<T>(0, 0),
			_invalid(false)
		{ }

		virtual ~Point()
		{

		}

		/**
		 * @brief Constructor.
		 *
		 * Parameters are explicit.
		 *
		 * @param x
		 * @param y
		 * @param invalid
		 */
		Point(const T x, const T y, const bool invalid = false):
			Pair<T>(x, y),
			_invalid(invalid)
		{ }

		/**
		 * @brief Checks validity.
		 *
		 * @return bool True if point is invalid.
		 */
		bool notValid() const
		{
			return _invalid;
		}

		double val = 0; /**< TODO */

	private:
		bool _invalid = false; /**< TODO */

};

#pragma once
