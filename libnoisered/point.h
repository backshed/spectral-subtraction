#ifndef POINT_H
#define POINT_H

#include "pair.h"
/**
 * @brief
 *
 */
struct Point : public Pair
{
	Point();
/**
 * @brief
 *
 * @param x
 * @param y
 * @param invalid
 */
	Point(int x, int y, int invalid = false);

	/**
	 * @brief
	 *
	 * @return bool
	 */
	bool notValid() const;
	double val = 0; /**< TODO */

	private:
	bool invalid; /**< TODO */

};

#endif // POINT_H
