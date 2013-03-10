#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <stack>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

const int MAX_CHARS_PER_LINE = 30;
const int MAX_LINES_IN_KB = 30;
const int MAX_TOKENS_PER_LINE = 20;
string rules[MAX_LINES_IN_KB];
string facts[MAX_LINES_IN_KB];
string fail="fail";
string success="yes";
int f,r,n,trace, flag=0;

stack<string> goalStack, whyStack;
string topOftheStack;

/*************************************************
* Parser to load rules and facts from input file
**************************************************/
int load(string filename)
{

  ifstream fin;
  fin.open(filename); 
  if (!fin.good()) 
    return 1; 

  //Initialize rules and facts
  for (int i=0; i<MAX_LINES_IN_KB; i++) 
  {
    rules[i] = {0};
    facts[i] = {0};
  }

  r=0;
  f=0;
  
  // read each line of the file
  while (!fin.eof())
  {

    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);
    
    string kb = buf;
    if (kb.find(">") != string::npos) 
    {
      rules[r] = kb;
      r++;
    }
    else
    {
      facts[f] = kb;
      f++;
    }
  }

  f--;

  return 0;

}

/****************************************************
* Print knowledge Base
****************************************************/
void show() 
{

  for (int i = 0; i < r; i++)
    cout << "Rule[" << i << "] = " << rules[i] << endl;

  for (int i = 0; i < f; i++)
    cout << "Fact[" << i << "] = " << facts[i] << endl;

  cout << endl; 
  return;
}

/******************************************************
* Clear all rules and facts
******************************************************/
void clear()
{

  for (int i = 0; i < r; i++)
    rules[i] = {0};

  for (int i = 0; i < f; i++)
    facts[i] = {0};

  while (!goalStack.empty())
    goalStack.pop();

  return;

}

/******************************************************
* Function to get the RHS of a rule
******************************************************/
string getRHS ( string str)
{

    string myToken[MAX_TOKENS_PER_LINE];
    n=0;

    char_separator<char> sep(">");
    tokenizer< char_separator<char> > my_tokens(str, sep);
    for (const auto& t : my_tokens) {
        myToken[n] = t;
        n++;
    }


  return myToken[1];

}

/*********************************************************
* Function to push Antecedants on to the goal stack
*********************************************************/
void pushAntecedants (string rule)
{

  string token[MAX_TOKENS_PER_LINE];
  string antecedants[MAX_TOKENS_PER_LINE];

  n=0;
  char_separator<char> my_sep(">");
    tokenizer< char_separator<char> > my_tokens(rule, my_sep);
    for (const auto& t : my_tokens) {
        token[n] = t;
        n++;
    }

  n=0;
  char_separator<char> sep("^");
    tokenizer< char_separator<char> > tokens(token[0], sep);
    for (const auto& t : tokens) {
        antecedants[n] = t;
        n++;
    }

  for (int i=0; i<n; i++)
  {
    goalStack.push(antecedants[i]);
  }

}

/**************************************************************
* The back-chaining algorithm
**************************************************************/
string back_chaining(string query)
{

  int i,j;
  int negation_flag = 0;
  string myString;

  while(!goalStack.empty())
  {

    // Print current goal stack if trace is set
    if (trace == 1)
    {
      stack<string> temp (goalStack);
      whyStack.push(temp.top());
      cout << "trace: stack [ ";
      while (!temp.empty()) {
        cout << temp.top() << " ";
        temp.pop();
      
      }

      cout << " ]" << "\n";
    }

    if (goalStack.top().at(0) == '-')
    {
      string tmp = goalStack.top().substr(1);
      goalStack.pop();
      stack<string> tempStack1 (goalStack);
      goalStack.push(tmp);
      myString = back_chaining(tmp);
      if (myString == "yes")
        return fail;
      
      if (goalStack.empty())
//     return fail;
        return success;

      negation_flag = 1;
    }

    // Check if the top of the goalStack is a fact; if yes pop it out
    for (i=0; i<f; i++)
    {
      if (facts[i] == goalStack.top())
      {
	goalStack.pop();
 	break;
      }
    }

    // If goalStack is empty, return
    if (goalStack.empty()) 
    {
      if (negation_flag == 0)
        return success;
      else
        return fail;
    }

    //if top of the stack is not a fact, then check if it is on the rhs of the rules. 
    //if it is not found, then return fail.
    if (i == f)
    {
      string myStr = goalStack.top();
      stack<string> tempStack (goalStack);
      for (j=0; j<r; j++)
      {
	if (getRHS(rules[j]) == myStr)
	{
	  goalStack.pop();
          pushAntecedants(rules[j]);
          myString = back_chaining(goalStack.top());
          if (myString == "fail") {
	    while(!goalStack.empty())
	      goalStack.pop();
            goalStack = tempStack;

            while(!tempStack.empty())
	      tempStack.pop();
	    continue;
          }
          else 
	    break;
	}
      }
    }

    if ( j == r ) // Goal is not on the rhs of any of the rules
    {	

        while (!goalStack.empty())
          goalStack.pop();
        return fail;
    }
    
  }

  return success;

}

/******************************************************************
* Main starts here
******************************************************************/
int main()
{

  //Interactive loop

  string inputVal, filename;

  while (true)
  {
    trace = 0;
    cout << "Enter: load <file>, show, clear, trace, quit or <query>\n";
    cin >> inputVal;

    if (inputVal == "load") {
      cout << "Enter the filename:\n";
      cin >> filename;
      load(filename);
    }

    else if (inputVal == "show") 
      show();

    else if (inputVal == "clear")
      clear();

    else if (inputVal == "trace")
    {
      trace = 1;
      cin >> inputVal;
      goalStack.push(inputVal);
      back_chaining(inputVal);
    }

    else if (inputVal == "quit")
      return 0;
   
    else if (inputVal == "why")
    {
      string val = back_chaining(inputVal);
      cout  << "proof: [ ";
      while(!whyStack.empty()) {
        cout << whyStack.top() << " ";
	whyStack.pop();
      }
      cout << "]\n";
    }

    else  
    {
      goalStack.push(inputVal);
      string val = back_chaining(inputVal);
      cout << val << "\n";
    }

  }

  return 0;

}
