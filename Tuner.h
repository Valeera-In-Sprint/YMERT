#ifndef Y_TUNER_H
#define Y_TUNER_H

#include "Candidate.h"

#define EPCILON 0.0001 //Y.H The minimum difference of the bleu score between two iteration
#define MAX_ITERATION 10 //Y.H maximum iteration for one round in line search
#define MAX_POINTS 5 //Y.H maximum number of initial points for line search


/*Y.H Tuner is the main body for MERT.
*/
class Tuner
{
public:
	Tuner();
	~Tuner();
	//Y.H filename is the feature and comps file of the nbest list
	Tuner(const string & filename);

	//Y.H Main function
	void Run(const string & weights_file);

	//Y.H one interation in tuning
	float Iteration(vector<float> & weights);

	/*Y.H After getting all the lambdas from every sentences in the nbest list, we sort and merge them all.
	*/
	void MergeSortIntersections(const vector<float> & lambdas, vector<float> & res);
	/*Y.H Since the bleu score only changes at each intersections, we calculate bleu score at each intersection and find the highest one.
		  Then we set lambda at the middle interval of this intersection.
		  The function has two return values: one is the lambda, the other is the "best_bleu" score.
	*/
	float FindLambda(const vector<float> & intersections, int index, vector<float> & weights, float & best_bleu);

	//Y.H Calculate the bleu score at the whole dev-set level
	float GetBleu(const vector<float> & weights);

private:

	void RandomStart(vector<float> & weights, const vector<float> & upbounds, const vector<float> & lowerbounds);
	void ReadWeights(const string & weights_file, vector<float> & weights, 
		                vector<float> & upbounds, vector<float> & lowerbounds);
	vector<Candidate *> _cands;
	int _weightnum;
};


#endif
