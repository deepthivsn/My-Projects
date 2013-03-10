#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <stack>
#include <boost/tokenizer.hpp>
#include <algorithm>

using namespace std;
using namespace boost;

const int MAX_CHARS_PER_LINE = 500;
const int MAX_LINES_IN_KB = 5000;
const int MAX_TOKENS_PER_LINE = 100;
const int MAX_DOMAINS = 50;
const int MAX_ELEMENTS_IN_DOMAIN = 50;
string rules[MAX_LINES_IN_KB];
string facts[MAX_LINES_IN_KB];
string domain[MAX_DOMAINS][MAX_ELEMENTS_IN_DOMAIN];
int domainValues[MAX_DOMAINS];
string newRules[MAX_LINES_IN_KB];
string fail="fail";
string success="yes";
int f,r,d,n,trace,flag=0;

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
    rules[i] = "";
    facts[i] = "";
  }

  for (int i=0; i<MAX_DOMAINS; i++)
  {
    for (int j=0; j<MAX_ELEMENTS_IN_DOMAIN; j++)
      domain[i][j] = "";
  }

  r=0;
  f=0;
  d=0;
  
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
    else if (kb.find("domain") != string::npos)
    {
      string myDomain[MAX_TOKENS_PER_LINE];
      n=0;

      char_separator<char> sep(" ");
      tokenizer< char_separator<char> > my_domains(kb, sep);
      for (const auto& t : my_domains) {
        myDomain[n] = t;
        n++;
      }

      domain[d][0] = myDomain[1];
      domainValues[d] = n-2;
      for (int i=1; i<n-1; i++)
      {
	domain[d][i] = myDomain[i+1];
      }
      d++; 

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
    rules[i] = "";

  for (int i = 0; i < f; i++)
    facts[i] = "";

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

      for (int i=0; i<f; i++)
      {
        if (facts[i] == tmp)
        {
          return fail;
        }
      }
      
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
          if (!goalStack.empty())
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

void propositionalize(string rules[]) 
{

  if (domain == NULL) 
    return;

  int myIndex=0;

  // Convert all facts into propositional symbols
  for (int i=0; i<f; i++)
  {
    replace(facts[i].begin(), facts[i].end(), '(', '_');
    replace(facts[i].begin(), facts[i].end(), ',', '_');

    int val;
    while ( (val = facts[i].find(")")) != string::npos )
      facts[i].erase(val, 1);
  }

  // Convert all rules into propositional symbols
  for (int i=0; i<r; i++)
  {
    string myRule[MAX_TOKENS_PER_LINE];
    string symbols[MAX_DOMAINS];
    n=0;

    char_separator<char> sep(" ");
    tokenizer< char_separator<char> > my_rules(rules[i], sep);
    for (const auto& t : my_rules) {
      myRule[n] = t;
      n++;
    }
   
    int m;

    if (myRule[0] == "forall")
    {
      for(m=0; m<n-1; m++)
      {
	  if (myRule[m+1] == ":")
	    break;
          symbols[m] = myRule[m+1];
      }
    }

    int number_of_symbols = m;

    string mySymbols[m];
    int myNum=0;
    for (int i=0; i<=number_of_symbols; i++)
    {
      int k;
      for (k=0; k<d; k++)
      {
        if (symbols[i] == domain[k][0]) {
          mySymbols[myNum]=symbols[i];
          myNum++;
        }
      }
    }
    myNum--;

    string tempRule;
      for (int j=number_of_symbols+2; j<n; j++) //Excluding forall and : (hence 2)
        tempRule += myRule[j];

    replace(tempRule.begin(), tempRule.end(), '(', '_');
    replace(tempRule.begin(), tempRule.end(), ',', '_');

    int val;
    while ( (val = tempRule.find(")")) != string::npos )
      tempRule.erase(val, 1);

    stack<string> tempRuleStack;
    tempRuleStack.push(tempRule);

    while (!tempRuleStack.empty())
    {
      string tempRule2 = tempRuleStack.top();
      tempRuleStack.pop();

      string myRule[MAX_TOKENS_PER_LINE];
      n=0;

      char_separator<char> sep("_", ">");
      tokenizer< char_separator<char> > my_rules(tempRule2, sep);
      for (const auto& t : my_rules) {
        myRule[n] = t;
        n++;
      }
      
      int i,j,l;

      for (i=0; i<d; i++) {
        for (j=0; j<n-1; j++) {
          if (myRule[j] == domain[i][0]) {

            for(l=1; l < domainValues[i]; l++)
            {
              string tempRule3 = tempRule2;
              int val;
              while ( (val = tempRule3.find(domain[i][0])) != string::npos ) {
                tempRule3.replace(tempRule3.find(domain[i][0]), domain[i][0].size(), domain[i][l]);
              }
              tempRuleStack.push(tempRule3);
            }
              while ( (val = tempRule2.find(domain[i][0])) != string::npos ) {
                tempRule2.replace(tempRule2.find(domain[i][0]), domain[i][0].size(), domain[i][l]);
              }
              tempRuleStack.push(tempRule2);

            break;

          }
        }
      }

      if (i==d && j==n-1) {
        newRules[myIndex] = tempRule2;
        myIndex++;
      } 

    }

  while (!tempRuleStack.empty())
  tempRuleStack.pop();

  }

  for (int i=0; i<MAX_LINES_IN_KB; i++)
  {
    rules[i] = "";
  }

  for (int i=0; i<myIndex; i++)
  {
    rules[i] = newRules[i];
  }
    
  r=myIndex;

  for (int i=0; i<r; i++)
  {
    replace(rules[i].begin(), rules[i].end(), '(', '_');
    replace(rules[i].begin(), rules[i].end(), ',', '_');

    int val;
    while ( (val = rules[i].find(")")) != string::npos )
      rules[i].erase(val, 1);

      string myRule[MAX_TOKENS_PER_LINE];
      n=0;

      char_separator<char> sep("_", ">");
      tokenizer< char_separator<char> > my_rules(rules[i], sep);
      for (const auto& t : my_rules) {
        myRule[n] = t;
        n++;
      }

      int k,j,l;

      for (k=0; k<d; k++) {
        for (j=0; j<n; j++) {
          if (myRule[j] == domain[k][0]) {
          for(l=1; l < domainValues[k]; l++)
            {
              string tempRule3 = rules[i];
              int val;
              while ( (val = tempRule3.find(domain[k][0])) != string::npos ) {
                tempRule3.replace(tempRule3.find(domain[k][0]), domain[k][0].size(), domain[k][l]);
              }
              rules[i] = tempRule3;
            }
	    break;
          }
        }
      }
        

  }

  return;

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
    cout << "Enter: load <file>, prop, show, clear, trace, quit or <query>\n";
    cin >> inputVal;

    if (inputVal == "load") {
      cout << "Enter the filename:\n";
      cin >> filename;
      load(filename);
    }

    else if (inputVal == "prop")
      propositionalize(rules);

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
