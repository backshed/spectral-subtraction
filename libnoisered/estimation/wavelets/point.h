#pragma once

#include "pair.h"
/**
 * @brief Represents a point in a 2D space.
 *
 * The point can be valid or not. It also is real-valued.
 *
 */
struct Point : public Pair
{
		/**
		 * @brief Basic constructor.
		 *
		 * Initializes a valid point at (0, 0).
		 */
		Point();
		virtual ~Point();

		/**
		 * @brief Constructor.
		 *
		 * Parameters are explicit.
		 *
		 * @param x
		 * @param y
		 * @param invalid
		 */
		Point(const unsigned int _x, const unsigned int _y, const bool invalid = false);

		/**
		 * @brief Checks validity.
		 *
		 * @return bool True if point is invalid.
		 */
		bool notValid() const;

		double val = 0; /**< TODO */

	private:
		bool _invalid = false; /**< TODO */

};

#pragma once
