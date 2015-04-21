#include "Candidate.h"

Candidate::Candidate()
{}

Candidate::~Candidate()
{
	for(size_t i = 0; i < this->_hyps.size(); ++i){
		free(_hyps[i]);
	}
}

Candidate::Candidate(const vector<string> & instr)
{
	for(size_t i = 0; i < instr.size(); ++i){
		Hypothesis * t = new Hypothesis(instr[i]);
		_hyps.push_back(t);
	}
}

Hypothesis* Candidate::Get1best(const vector<float> & weights)
{
	int best = -1;
	float score = 0.0;
	float tmp;
	for(int i = 0; i < (int)_hyps.size(); ++i){
		tmp = _hyps[i]->compute_score(weights);
		//Y.H score, the lower, the better, bleu
		if( tmp < score || best == -1){
			best = i;
			score = tmp;
		}
	}
	assert(best != -1);
	return _hyps[best];
}


/* Y.H For line1: y=lambda*x1+b1 and line2: y=lambda*x2 + b2. Here "lambda" is the variable.
                  so if line1 and line2 have intersection. The point should be lambda = (b1-b2) / (x2-x1)
*/
float Candidate::CalLambda(int line1, int line2, int index, const vector<float> & weights)
{
	float x1 = _hyps[line1]->_features[index];
	float x2 = _hyps[line2]->_features[index];
	float b1 = 0.0;
	float b2 = 0.0;
	for(int k = 0; k < (int)weights.size(); ++k){
		if(k != index){
			b1 += _hyps[line1]->_features[k] * weights[k];
			b2 += _hyps[line2]->_features[k] * weights[k];
		}
	}
	//cout << b1 << " " << b2 << ", " << x1 << " " << x2 << ", res:" << (b1-b2)/(x2-x1) << endl; 
	return (b1-b2) / (x2-x1);
}

bool Candidate::IsbestScore(float score, const vector<float> & weights)
{
	for(size_t i = 0; i < _hyps.size(); ++i){
		if(_hyps[i]->compute_score(weights) > score){
			return false;
		}
	}
	return true;
}

void Candidate::GetIntersections(int index, vector<float> & lambdas, const vector<float> & weights)
{
	/* Y.H First attempt: find all interactions, select those at its x, has the best y.
	assert(index < weights.size());
	for(int i = 0; i < (int)_hyps.size(); ++i){
		for(int j = i+1; j < (int)_hyps.size(); ++j){
			float lambda = CalLambda(i,j,index,weights); // find the intersection for two line
			vector<float> tmp_weights = const_cast<vector<float> &>(weights);
			tmp_weights[index] = lambda;
			float score = _hyps[i]->compute_score(tmp_weights);
			if(IsbestScore(score, tmp_weights)){ // if the score is the highest in all nbest-list, add.
				lambdas.push_back(lambda);
			}
		}
	}*/

	/* Y.H Second attemp: first find the one with the biggest intercept (l1), than select its leftmost intersection (l2).
	                      substitute the l1 with l2, repeat the process until there is no leftmost intersection. */
	float intercept_y = 0.0;
	int intercept = -1;
	for(int i = 0; i < (int)_hyps.size(); ++i){
		float tmp_intercept_y = _hyps[i]->compute_score(weights) - weights[index] * _hyps[i]->_features[index];
		if(intercept == -1 || intercept_y < tmp_intercept_y || 
		      (intercept_y == tmp_intercept_y && _hyps[intercept]->_features[index] > _hyps[i]->_features[index]) )
		{
			intercept_y = tmp_intercept_y; intercept = i;
		}
	}
	int leftmost;
	float leftmost_x = 0.0;
	float prev_x = 0.0;
	int prev = intercept;
	bool first = true;
	//cout << "intercept:" << intercept << "," << intercept_y << ","<< _hyps[intercept]->_features[index] << endl;
	while(1){
		leftmost = -1;
		for(int i = 0; i < (int)_hyps.size(); ++i){
			if(_hyps[i]->_features[index] <= _hyps[prev]->_features[index]) // slope less than intercept_x should be discarded.
				continue;
			float lambda = CalLambda(i, prev, index, weights);
			//cout << "lambda:" << lambda << ", leftmost:" << leftmost_x << ", index" << i << ", feature:" << _hyps[i]->_features[index] << endl; 
			if(lambda < leftmost_x || leftmost == -1){
				leftmost_x = lambda;
				leftmost = i;
			}
		}
		if(leftmost == -1)
			break;
		if(first || leftmost_x - prev_x > MIN_INTERVAL){
			lambdas.push_back(leftmost_x);
		}
		else{
			lambdas[lambdas.size()-1] = leftmost_x;
		}
		first = false;
		prev_x = leftmost_x;
		prev = leftmost;
	}
	
}

