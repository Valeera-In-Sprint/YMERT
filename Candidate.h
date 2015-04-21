#ifndef Y_CANDIDATE_H
#define Y_CANDIDATE_H

#include "Hypothesis.h"

#define MIN_INTERVAL 0.0001

/* Y.H Candidate stores the nbest list for one sentence.
*/

class Candidate
{
public:
	Candidate();
	~Candidate();

	/* Y.H the instr format please see Hypothesis.h
	*/
	Candidate(const vector<string> & instr);


	/*
	  Y.H Get 1-best hypothesis given weights
	*/
	Hypothesis * Get1best(const vector<float> & weights);

	/* Y.H Get the top intersection points in the MERT chart, "index" is the specific dimension we focus on.
	       "lambdas" is the return point-set, and "weights" is the given weights.
	*/
	void GetIntersections(int index, vector<float> & lambdas, const vector<float> & weights);

private:

	//Y.H Get the intersection of two lines
	float CalLambda(int line1, int line2, int index, const vector<float> & weights);

	//Y.H To see if "score" is the highest one given weights in the nbest (we only keep the top intersections in MERT).
	bool IsbestScore(float score, const vector<float> & weights);

	vector<Hypothesis *> _hyps;
};


#endif