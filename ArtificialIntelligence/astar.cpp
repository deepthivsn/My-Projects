#include <iostream>
#include <sstream>
#include <unordered_map>
#include <cstdlib>
#include <cstddef>
#include <vector>
#include <queue>
#include <stack>
#include <list>
#include <ctime>
#include <cstring>
using namespace std;

//To keep track of states in the closed set
typedef unordered_map<string,int> hash_map;
hash_map map;
hash_map::iterator It;

//To keep track of states to deduce the solution path
typedef unordered_map<string, string> hash_map2;
hash_map2 parent_map;
hash_map2::iterator It2;

int n=0, m=1, max_queue_size=0, max_depth=0;
int NUM1, NUM2;

template <typename T> 
T **AllocateDynamicArray( int nRows, int nCols)
{
      T **dynamicArray;

      dynamicArray = new T*[nRows];
      for( int i = 0 ; i < nRows ; i++ )
      dynamicArray[i] = new T [nCols];

      return dynamicArray;
}

template <typename T>
void FreeDynamicArray(T** dArray)
{
      delete [] *dArray;
      delete [] dArray;
}

string convertArraytoString(int** arr) {

	int num1=NUM1, num2=NUM2;

	std::stringstream s;

        for (int i=0; i<num1; i++) {
                for (int j=0; j<num2; j++) 
        	s << arr[i][j];
        }

        std::string myString = s.str();

	return myString;

}

char** convertStringtoArray(string s, int num1, int num2) {

	char** myArray = AllocateDynamicArray<char>(num1,num2);

	int k=0;
		
	for (int i=0; i<num1; i++) {
		for (int j=0; j<num2; j++) {
			myArray[i][j] = s.at(k);
			k++;
		}
	}

	return myArray;

}

void format(int** arr,int num1, int num2) {

        for (int i=0; i<num1; i++) {
                for (int j=0; j<num2; j++) {
                        if (arr[i][j] == 0)
                                break;
                        cout << arr[i][j] << "\t" ;
                }
                cout << "\n";
        }
}

void format(string str, int num1, int num2) {

	char** myArray = AllocateDynamicArray<char>(num1,num2);
	
        myArray = convertStringtoArray(str,num1,num2);

	for (int i=0; i<num1; i++) {
                for (int j=0; j<num2; j++) {
                        if (myArray[i][j] == '0')
                                break;
                        cout << myArray[i][j] << "\t" ;
                }
                cout << "\n";
	}
}

void format(char** arr, int num1, int num2) {
	
	for (int i=0; i<num1; i++) {
		cout << i+1 << " | ";
                for (int j=0; j<num2; j++) {
                        if (arr[i][j] == '0')
                                break;
                        cout << arr[i][j] << "\t" ;
                }
                cout << "\n";
        }
	cout << "\n";
}

int convertStringtoInteger(string str) {

        int Result;
        istringstream convert(str);

        if ( !(convert >> Result) )
                Result = 0;

        return Result;
}

