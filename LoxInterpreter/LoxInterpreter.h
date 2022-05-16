#include <string>
#include <vector>

using namespace std;

struct operators
{
    string op;
    int priority;
};

struct args
{
    vector<float> farg;
    vector<string> sarg;
    vector<bool> barg;
};

struct node
{
    string value;
    int priority;
    node* lChild;
    node* rChild;
};

struct expression
{
    string func;
    node* args;
    string braceArgs;
};

template<typename T>
struct varTable
{
    string varName;
    T value;
};

struct variableTable
{
    vector<varTable<float>> floatVarTable;
    vector<varTable<string>> stringVarTable;
    vector<varTable<bool>> boolVarTable;
    vector<string> noTypeVarTable;
    string loadedVariable;
};

struct funTable
{
    string funName;
    variableTable funVarTable;
    string functionContent;
};


const string TOKENS[] = {"print", "var", "if", "else", "while", "for", "fun", "return"};
const operators OPERATORS[] = {{"True", 1}, {"False", 1}, {"*", 2}, {"/", 2}, {"+", 3}, {"-", 3}, {"<", 4},
{">", 4}, {"<=", 4}, {">=", 4}, {"==", 4}, {"!=", 4}, {"!", 5}, {"and", 6}, {"or", 6}, {"=", 7}};

const int TOKENSIZE = sizeof(TOKENS)/sizeof(TOKENS[0]);
const int OPERATORSIZE = sizeof(OPERATORS)/sizeof(OPERATORS[0]);

bool elseFlag = false;

variableTable globalVarTable;

vector<funTable> globalFunTable;
string loadedFunArgs;
string loadedReturnType = "fun_args";
bool bFunCall = true;

vector<expression> exprQueue;

vector<string> ParseTokens(string line);

void Parse(expression* expr, node** root, args* arg, variableTable* vTable=&globalVarTable);
void RunCode(vector<string>, node** root, args* arg, variableTable* vTable=&globalVarTable, bool parse = true);
void ExecuteTree(expression* expr, node* curNode, args* arg, variableTable* vTable=&globalVarTable, bool varDec=false);
void PrintPostorder(node*);
void AddNode(node** root, string value, int priority=0);
