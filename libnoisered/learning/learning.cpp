#include "learning.hpp"

#include <ctime>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

// instantiate the class
LearningManager::LearningManager(int numactions, std::vector<int> states):
	dist(0, 1),
	currentAction(0),
	previousAction(0),
	currentReward(0),
	isLearning(false)
{
	nActions = numactions;
	nStateDim = 1;
	for (auto n : states)
	{
		nStates.push_back(n);
		currentState.push_back(uninitialized);
		previousState.push_back(uninitialized);
		nStateDim *= n;
	}

	// Initializing parameters with default values
	temperature = 1.0;
	epsilon = 0.1;
	lambda = 0.9;
	alpha = 0.1;
	gamma = 0.9;
	policy = SOFTMAX;
	learningMethod = SARSA;

	// Build Q and e matrix
	Q.resize(nStateDim);
	e.resize(nStateDim);

	for (auto i = 0U; i < nStateDim; ++i)
	{
		Q[i].resize(nActions);
		e[i].resize(nActions);
	}

	stateChanged.resize(nStateDim);
	softmax.resize(nActions);

	reset();

	// Initialize the random generator
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	rng.seed(time);
}

LearningManager::~LearningManager()
{
}


double LearningManager::randU01()
{
	return dist(rng); // random uniform [0,1]
}

int LearningManager::stateToInt(const LearningManager::State &from) const
{
	int index = 0;
	if (from[0] == uninitialized)
	{
		return uninitialized;
	}
	else
	{
		int factor = 1;
		for (int i = 0; i < (int)from.size(); ++i)
		{
			index += factor * from[i];
			factor *= nStates[i];
		}
	}
	return index;
}


void LearningManager::intToState(LearningManager::State &to, int from) const
{
	int factor = nStateDim;
	if (from == uninitialized)
	{
		for (auto i = 0U; i < to.size(); i++)
		{
			to[i] = uninitialized;
		}
	}
	else
	{
		for (int i = to.size() - 1; i >= 0; --i)
		{
			factor /= nStates[i] ;
			to[i] = from / factor ;
			from %= factor ;
		}
	}
}


void LearningManager::reset()
{
	for (auto i = 0U; i < nStateDim; ++i)
		for (auto j = 0U; j < nActions; ++j)
		{
			Q[i][j] = 1 / nActions;    //randU01();
		}

	newExperiment();
}


void LearningManager::newExperiment()
{
	for (auto i = 0U; i < nStateDim; ++i)
		for (auto j = 0U; j < nActions; ++j)
		{
			e[i][j] = 0.0;
		}

	for (int i = 0; i < (int)currentState.size(); ++i)
	{
		currentState[i] = uninitialized;
		previousState[i] = uninitialized;
		stateChanged[i] = false;
	}

	currentAction = previousAction = 0;
}


void LearningManager::startLearn()
{
	isLearning = true;
}

void LearningManager::stopLearn()
{
	isLearning = false;
}

int LearningManager::nextStep()
{
	int currentStateIndex = stateToInt(currentState);
	int previousStateIndex = stateToInt(previousState);

	if (currentStateIndex == uninitialized)
	{
		// Ask for the currentState
		return -1;
	}

	if (policy == EGREEDY)
	{
		egreedyCurrentActionChooser(currentStateIndex);
	}
	else if (policy == SOFTMAX)
	{
		softmaxCurrentActionChooser(currentStateIndex);
	}
	else // Unexpected situation
	{
		printf("WARNING, rl_discrete : Unexpected policy, using softmax instead.");
		softmaxCurrentActionChooser(currentStateIndex);
	}

	if (isLearning && previousStateIndex != uninitialized)
	{
		// Compute new weights
		if (learningMethod == QLEARNING)
		{
			qlearningUpdate(currentStateIndex, previousStateIndex);
		}
		else if (learningMethod == SARSA)
		{
			sarsaUpdate(currentStateIndex, previousStateIndex);
		}
		else // Unexpected situation
		{
			printf("WARNING, rl_discrete : Unexpected learining method, using qlearning instead.");
			qlearningUpdate(currentStateIndex, previousStateIndex);
		}
	}

	// Copy action
	previousAction = currentAction;
	for (auto i = 0U; i < previousState.size(); i++)
	{
		previousState[i] = currentState[i];    // copy old state
	}

	// Reset state change
	for (auto i = 0U; i < nStateDim; ++i)
	{
		stateChanged[i] = false;
	}

	// Send action
	return currentAction;
}