int f_func(string str) {

 	int num1=NUM1, num2=NUM2;

	char** myArray = AllocateDynamicArray<char>(num1,num2);
        myArray = convertStringtoArray(str,num1,num2);

	int** blocks = AllocateDynamicArray<int>(num1,num2);

	for (int i=0; i<num1; i++)
        {
                for (int j=0; j<num2; j++) {
                        blocks[i][j]=myArray[i][j]-'0';

                }
        }

	int num=0, min;

	for (int i=0; i<num2-1; i++) {
		if (blocks[0][i] == 0)
		break;

		if (blocks[0][i] < blocks[0][i+1])
			num--;
		else
			num++;
	}

	for (int i=1; i<num1; i++) {
		for (int j=0; j<num2-1; j++) {
			if (blocks[i][j] == 0)
			break;

			if (blocks[i][j] > blocks[i][j+1])
				num--;
			else
				num++;
		}	
	}

	string s = convertArraytoString(blocks);

                It=map.find(s);
                int depth=It->second;
		
	int value1;

	for (int i=0; i<num2; i++)
        {
                if (blocks[0][i]==i+1)
                        continue;
                else
                {
                        min=i+1;
                        // Number of blocks out of place on stack 1 is value1
                        value1=num2-i;
                        break;
                }
        }


	int min_stack, min_block;

        for (int i=0; i<num1; i++)
        {
                for (int j=0; j<num2; j++)
                {
                        if (blocks[i][j] == min)
                        {
                                min_stack=i, min_block=j;
                                break;
                        }
                        else
                                continue;
                }
        }
	
	int* myArr;
        myArr = new int[num1];

        //Get the last block number on each of the stacks
	
        int* arr;
        arr = new int[num1];

	//Get the last block on each of the stacks
        for (int i=0; i<num1; i++)
        {
                for (int j=0; j<num2; j++)
                {
                        if (blocks[i][0] == 0)
                        {
                                myArr[i] = 0;
                                arr[i]=0;
                                break;
                        }
                        if (blocks[i][j] == 0) {
                                myArr[i] = blocks[i][j-1];
                                arr[i] = j;
                                break;
                        }
                        continue;
                }
        }

        // Value2 represents how deep the next minimum block is

        int value2;
        value2 = arr[0] - min + 1;
        if (min_stack !=0)
        {
                value2+=arr[min_stack]-min_block;
        }

	int h = num+min; //heuritic
	int g = depth;

return h+g;

}

struct CompareNode {
public:
bool operator() (string arr1, string arr2)
{

	if (f_func(arr1) <= f_func(arr2))
		return true;
	else
		return false;
}
};

priority_queue<string,vector<string>, CompareNode > state; 
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

void find_solution_path(string str, int num1, int num2) {

	char** myArray = AllocateDynamicArray<char>(num1,num2);
        while (str != " "){

                solution.push(str);
                It2=parent_map.find(str);
                str=It2->second;
        }

        while (!solution.empty()){
                str=solution.top();
		myArray = convertStringtoArray(str, num1, num2);
                format(myArray, num1, num2);
                solution.pop();
        }

}

int**  initial_state(int num1, int num2){

	int** blocks = AllocateDynamicArray<int>(num1,num2);
	int* myArray1;
	myArray1 = new int[num1*num2];
	int num=0;

	for (int i=0; i<num1; i++)
	{	
		for (int j=0; j<num2; j++)
		{

			if (i==0)
			{
	                	blocks[i][j]=num+1;
			}
			else
			{
				blocks[i][j]=0;
			}
			
			myArray1[num]= blocks[i][j];
			num++;
		}
			
	}

	int r, temp;

	for (int i = num-1; i > 0; i--) {
		r = rand() % i;
		temp = myArray1[i];
		myArray1[i] = myArray1[r];
		myArray1[r] = temp;
        }

	int tmp1;
		
	for (int i=0; i < num1; i++)
	{
		tmp1=0;
		for (int j=0; j<num2; j++)
		{
			if (myArray1[i*num2+j] > 0)
			{
				blocks[i][tmp1] = myArray1[i*num2+j];
				tmp1++;
			}
			
		}
		
		for (int k=tmp1; k<num2; k++)
		{
			blocks[i][k]=0;
		}
	}

delete[] myArray1;
return blocks;

}

