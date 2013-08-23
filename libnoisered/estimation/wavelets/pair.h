#pragma once

// x for upper, y for lower
/**
 * @brief Represents a pair of ints.
 *
 *TODO replace with std::pair ?
 *
 */
struct Pair
{
	/**
	 * @brief Constructor.
	 *
	 * @param x
	 * @param y
	 */
	Pair(const unsigned int x, const unsigned int y);
	unsigned int x, y; /**< TODO */
	/**
	 * @brief Debug : displays the value of the point on stdout.
	 *
	 */
	void display();
};

#pragma once
