#include <iostream>
#include <ctime>
#include <unordered_map>
#include <cstdlib>
#include <queue>
#include <stack>
#include <list>
#include <cstring>
using namespace std;

typedef unordered_map<string,int> hash_map;
hash_map map;
hash_map::iterator It;

typedef unordered_map<string, string> hash_map2;
hash_map2 parent_map;
hash_map2::iterator It2;

int n=0, m=1, max_queue_size=0, max_depth=0;

queue<string> state;
stack<string> solution;

//Add method to add the new string to the Map and Queue
void add(string str, int num,string parent){

	if(map.find(str) == map.end()){
		map.insert(hash_map::value_type(str, num));
		parent_map.insert(hash_map2::value_type(str,parent));
		n++;
		state.push(str);
	}
}

void format(string str) {

cout << str.at(0) << "\t" << str.at(1) << "\t" << str.at(2) << "\n";
cout << str.at(3) << "\t" << str.at(4) << "\t" << str.at(5) << "\n";
cout << str.at(6) << "\t" << str.at(7) << "\t" << str.at(8) << "\n\n";

}

void find_solution_path(string str) {

        while (str != ""){

		solution.push(str);
                It2=parent_map.find(str);
                str=It2->second;
        }
		
	while (!solution.empty()){
		str=solution.top();
		format(str);
		solution.pop();
	}
}

string shuffler(string str, int num){

int move;
int a;

if (num == 0)
return str;

string s=str;
while (num!=0)
{

a=s.find("0");
move = rand() % 4;

if (move == 0)
{
        if (a>2)
        s=str.substr(0,a-3) + "0" + str.substr(a-2,2) + str.at(a-3) + str.substr(a+1);
}

if (move == 1)
{
        if(a<6)
        s = str.substr(0,a) + str.substr(a+3,1) + str.substr(a+1,2) + "0" + str.substr(a+4);
}

if (move == 2)
{
        if(a<6)
        s = str.substr(0,a) + str.substr(a+3,1) + str.substr(a+1,2) + "0" + str.substr(a+4);
}
if (move == 3)
{

        if(a!=2 && a!=5 && a!=8)
        s = str.substr(0,a) + str.at(a+1) + "0" + str.substr(a+2);
}

return shuffler(s,num-1);
}

}

void up(string str, int num, string parent) {

	int a = str.find("0");

	if(a>2){

		string s = str.substr(0,a-3) + "0" + str.substr(a-2,2) + str.at(a-3) + str.substr(a+1);
			
		s = s.substr(0,9);

		add(s,num+1,parent);

		if(s.compare("123456780") == 0) {

			cout << "Success!";
			cout << "depth=" << max_depth << ", total goal tests=" << m << ", max queue size=" << max_queue_size << "\n";
			find_solution_path(s);

			exit(0);

		}

	}

return;

}

void down(string str, int num, string parent){

	int a = str.find("0");

	if(a<6){
		string s = str.substr(0,a) + str.substr(a+3,1) + str.substr(a+1,2) + "0" + str.substr(a+4);

		s = s.substr(0,9);

		add(s,num+1,parent);

		if(s.compare("123456780") == 0) {
			cout << "Success!";
			cout << "depth=" << max_depth << ", total goal tests=" << m << ", max queue size=" << max_queue_size << "\n";
			find_solution_path(s);

			exit(0);

		}

	}

return;

}

void left(string str, int num, string parent){

	int a = str.find("0");

	if(a!=0 && a!=3 && a!=6){

		string s = str.substr(0,a-1) + "0" + str.at(a-1) + str.substr(a+1);

		s = s.substr(0,9);

		add(s,num+1,parent);

		if(s.compare("123456780") == 0) {

		cout << "Success!";
		cout << "depth=" << max_depth << ", total goal tests=" << m << ", max queue size=" << max_queue_size << "\n";

			find_solution_path(s);

			exit(0);

		}

	}

return;

}

void right(string str,int num, string parent){

	int a = str.find("0");
	string s;

	if(a!=2 && a!=5 && a!=8){

		s = str.substr(0,a) + str.at(a+1) + "0" + str.substr(a+2);

		s = s.substr(0,9);

		add(s,num+1, parent);

		if(s.compare("123456780") == 0) {

			cout << "Success!";
			cout << "depth=" << max_depth << ", total goal tests=" << m << ", max queue size=" << max_queue_size << "\n";

			find_solution_path(s);

			exit(0);

		}

	}

return;

}

int main(string args[]) {

srand(time(0));

cout << "Enter value for shuffling:";

int val;
cin >> val;
string str=shuffler("123085476",val);

int depth=0;

add(str,depth,"");					// Add the Initial State

string parent;

while(!state.empty()){

	string tmp_str = state.front();

	parent = tmp_str;

        It=map.find(tmp_str);

        depth=It->second;
	
	cout << "Iterator=" << m << ", queue=" << state.size() << ", depth=" << depth << "\n";

        if (max_queue_size < state.size())
        max_queue_size=state.size();

        if (max_depth < depth)
        max_depth=depth;

	up(tmp_str,depth,parent);	// Move the blank pace up and add new state to queue		

	down(tmp_str,depth,parent);	// Move the blank spac down 

	left(tmp_str,depth,parent);	// Move left

	right(tmp_str,depth,parent);	// Move right and remove the current node from queue

	state.pop();
	m++;
}		

cout << "Solution doesn't exist";

return 1;

}
