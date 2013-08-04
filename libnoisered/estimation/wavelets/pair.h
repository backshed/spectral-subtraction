#ifndef PAIR_H
#define PAIR_H
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
	Pair(int x, int y);
	unsigned int x, y; /**< TODO */
	/**
	 * @brief Debug : displays the value of the point on stdout.
	 *
	 */
	void display();
};

#endif // PAIR_H
