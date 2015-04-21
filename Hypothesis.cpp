#include "Hypothesis.h"

Hypothesis::Hypothesis()
{}


Hypothesis::~Hypothesis()
{}

Hypothesis::Hypothesis(const std::string in_str)
{
	vector<string> tmp_strs;
	yutils::split_by_tag(in_str, tmp_strs);
	for(size_t i = 0; i < tmp_strs.size(); ++i){
		if(tmp_strs.size()-i > COMPS_NUM){
			_features.push_back(atof(tmp_strs[i].c_str()));
		}
		else{
			_comps.push_back(atoi(tmp_strs[i].c_str()));
		}
	}
}

float Hypothesis::compute_score(const vector<float> & weights)
{
	float score = 0.0;
	if(weights.size() != _features.size()){
		yutils::LOGE1("Hypothesis:: compute score, w and f size not match");
		return 0.0;
	}
	for(size_t i = 0; i < weights.size(); ++i){
		score += weights[i] * _features[i];
	}
	return score;
}


float Hypothesis::get_bleu(vector<int> & comps)
{
	float logbleu = 0.0;
	int n = (COMPS_NUM - 1) / 2;
	for(int i = 0;i < n; ++i){
		if(comps[2*i] == 0){
			return 0.0;
			//logbleu += 0;
		}
		else{
			logbleu += log((float)comps[2*i]) - log((float)comps[2*i+1]); // Pn = matched-ngram-count / total-count
		}
	}
	logbleu /= n;
	float brevity = 1.0- (float)comps[comps.size()-1] / comps[1]; // brevity = exp(1 - reference_length/hypothesis_length)
	if(brevity < 0.0){
		logbleu += brevity;
	}
	return exp(logbleu);
}