void LearningManager::egreedyCurrentActionChooser(int currentStateIndex)
{
	double rnd = randU01();

	if (rnd < epsilon)
	{
		currentAction = (int)(randU01() * nActions);
	}
	else
	{
		Vector &actionWeights = Q[currentStateIndex];
		currentAction = (int)(max_element(actionWeights.begin(), actionWeights.end()) - actionWeights.begin());
	}
}


void LearningManager::softmaxCurrentActionChooser(int currentStateIndex)
{
	// Choose next action using softmax policy
	double rnd = randU01();

	// compute exponentials
	Vector &actionWeights = Q[currentStateIndex];

	for (auto i = 0U; i < nActions; ++i)
	{
		softmax[i] = exp(actionWeights[i] / temperature);
	}

	// prevent overflow and compute sum
	double maxExp = *max_element(softmax.begin(), softmax.end());
	double sumExp = 0.0;
	for (auto i = 0U; i < nActions; ++i)
	{
		sumExp += softmax[i] / maxExp;
		softmax[i] = sumExp;
	}

	// Choose action.
	currentAction = (int)(lower_bound(softmax.begin(), softmax.end(), rnd * sumExp) - softmax.begin());
}


void LearningManager::qlearningUpdate(int currentStateIndex, int previousStateIndex)
{
	Vector &actionWeights = Q[currentStateIndex];
	int bestAction = (int)(max_element(actionWeights.begin(), actionWeights.end()) - actionWeights.begin());

	double delta = currentReward + gamma * Q[currentStateIndex][bestAction] - Q[previousStateIndex][previousAction];
	e[previousStateIndex][previousAction]++;

	updateEligibility(previousStateIndex);

	std::list<std::vector<int> >::iterator stateActionIter;
	for (stateActionIter = worthItStateActions.begin();
		 stateActionIter != worthItStateActions.end();
		 stateActionIter++)
	{
		Q[((*stateActionIter))[0]][(*stateActionIter)[1]] += alpha * delta * e[(*stateActionIter)[0]][(*stateActionIter)[1]];
		if (currentAction == bestAction)
		{
			e[(*stateActionIter)[0]][(*stateActionIter)[1]] *= gamma * lambda;
		}
		else
		{
			e[(*stateActionIter)[0]][(*stateActionIter)[1]] = 0;
		}
	}
}



void LearningManager::sarsaUpdate(int currentStateIndex, int previousStateIndex)
{
	double delta = currentReward + gamma * Q[currentStateIndex][currentAction] - Q[previousStateIndex][previousAction];
	e[previousStateIndex][previousAction]++;

	updateEligibility(previousStateIndex);

	std::list<std::vector<int> >::iterator stateActionIter;
	for (stateActionIter = worthItStateActions.begin();
		 stateActionIter != worthItStateActions.end();
		 stateActionIter++)
	{
		Q[(*stateActionIter)[0]][(*stateActionIter)[1]] += alpha * delta * e[(*stateActionIter)[0]][(*stateActionIter)[1]];
		e[(*stateActionIter)[0]][(*stateActionIter)[1]] *= gamma * lambda;
	}
}

void LearningManager::updateEligibility(int previousStateIndex)
{
	std::vector<int> previousStateAction;
	previousStateAction.push_back(previousStateIndex);
	previousStateAction.push_back(previousAction);

	bool stateIsFound = false;

	// Removes negligeable values
	std::list<std::vector<int> >::iterator stateActionIter = worthItStateActions.begin();
	while (stateActionIter != worthItStateActions.end())
	{
		if (*stateActionIter == previousStateAction)
		{
			stateIsFound = true;
		}

		if (stateActionIsNegligeable(*stateActionIter))
		{
			stateActionIter = worthItStateActions.erase(stateActionIter);
		}
		else
		{
			stateActionIter++;
		}
	}

	if (!stateIsFound)
	{
		worthItStateActions.push_back(previousStateAction);
	}
}

bool LearningManager::stateActionIsNegligeable(const std::vector<int> &sa)
{
	if (e[sa[0]][sa[1]] < 0.0001) // negligible
	{
		e[sa[0]][sa[1]] = 0;
		return true;
	}
	return false;
}

void LearningManager::setState(int index, int state)
{
	if (!stateChanged[index])
	{
		previousState[index] = currentState[index];    // copy old state
	}

	currentState[index] = state;
	stateChanged[index] = true;
}

void LearningManager::setReward(float r)
{
	currentReward = (double) r;
}
