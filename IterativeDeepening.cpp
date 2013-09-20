#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include <stack>
#include <list>
#include <ctime>
#include <cstring>
using namespace std;

typedef unordered_map<string,int> hash_map;
hash_map map;
hash_map::iterator It;

typedef unordered_map<string, string> hash_map2;
hash_map2 parent_map;
hash_map2::iterator It2;

int n=0, m=1, max_queue_size=0, max_depth=0;

stack<string> state;
stack<string> solution;

//Add method to add the new string to the Map and Queue
void add(string str, int num, string parent){

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

void up(string str, int num, string parent){

	int a = str.find("0");

	if(a>2){

		string s = str.substr(0,a-3) + "0" + str.substr(a-2,2) + str.at(a-3) + str.substr(a+1);
			
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

void down(string str,int num, string parent){

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

int DLS(string s, int depth) {

int num;
It=map.find(s);
num=It->second;

if (state.empty())
        return -1;

        if (num == depth)
        {
                if (state.size() > 0)	
                {
                state.pop();
		if (state.empty())
	        return -1;
                return DLS(state.top(), depth);
                }
                else
                return -1;
        }

        string parent;
        parent=s;

        cout << "Iterator=" << m << ", queue=" << state.size() << ", depth=" << num << "\n";

        if (max_queue_size < state.size())
        max_queue_size=state.size();

        if (max_depth < depth)
        max_depth=depth;

        state.pop();
        up(s,num,parent);    // Move the blank pace up and add new state to queue
        down(s,num,parent);  // Move the blank spac down
        left(s,num,parent);  // Move left
        right(s,num,parent); // Move right and remove the current node from queue
        m++;

        return DLS(state.top(),depth);

}

void iddfs(string str)
{
        int depth = 0, result;

	while (1) {

	add(str,0, "");
	result = DLS(str, depth++);
cout << "DEBUG result is: " << result;
		map.erase(map.begin(), map.end());
		parent_map.erase(parent_map.begin(),parent_map.end());
//		add(str,0, "");
 //               depth++;
	}
                return;
}


int main(string args[]) {

srand(time(0));

cout << "Enter value for shuffling:";
int tmp;
cin >> tmp;
string str=shuffler("123085476",tmp);

cout << "you string is" << str;

add(str,0, "");					// Add the Initial State

iddfs(str);

cout << "Solution doesn't exist";

return 1;

}
