#pragma once
#include <vector>
#include <list>
#include <string>
#include <random>
#include <chrono>

// Learning methods
#define QLEARNING 0
#define SARSA     1

// Policies
#define EGREEDY   2
#define SOFTMAX   3
/**
 * @brief (currently unfinished)
 */
class LearningManager
{
	typedef std::vector<double> Vector;
	typedef std::vector<Vector> Mat;
	typedef std::vector<int> State;

public:
	// constructor
	LearningManager(int numactions, std::vector<int> states);
	~LearningManager();

	void startLearn();
	void stopLearn();

	int nextStep();

	void reset();
	void newExperiment();


	/** Sets the n-th (= index-th) coordinate to the "state" value
	 **/
	void setState(int index, int state);
	void setReward(float r);

private:
	// if a stateAction is negligeable, sets it to 0 and returns true
	bool stateActionIsNegligeable(const std::vector<int>& sa);

	// Sets currentAction using the e-greedy or softmax policy
	void egreedyCurrentActionChooser(int currentStateIndex);
	void softmaxCurrentActionChooser(int currentStateIndex);

	// Updates Q and e using q-learning or sarsa method
	void qlearningUpdate(int currentStateIndex, int previousStateIndex);
	void sarsaUpdate(int currentStateIndex, int previousStateIndex);
	void updateEligibility(int previousStateIndex);

	// RNG
	std::mt19937 rng;
	std::uniform_real_distribution<double> dist;
	double randU01();

	int stateToInt(const State& from) const;
	void intToState(State& to, int from) const;

	/** paramters, learningRate might be called alpha in litterature **/
	double temperature, epsilon, lambda, alpha, gamma;

	/** Number of possible actions in each state **/
	unsigned int nActions;

	/** size of each dimension of states  **/
	std::vector<int> nStates;
	std::vector<bool> stateChanged;

	/** total dimensions of the state space (= nStates[0] * nStates[1] * ... * nStates[n-1]) **/
	unsigned int nStateDim;

	int currentAction;
	int previousAction;
	State previousState;
	State currentState;
	double currentReward;

	short policy;
	short learningMethod;

	bool isLearning;

	/** state-action values matrix.
	 *  each state has an integer index corresponding
	 *  to its index in this matrix.
	 **/
	Mat Q;

	/** Eligibility trace **/
	Mat e;

	/** list of states that have a non negigeable
	 *  eligibility value (in e).
	 **/
	std::list<std::vector<int> > worthItStateActions;

	/** temporary vector to stock softmax values **/
	Vector softmax;

	const int uninitialized = -1;
};
