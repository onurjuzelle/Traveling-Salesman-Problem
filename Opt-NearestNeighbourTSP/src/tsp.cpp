#include "header/tsp.h"

mtsp::mtsp()
{
	consistency_check = 0;
}

mtsp::~mtsp()
{
	axis_goals.clear();
	solutions.clear();
}

void mtsp::add_axisgoal(AxisGoal* ag)
{
	if (consistency_check == 0 || consistency_check == ag->num_cities()) {
		axis_goals.push_back(ag);
		consistency_check = ag->num_cities();
	}
	else {
		cout << "Incoherent numbers" << endl;
		exit(-1);
	}
}

void mtsp::shuffle_sol()
{

	random_device rd;
	mt19937 g(rd());
	std::shuffle(sol_current.begin(), sol_current.end(), g);
}

vector<double> mtsp::eval_axisgoals(SolOrder order)
{
	vector<double> ret(axis_goals.size());
	int i = 0;
	for (AxisGoal *ag : axis_goals) {
		ret[i++] = ag->eval_solution(order);
	}
    return ret;
}

void mtsp::two_opt(AxisGoal& ag)
{
	
	init();
	bool amelioration = true;
	while (amelioration){
		amelioration = false;
		for (int i = 1; i < ag.num_cities() && !amelioration; ++i){
			for (int j = i+1; j < ag.num_cities()-1 && !amelioration; ++j){
				if (ag.matrix_dist()[sol_current[i-1]][sol_current[j]] + ag.matrix_dist()[sol_current[i]][sol_current[j+1]]
					< ag.matrix_dist()[sol_current[i-1]][sol_current[i]] + ag.matrix_dist()[sol_current[j]][sol_current[j+1]]){
				    sol_current = two_swap(i, j);
				    amelioration = true;
				}
			}
    	}
	}
	Solution *new_sol = new Solution(sol_current, eval_axisgoals(sol_current));
	solutions.push_back(new_sol);
}

SolOrder mtsp::two_swap(int i, int k)
{
	SolOrder ret = sol_current;
	std::reverse(ret.begin()+i, ret.end()-(ret.size()-1-k));
	return ret;
}

bool mtsp::cmp_sol(Solution& sol)
{
	
	bool ret = false;
	vector<Solution*> sol_to_del;
	
	for (Solution *s : solutions) {
		bool keep_sol = true;
		bool keep_s = true;
		size_t same_value = 0;
		for (size_t i = 0; i < s->get_axis_eval().size(); ++i) {
			
			bool k_sol = (s->get_axis_eval()[i] > sol.get_axis_eval()[i]);
			keep_sol &= k_sol;
			bool k_s = (s->get_axis_eval()[i] < sol.get_axis_eval()[i]); 
			keep_s &= k_s;
			if (!k_sol && !k_s)
				same_value++;
		}
		if (same_value == sol.get_axis_eval().size()){
			ret = false;
			break;
		}
		if (!keep_sol && !keep_s){
			ret = true;
		}
		else if (keep_sol && !keep_s){
			ret = true;
			sol_to_del.push_back(s);
		}
		else if (!keep_sol && keep_s){
			ret = false;
			break;
		}
	}
	for (size_t i=0; i<sol_to_del.size(); i++){
		for (size_t j=0; j<solutions.size(); j++){
			if (sol_to_del[i] == solutions[j]){
				delete solutions[j];
				solutions.erase(solutions.begin()+j);
			}
		}
	}
	return ret;
}

void mtsp::find_solutions(int nsol)
{
	init();
	solutions.push_back(new Solution(sol_current, eval_axisgoals(sol_current)));
	bool amelioration = true;
	while (amelioration) {
		amelioration = false;
		if (nsol != -1 && nsol == (int) solutions.size()){
		
		}
		else{
			for (Solution *s : solutions) {
				sol_current = s->get_order();
				for (int i = 0; i < consistency_check - 1 && !amelioration; ++i) {
					for (int j = i + 1; j < consistency_check && !amelioration; ++j) {
						SolOrder sol_o = two_swap(i, j);
						vector<double> sol_evaluation = eval_axisgoals(sol_o);
						Solution *new_sol = new Solution(sol_o, sol_evaluation);
						if (cmp_sol(*new_sol)){
							amelioration = true;
							solutions.push_back(new_sol);
						}
						else
							delete new_sol;
					}
				}
				if (amelioration)
					break;
			}
		}
	}		
}

void mtsp::write_outfile(const char* filename) 
{
	ofstream out_file(filename);
	if (!out_file){
		cout << "Unable to write file \"" << filename << "\"." << endl;
	}
	else{
		for (Solution *s : solutions){
			for (int dist : s->get_axis_eval())
			
			
			out_file <<"The shortest path distance is: "<< dist << " ";
			out_file << endl;
			out_file << endl;
			out_file <<"The shortest path is: ";
			out_file << endl;
			out_file << endl;
			for (int id : s->get_order())
			out_file << id << "--";
			out_file << endl;
		}
		out_file.close();
	}
}

void mtsp::init()
{
	solutions.clear();
	sol_current.clear();

	sol_current.resize(consistency_check);
	std::iota(sol_current.begin(), sol_current.end(), 0);
	shuffle_sol();
}

double mtsp::getSolutionCost(int sol_idx)
{
	Solution *s = solutions[sol_idx];
	double cost = s->get_axis_eval().front();
	return cost;
}