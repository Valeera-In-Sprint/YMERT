#include "Tuner.h"

Tuner::Tuner()
{}

Tuner::~Tuner()
{
	for(size_t i = 0; i < _cands.size(); ++i)
		free(_cands[i]);
}


Tuner::Tuner(const string & filename)
{
	string line;
	ifstream infile(filename.c_str());
	vector<string> tmp_strs;
	int last_index = 0;
	while(getline(infile, line)){
		string::size_type pos = line.find(" ", 0);
		int tindex = yutils::to_int(line.substr(0, pos));
		if(tindex != last_index){
			Candidate * cand = new Candidate(tmp_strs);
			_cands.push_back(cand);
			tmp_strs.clear();tmp_strs.push_back(line.substr(pos+1, line.size()-pos-1));
			last_index = tindex;
		}
		else{
			tmp_strs.push_back(line.substr(pos+1, line.size()-pos-1));
		}
	}
	if(tmp_strs.size() > 0){
		Candidate * cand = new Candidate(tmp_strs);
		_cands.push_back(cand);
	}
}

float Tuner::GetBleu(const vector<float> &weights)
{
	vector<int> comps;
	comps.resize(COMPS_NUM, 0);
	//int n = (COMPS_NUM - 1) / 2; // the comps are stored as "C1 M1 C2 M2 .. Cn Mn Length"
	Hypothesis * y = NULL;
	for(size_t i = 0; i < _cands.size(); ++i){
		y = _cands[i]->Get1best(weights); // the total bleu score only concerns the 1-best hypothesis
		for(int j = 0; j < COMPS_NUM; ++j){
			comps[j] += y->_comps[j];
			//comps[2*j] += y->_comps[2*j];
			//comps[2*j+1] += y->_comps[2*j+1];
		}
	}
	/*cout << "comps:";
	for(size_t i = 0; i < comps.size(); ++i){
		cout << comps[i] << " ";
	}
	cout << endl;*/
	return Hypothesis::get_bleu(comps);
}

void Tuner::MergeSortIntersections(const vector<float> & lambdas, vector<float> & res)
{
	map<float, int> tmp_map;
	for(size_t i = 0; i < lambdas.size(); ++i){
		tmp_map.insert(pair<float, int>(lambdas[i], 1));
	}
	for(map<float, int>::iterator iter = tmp_map.begin(); iter != tmp_map.end(); ++iter){
		res.push_back(iter->first);
	}
	return;
}

float Tuner::FindLambda(const vector<float> & intersections, int index, vector<float> & weights, float & best_bleu)
{
	int best_index = -1;
	for(size_t i = 0; i < intersections.size(); ++i){
		vector<float> tmp_weights = weights; // the weights should be changed according to each intersection
		tmp_weights[index] = intersections[i];
		float tmp_bleu = GetBleu(tmp_weights);
		if(tmp_bleu > best_bleu || best_index == -1){
			best_bleu = tmp_bleu; best_index = i; // find the best bleu
		}
	}
	if(intersections.size() == 1){
		yutils::LOG1("only one intersection, error");
		return 0.0;
	}
	if(best_index != (int)intersections.size()-1){
		return (intersections[best_index] + intersections[best_index+1]) /2 ;
	}
	else{
		yutils::LOG1("weird line style"); // the last point in the intersection yields the highest bleu, not quite sure.
		return (intersections[best_index] + intersections[best_index-1]) /2 ;
	}
}

float Tuner::Iteration(vector<float> & weights)
{
	vector<float> lambdas;
	float best_bleu = 0.0;
	for(int i = 0; i < _weightnum; ++i){
		lambdas.clear();
		for(int j = 0; j < (int)_cands.size(); ++j){
			_cands[j]->GetIntersections(i, lambdas, weights);
			//cout << "dimension:" << i << ", "<< j << "th candidate =======" << endl;
		}
		vector<float> res;
		MergeSortIntersections(lambdas, res);
		/*for(size_t k = 0; k < res.size(); ++k){
			cout << res[k] << endl;
		}*/
		weights[i] = FindLambda(res, i, weights, best_bleu);
		cout << "Best bleu:" << best_bleu << ", weights:" ;
		for(size_t k = 0; k < weights.size(); ++k){
			cout << weights[k] << " ";
		}
		cout << endl;
	}
	return best_bleu;
}

void Tuner::ReadWeights(const string & weights_file, vector<float> & weights, 
		                vector<float> & upbounds, vector<float> & lowerbounds)
{
	ifstream infile(weights_file.c_str());
	string line;
	getline(infile, line);
	vector<string> tmp_strs;
	yutils::split_by_tag(line, tmp_strs);
	for(size_t i = 0; i < tmp_strs.size(); ++i){
		weights.push_back(atof(tmp_strs[i].c_str()));
	}
	getline(infile, line);
	yutils::split_by_tag(line, tmp_strs);
	for(size_t i = 0; i < tmp_strs.size(); ++i){
		upbounds.push_back(atof(tmp_strs[i].c_str()));
	}
	getline(infile, line);
	yutils::split_by_tag(line, tmp_strs);
	for(size_t i = 0; i < tmp_strs.size(); ++i){
		lowerbounds.push_back(atof(tmp_strs[i].c_str()));
	}
	return;
}

void Tuner::RandomStart(vector<float> & weights, const vector<float> & upbounds, const vector<float> & lowerbounds)
{
	assert(upbounds.size() == lowerbounds.size());
	weights.resize(upbounds.size(), 0.0);
	for(size_t i =0; i < upbounds.size(); ++i){
		weights[i] = lowerbounds[i] + (float)random()/RAND_MAX * (upbounds[i]-lowerbounds[i]);
	}
	return;
}

void Tuner::Run(const string & weights_file)
{
	vector<float> init_weights;
	vector<float> upbounds;
	vector<float> lowerbounds;
	ReadWeights(weights_file, init_weights, upbounds, lowerbounds);
	this->_weightnum = (int)init_weights.size(); //Y.H The weight number is set here

	float best_bleu = 0.0; vector<float> best_weights;
	vector<float> weights;
	for(int i = 0;i < MAX_POINTS; ++i){
		if(i == 0)
			weights = init_weights;
		else
			RandomStart(weights, upbounds, lowerbounds);
		float last_bleu = -1.0;
		float cur_bleu = 0.0;
		int round = 0;
		while(round < MAX_ITERATION && cur_bleu - last_bleu > EPCILON){
			last_bleu = cur_bleu;
			cur_bleu = Iteration(weights); // Y.H So the init_weights is always changing here
			cout << "Iteration " << round << ", BLEU=" << cur_bleu << endl;
			if(cur_bleu > best_bleu){
				best_weights = weights;
				best_bleu = cur_bleu;
			}
			++round;
		}
	}
	//Y.H add normalization
	float norm = 0.0;
	for(size_t i = 0; i < best_weights.size(); ++i){
		norm += fabs(best_weights[i]);
	}
	ofstream outfile("lambdas.out");
	//outfile << "bleu:" << best_bleu << endl;
	for(size_t i = 0; i < best_weights.size(); ++i){
		outfile << best_weights[i]/norm << " ";
	}
	outfile << endl;
	outfile.close();
	return;
}
