#ifndef Y_HYPOTHESIS
#define Y_HYPOTHESIS

#define COMPS_NUM 9

#include "utils.h"


/*Y.H Hypothesis: store the features and n-gram matches of one hypothesis.
				  correspond to one line in the MERT chart.
*/
class Hypothesis
{
public:
	Hypothesis();
	//Y.H The in_str should be "F1 F2 F3 ... Fn C1 M1 C2 M2 .. Cn Mn Length"
	//    F for features, Cn for n-gram total, Mn for n-gram matches, and Length of the reference
	Hypothesis(const string in_str);
	~Hypothesis();

	//Y.H Calculate the score of this hypothesis given weights
	float compute_score(const vector<float> & weights);

	//Y.H static function, calculate bleu score
	static float get_bleu(vector<int> & comps);

	vector<float> _features;
	vector<int> _comps;

};


#endif