#ifndef STANDARDSSACTION_H
#define STANDARDSSACTION_H

// Dans notre cas on en aura 4
// Une pour augmenter alpha, une pour diminuer alpha
// Une pour augmenter beta, une pour diminuer beta
#include "realaction.hpp"
#include "standardssstate.hpp"
/**
 * @brief
 *
 */
class StandardSSAction : public RealAction
{
	public:
		StandardSSAction(State::Parameter parameter, bool direction);

		/**
		 * @brief
		 *
		 * @param s
		 * @return StandardSSState
		 */
		virtual StandardSSState execute(StandardSSState &s)
		{
			StandardSSState s2(s);
			if (direction)
				s2.increase(parameter);
			else
				s2.decrease(parameter);

			return s2;
		}

};


#endif // STANDARDSSACTION_H