string move(string parent, int num3, int num4, int num1, int num2) {

	char** myArray = AllocateDynamicArray<char>(num3,num4);

	myArray = convertStringtoArray(parent,num3,num4);

	int** arr = AllocateDynamicArray<int>(num3,num4);
	
	for (int i=0; i<num3; i++) {
		for (int j=0; j<num4; j++) {
			arr[i][j] = myArray[i][j]-'0';
		}
	}
	
        int* myArr;
	int* my_arr;
        myArr = new int[num3];
	my_arr = new int[num3];

	//Get the last block on each of the stacks
        for (int i=0; i<num3; i++)
        {
                for (int j=0; j<num4; j++)
                {
			if (arr[i][0] == 0)
			{
				myArr[i] = 0;
				my_arr[i]=0;
				break;
			}
                        if (arr[i][j] == 0) {
                                myArr[i] = arr[i][j-1];
				my_arr[i] = j;
                                break;
                        }
                        continue;
                }
        }

 	int i, j;

	// Return null if there is no number to be moved
	if (arr[num1][0] == 0)
                return " ";

	if (my_arr[num1] <= 0)
		return " ";

	int tmp;
	if (num1 == 0) {

		//check if num1 is valid otherwise return null
		for (i=0; i<my_arr[0]; i++) {
			if (arr[0][i] == i+1)
				continue;
			else
			{
				tmp=i;
				break;
			}
		}
	
// if ( myArr[0] == i+1)
			if (myArr[0] == tmp)
			return " ";

	}

	// Move elements
	if (arr[num2][0] == 0)
		arr[num2][0] = myArr[num1];
	else
		arr[num2][my_arr[num2]] = myArr[num1];

	arr[num1][(my_arr[num1])-1] = 0;

	string s;
	s=convertArraytoString(arr);

	delete[] myArr;
	delete[] my_arr;
		
	return s;

}

int isSolution(string str) {

	int num1=NUM1, num2=NUM2;
	char** myArray = AllocateDynamicArray<char>(num1,num2);

	myArray = convertStringtoArray(str, num1, num2);

	int** node = AllocateDynamicArray<int>(num1,num2);

	for (int i=0; i<num1; i++)
	{
		for (int j=0; j<num2; j++) {
			node[i][j]=myArray[i][j]-'0';

		}
	}

        for (int i=0; i<num1; i++)
        {      
                for (int j=0; j<num2; j++)
                {

                        if (i==0)
                        {
				if (node[i][j] != j+1)
				return -1;
                        }
                        else
                        {
				if (node[i][i] != 0)
				return -1;
                        }

                }

        }

return 0;

}

void generate_children (string parent, int num1, int num2, int depth) {

	if (parent == " ")
	return;

	string child;	

	for (int i=0; i<num1; i++)
        {
                for (int j=0; j<num1; j++) {

			if (i != j) {
				child=move(parent, num1, num2, i, j);

				if (child != " ") {

				add(child, depth+1, parent);

				int val=isSolution(child);

				if (val==0) {
					cout << "Success!";
                        		cout << "depth=" << max_depth << ", total goal tests=" << m << ", max queue size=" << max_queue_size << "\n";
                        		find_solution_path(child, num1, num2);

                        		exit(0);

				}
				}

			}
		}
	}

	return;

}


int main(string args[]) {

	srand(time(0));
	int num1, num2;

	cout << "How many stacks?\n";
	cin >> num1;

	cout << "How many blocks?\n";
	cin >> num2;

	NUM1=num1;
	NUM2=num2;
		
	int **my2dArr = initial_state(num1,num2);

	std::stringstream s;

	cout << "Initial state is:\n";
	for (int i=0; i<num1; i++) {
		for (int j=0; j<num2; j++) {
			cout << my2dArr[i][j] << "\t";
        s << my2dArr[i][j];
	}
		cout << "\n";
	}

	std::string header = s.str();

	int depth=0;
	string parent;

	string s1=convertArraytoString(my2dArr);
	string s2=" ";

	add(s1,depth," ");		// Add the Initial State

	while(!state.empty()){

		string tmp_str = state.top();
		parent = tmp_str;

        	It=map.find(tmp_str);
        	depth=It->second;
		cout << "Iter=" << m << ", queue=" << state.size() << ", f=g+h=" << f_func(tmp_str) << ", depth=" << depth << "\n";

        	if (max_queue_size < state.size())
        	max_queue_size=state.size();

        	if (max_depth < depth)
        	max_depth=depth;

		state.pop();
		
		generate_children (parent, num1, num2, depth);

		m++;
}		

	cout << "Solution doesn't exist";

	return 1;

}
