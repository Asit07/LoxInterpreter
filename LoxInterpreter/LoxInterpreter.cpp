// Joe Hudson and Asit Singh 403
// Joe Tasks: Structure, Math, Fun, Return, While, For, Testing
// Asit Tasks: Print, Var, Assignment, If, Else, Boolean, Testing

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "LoxInterpreter.h"

using namespace std;

void PrintPostOrder(node* curNode)
{
    //Prints argument tree in postorder listing children as well
    if(curNode == nullptr)
        return;

    PrintPostOrder(curNode->lChild);
    PrintPostOrder(curNode->rChild);

    if(curNode->lChild != nullptr)
        cout << "Left Child is " << curNode->lChild->value << " ";
    else
        cout << "Left Child is nullptr ";
    if(curNode->rChild != nullptr)
        cout << "Right Child is " << curNode->rChild->value << " ";
    else
        cout << "Right Child is nullptr ";
    cout << "Node value is " << curNode->value << endl;
}

void AddNewlines(string* str)
{
    //converts "\n" to newline character
    int pos = str->find("\\n");
    while(pos < str->length())
    {
        str->erase(pos, 2);
        str->insert(pos, 1, '\n');
        pos = str->find("\\n");
    }
}

bool IsNumber(const string& str)
{
    //determines if string is number
    for(char const &c : str)
    {
        if(isdigit(c) == 0 && c  != '.') return false;
    }
    return true;
}

bool IsBool(string str)
{
    //Determines if string is boolean literal
    if(str == "True")
        return true;
    if(str == "False")
        return false;
    return false;
}

void ConvertStrings(vector<string>* sarg)
{
    //Removes "" from strings
    for(int i=0; i<sarg->size(); i++)
    {
        if(sarg->at(i)[0] == '\"' && sarg->at(i)[sarg->at(i).length()-1] == '\"')
            sarg->at(i) = sarg->at(i).substr(1, sarg->at(i).length()-2);
    }
}

void AddZeroes(node* curNode)
{
    //adds 0 node to missing - arguments
    if(curNode == nullptr)
        return;

    if(curNode->value == "-")
    {
        if(curNode->rChild == nullptr)
        {
            curNode->rChild = new node;
            curNode->rChild->value = "0";
            curNode->rChild->lChild = nullptr;
            curNode->rChild->rChild = nullptr;
            curNode->rChild->priority = 0;
        }
        if(curNode->lChild == nullptr)
        {
            curNode->lChild = new node;
            curNode->lChild->value = "0";
            curNode->lChild->lChild = nullptr;
            curNode->lChild->rChild = nullptr;
            curNode->lChild->priority = 0;
        }
    }
    AddZeroes(curNode->lChild);
    AddZeroes(curNode->rChild);
}

void DeleteTree(node* curNode)
{
    //Deltes arg tree
    if(curNode == nullptr)
        return;

    DeleteTree(curNode->lChild);
    DeleteTree(curNode->rChild);

    delete curNode;
}

string NamedTokenConv(string name, variableTable* vTable)
{
    //Returns type of named token 
    for(int i=0; i<vTable->floatVarTable.size(); i++)
    {
        if(vTable->floatVarTable.at(i).varName == name)
            return "float";
    }

    for(int i=0; i<vTable->stringVarTable.size(); i++)
    {
        if(vTable->stringVarTable.at(i).varName == name)
            return "string";
    }

    for(int i=0; i<vTable->boolVarTable.size(); i++)
    {
        if(vTable->boolVarTable.at(i).varName == name)
            return "bool";
    }
    return "named_token";
}

bool VarEval(string name, args* arg, variableTable* vTable = &globalVarTable)
{
    //Determines if named token is a variable
    for(int i=0; i<vTable->floatVarTable.size(); i++)
    {
        if(vTable->floatVarTable.at(i).varName == name)
        {
            arg->farg.push_back(vTable->floatVarTable.at(i).value);
            return true;
        }
    }

    for(int i=0; i<vTable->stringVarTable.size(); i++)
    {
        if(vTable->stringVarTable.at(i).varName == name)
        {
            arg->sarg.push_back(vTable->stringVarTable.at(i).value);
            return true;
        }
    }

    for(int i=0; i<vTable->boolVarTable.size(); i++)
    {
        if(vTable->boolVarTable.at(i).varName == name)
        {
            arg->barg.push_back(vTable->boolVarTable.at(i).value);
            return true;
        }
    }

    return false;
}

void ClearVariables(variableTable* vTable)
{
    //Clears function table variable values
    int boolSize = vTable->boolVarTable.size();
    int floatSize = vTable->floatVarTable.size();
    int stringSize = vTable->stringVarTable.size();
    for(int i=0; i<boolSize; i++)
        vTable->boolVarTable.pop_back();
    for(int i=0; i<floatSize; i++)
        vTable->floatVarTable.pop_back();
    for(int i=0; i<stringSize; i++)
        vTable->stringVarTable.pop_back();
}

string GetType(string input, variableTable* vTable = &globalVarTable)
{
    //Returns appropriate input type
    for(int i=0; i<OPERATORSIZE; i++)
    {
        if(input == OPERATORS[i].op)
        {
            if(OPERATORS[i].priority == 2 || OPERATORS[i].priority == 3)
                return "float";
            else if(OPERATORS[i].priority == 1)
                return "boolLiteral";
            else
                return "bool";
        }
    }

    if(input[0] == '\"' && input[input.length()-1] == '\"')
        return "string";

    else if(input == "True" || input == "False")
        return "bool";

    else if((input[0] == '(' && input[input.length()-1] == ')') || (input[0] == '{' && input[input.length()-1] == '}'))
        return "parentheses";

    else if(IsNumber(input))
        return "float";

    return "named_token";
}

bool FunCall(string braceArgs, args* arg, variableTable* vTable)
{
    //Runs called function
    string str;
    string argType;
    int j=0;

    //Gets function arguments
    for(int i=1; i<loadedFunArgs.length(); i++)
    {
        if(loadedFunArgs[i] == ',' || i == loadedFunArgs.length()-2)
        {
            argType = GetType(str, vTable);
            if(argType == "float")
                vTable->floatVarTable.push_back({.varName = vTable->noTypeVarTable.at(j++), .value = stof(str)});

            else if(argType == "string")
                vTable->stringVarTable.push_back({.varName = vTable->noTypeVarTable.at(j++), .value = str});

            else if(argType == "bool")
                vTable->boolVarTable.push_back({.varName = vTable->noTypeVarTable.at(j++), .value = IsBool(str)});

            str = "";
        }
        else if(loadedFunArgs[i] == '\"')
        {
            for(i = i+1; loadedFunArgs[i] != '\"'; i++)
                str.push_back(loadedFunArgs[i]);
            str.insert(0, "\"");
            str.push_back('\"');
        }
        else if(loadedFunArgs[i] != ' ')
            str.push_back(loadedFunArgs[i]);
    }
    
    //Runs code inside function {}
    vector<string> funCodeLines = ParseTokens(braceArgs.substr(1, braceArgs.length()-2));
    node* newRoot = nullptr;
    RunCode(funCodeLines, &newRoot, arg, vTable);
    ClearVariables(vTable);
    return true;
}


bool FunEval(string name, expression* expr, args* arg, bool callFun=true)
{
    //Determines if named token is a function
    for(int i=0; i<globalFunTable.size(); i++)
    {
        if(globalFunTable.at(i).funName == name && callFun)
            return FunCall(globalFunTable.at(i).functionContent, arg, &globalFunTable.at(i).funVarTable);
        else if(globalFunTable.at(i).funName == name)
            return true;
    }
    return false;
}

void CreateVar(string name, args* arg, variableTable* vTable = &globalVarTable)
{
    //Adds variable to table with no determined type
    if(VarEval(name, arg, vTable)) return;
    vTable->noTypeVarTable.push_back(name);
}

template<typename T>
void TreeConversion(T val, vector<T>* targs, string sarg, args* arg, variableTable* vTable=&globalVarTable, node** curNode = nullptr)
{
    //Converts () and {} to arguments
    if((sarg[0] == '(' && sarg[sarg.length()-1] == ')') || (sarg[0] == '{' && sarg[sarg.length()-1] == '}'))
    {
        vector<string> line = ParseTokens(sarg.substr(1, sarg.length()-2));

        node* newRoot = nullptr;
        RunCode(line, &newRoot, arg, vTable);
        (*curNode)->value = newRoot->value;
        (*curNode)->priority = newRoot->priority;
        (*curNode)->lChild = newRoot->lChild;
        (*curNode)->rChild = newRoot->rChild;
    }
    //Adds literal value to appropriate argument list
    else
        targs->push_back(val);
}

template<typename T>
void Print(T arg)
{
    //Prints argument
    cout << arg;
}

template<typename T>
void Var(expression* expr, args* arguments, bool varDec, variableTable* vTable=&globalVarTable)
{
    //Creates variable by passing varDec boolean as true
    ExecuteTree(expr, expr->args, arguments, vTable, varDec);
}

template<typename T>
void Assignment(vector<T>* args, vector<varTable<T>>* variableTable, vector<string>* noTypeTable, string loadedVariable)
{
    //Assigns value to appropriate variable table
    for(int i=0; i<variableTable->size(); i++)
    {
        if(variableTable->at(i).varName == loadedVariable)
        {
            variableTable->at(i).value = args->back();
            args->pop_back();
            return;
        }
    }

    //Adds to table where type is not yet decided
    for(int i=0; i<noTypeTable->size(); i++)
    {
        if(noTypeTable->at(i) == loadedVariable)
        {
            variableTable->push_back(varTable<T>{.varName = loadedVariable, .value = args->back()});
            args->pop_back();
            return;
        }

        else if(i == noTypeTable->size()-1)
        {
            variableTable->push_back(varTable<T>{.varName = loadedVariable, .value = args->back()});
            args->pop_back();
            return;
        }
    }
}

void Plus(vector<float>* args)
{
    //Adds two numbers
    float temp = (args->at(args->size()-2) + args->back());
    args->pop_back();
    args->pop_back();
    args->push_back(temp);
}

void Minus(vector<float>* args)
{
    //Subtracts two numbers
    float temp = (args->at(args->size()-2) - args->back());
    args->pop_back();
    args->pop_back();
    args->push_back(temp);
}

void Multiply(vector<float>* args)
{
    //Multiplies two numbers
    float temp = (args->at(args->size()-2) * args->back());
    args->pop_back();
    args->pop_back();
    args->push_back(temp);
}

void Divide(vector<float>* args)
{
    //Divides two numbers
    float temp = (args->at(args->size()-2) / args->back());
    args->pop_back();
    args->pop_back();
    args->push_back(temp);
}

template<typename T>
void True(vector<T>* args, vector<bool>* barg)
{
    //evaluates to True
    barg->push_back(true);
}

template<typename T>
void False(vector<T>* args, vector<bool>* barg)
{
    //evaluates to False
    barg->push_back(false);
}

template<typename T>
void Not(vector<T>* args, vector<bool>* barg)
{
    //run ! boolean transformation
    barg->at(barg->size()-1) = !barg->at(barg->size()-1);
}

template<typename T>
void LessThan(vector<T>* args, vector<bool>* barg)
{
    //runs > boolean comparison
    if(args->at((args->size())-2) < args->back())
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(true);
    }
    else
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(false);
    }
}

template<typename T>
void GreaterThan(vector<T>* args, vector<bool>* barg)
{
    //runs < boolean comparison
    if(args->at((args->size())-2) > args->back())
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(true);
    }
    else
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(false);
    }
}

template<typename T>
void LessThanEqual(vector<T>* args, vector<bool>* barg)
{
    // Runs >= boolean comparison
    if(args->at((args->size())-2) <= args->back())
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(true);
    }
    else
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(false);
    }
}

template<typename T>
void GreaterThanEqual(vector<T>* args, vector<bool>* barg)
{
    //Runs <= boolean comparison
    if(args->at((args->size())-2) >= args->back())
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(true);
    }
    else
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(false);
    }
}

template<typename T>
void EqualTo(vector<T>* args, vector<bool>* barg)
{
    //Runs equal boolean comparison
    if(args->at((args->size())-2) == args->back())
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(true);
    }
    else
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(false);
    }
}

template<typename T>
void NotEqualTo(vector<T>* args, vector<bool>* barg)
{
    //Runs != boolean comparison
    if(args->at((args->size())-2) != args->back())
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(true);
    }
    else
    {
        args->pop_back();
        args->pop_back();
        barg->push_back(false);
    }
}

void And(vector<bool>* barg)
{
    //Runs And boolean comparison
    if(barg->at((barg->size())-2) && barg->back())
    {
        barg->pop_back();
        barg->pop_back();
        barg->push_back(true);
    }
    else
    {
        barg->pop_back();
        barg->pop_back();
        barg->push_back(false);
    }
}

void Or(vector<bool>* barg)
{
    //Runs Or boolean comparison
    if(barg->at((barg->size())-2) || barg->back())
    {
        barg->pop_back();
        barg->pop_back();
        barg->push_back(true);
    }
    else
    {
        barg->pop_back();
        barg->pop_back();
        barg->push_back(false);
    }
}

template<typename T>
void If(T arg, expression* expr, args* arguments, variableTable* vTable=&globalVarTable)
{
    //Runs if block
    //If arg is false then else flag is set to true in case of else block
    if(arg)
    {
        vector<string> ifCodeLines = ParseTokens(expr->braceArgs.substr(1, expr->braceArgs.length()-2));

        node* newRoot = nullptr;
        RunCode(ifCodeLines, &newRoot, arguments, vTable);
        elseFlag = false;
    }
    else
        elseFlag = true;
}

template<typename T>
void Else(expression* expr, args* arguments, variableTable* vTable=&globalVarTable)
{
    //Runs else block
    if(elseFlag)
    {
        vector<string> elseCodeLines = ParseTokens(expr->braceArgs.substr(1, expr->braceArgs.length()-2));

        node* newRoot = nullptr;
        RunCode(elseCodeLines, &newRoot, arguments, vTable);
        elseFlag = false;
    }
}

template<typename T>
void While(expression* expr, args* argument, vector<bool>* barg, variableTable* vTable = &globalVarTable)
{
    //While loop boolean and loop exectuion
    ExecuteTree(expr, expr->args, argument, vTable);
    while(barg->back())
    {
        vector<string> whileCodeLines = ParseTokens(expr->braceArgs.substr(1, expr->braceArgs.length()-2));

        node* newRoot = nullptr;
        RunCode(whileCodeLines, &newRoot, argument, vTable);
        barg->pop_back();
        ExecuteTree(expr, expr->args, argument, vTable);
    }
}

template<typename T>
void For(expression* expr, args* argument, vector<bool>* barg, variableTable* vTable = &globalVarTable)
{
    //Runs for loop
    vector<vector<string>> forArguments;
    int mark = 1;

    //Get for arguments
    for(int i=0; i<expr->args->value.length(); i++)
    {
        if(expr->args->value[i] == ';' || i == expr->args->value.length()-2)
        {
            forArguments.push_back(ParseTokens(expr->args->value.substr(mark, i-mark+1)));
            mark = i+1;
        }
    }
    
    forArguments.at(forArguments.size()-1).push_back(";");

    node* varInit = nullptr;
    node* boolCond = nullptr;
    node* finalArg = nullptr;


    //Run first two arguments
    RunCode(forArguments.at(0), &varInit, argument, vTable);
    RunCode(forArguments.at(1), &boolCond, argument, vTable, false);
    ExecuteTree(expr, boolCond, argument, vTable);

    //Loop and third argument execution
    while(barg->back())
    {
        vector<string> forCodeLines = ParseTokens(expr->braceArgs.substr(1, expr->braceArgs.length()-2));
        node* newRoot = nullptr;
        RunCode(forCodeLines, &newRoot, argument, vTable);
        barg->pop_back();
        RunCode(forArguments.at(2), &finalArg, argument, vTable);
        ExecuteTree(expr, boolCond, argument, vTable);
    }
}

template<typename T>
void Fun(expression* expr,  args* arg)
{
    //Creates function and adds to global function table
    funTable temp;
    string str;

    //Split arguments
    for(int i=1; i<expr->args->value.length(); i++)
    {
        if(expr->args->value[i] == ',' || i == expr->args->value.length()-2)
        {
            temp.funVarTable.noTypeVarTable.push_back(str);
            str = "";
        }
        else if(expr->args->value[i] != ' ')
            str.push_back(expr->args->value[i]);
    }
    temp.functionContent = expr->braceArgs;
    temp.funName = expr->args->lChild->value;
    globalFunTable.push_back(temp);
}

template<typename T>
void Return(expression* expr, args* arguments, variableTable* vTable = &globalVarTable)
{
    //Gets return value and the type
    ExecuteTree(expr, expr->args, arguments, vTable);

    loadedReturnType = GetType(expr->args->value, vTable);
    if(loadedReturnType == "named_token")
        loadedReturnType = NamedTokenConv(expr->args->value, vTable);
}

void MathTreeEval(vector<float>* args, string sarg)
{
    //Runs math operators
    if(sarg == "+")
        Plus(args);

    if(sarg == "-")
        Minus(args);

    else if(sarg == "*")
        Multiply(args);

    else if(sarg == "/")
        Divide(args);    
}

template<typename T>
void BoolTreeEval(vector<T>* args, vector<bool>* bargs, string sarg, vector<varTable<T>>* varTable, vector<string>* noTypeTable, string loadedVariable, variableTable* vTable = &globalVarTable)
{
    //Runs booleand operators and assignment operator
    if(sarg == "True")
        True<T>(args, bargs);

    else if(sarg == "False")
        False<T>(args, bargs);

    else if(sarg == "!")
        Not<T>(args, bargs);

    else if(sarg == "<")
        LessThan<T>(args, bargs);

    else if(sarg == ">")
        GreaterThan<T>(args, bargs);

    else if(sarg == "<=")
        LessThanEqual<T>(args, bargs);

    else if(sarg == ">=")
        GreaterThanEqual<T>(args, bargs);

    else if(sarg == "==")
        EqualTo<T>(args, bargs);

    else if(sarg == "!=")
        NotEqualTo<T>(args, bargs);

    else if(sarg == "and")
        And(bargs);

    else if(sarg == "or")
        Or(bargs);

    else if(sarg == "=")
        Assignment<T>(args, varTable, noTypeTable, loadedVariable);
}

template<typename T>
void ExpressionEval(expression* expr, args* arguments, vector<bool>* barg, vector<T>* arg, variableTable* vTable = &globalVarTable)
{
    //Runs token functions 
    if(expr->func == "print")
    {
        ExecuteTree(expr, expr->args, arguments, vTable);
        Print<T>(arg->back());
        arg->pop_back();
    }   

    else if(expr->func == "var")
        Var<T>(expr, arguments, true, vTable);
    
    else if(expr->func == "")
        Var<T>(expr, arguments, false, vTable);

    else if(expr->func == "if")
    {
        ExecuteTree(expr, expr->args, arguments, vTable);
        If<bool>(barg->back(), expr, arguments, vTable);
        barg->pop_back();
        return;
    }

    else if(expr->func == "else")
        Else<T>(expr, arguments, vTable);

    else if(expr->func == "while")
        While<bool>(expr, arguments, barg, vTable);

    else if(expr->func == "for")
        For<bool>(expr, arguments, barg, vTable);

    else if(expr->func == "fun")
        Fun<T>(expr, arguments);

    else if(expr->func == "return")
        Return<T>(expr, arguments, vTable);

    else if(expr->func == ";")
        return;
}

void NamedTokenEval(expression* expr, args* arg, variableTable* vTable = &globalVarTable)
{
    //Determine what type the named token is and execute token function
    for(int i=0; i<vTable->floatVarTable.size(); i++)
    {
        if(vTable->floatVarTable.at(i).varName == expr->args->value)
            ExpressionEval<float>(expr, arg, &arg->barg, &arg->farg, vTable);
    }
    for(int i=0; i<vTable->boolVarTable.size(); i++)
    {
        if(vTable->boolVarTable.at(i).varName == expr->args->value)
            ExpressionEval<bool>(expr, arg, &arg->barg, &arg->barg, vTable);
    }
    for(int i=0; i<vTable->stringVarTable.size(); i++)
    {
        if(vTable->stringVarTable.at(i).varName == expr->args->value)
            ExpressionEval<string>(expr, arg, &arg->barg, &arg->sarg, vTable);
    }
}

void RunTree(expression* expr, node* curNode, args* arg, variableTable* vTable, bool varDec)
{
    //Executes Nodes in PostOrder
    if(curNode == nullptr)
        return;

    //Determine type
    string argType = GetType(curNode->value, vTable);

    //Chekc if function call
    if(curNode->lChild != nullptr)
    {
        if(FunEval(curNode->lChild->value, expr, arg, false))
        {
            loadedFunArgs = curNode->value;
            ExecuteTree(expr, curNode->lChild, arg, vTable, varDec);
            ExecuteTree(expr, curNode->rChild, arg, vTable, varDec);
            return;
        }
    }

    //set variable to be assigned if assignment operator is used
    if(curNode->value == "=")
        vTable->loadedVariable = curNode->lChild->value;

    ExecuteTree(expr, curNode->lChild, arg, vTable, varDec);
    ExecuteTree(expr, curNode->rChild, arg, vTable, varDec);

    //If node is a literal value
    if(curNode->priority == 0)
    {
        if(argType == "float")
        {
            float argConv = stof(curNode->value);
            TreeConversion<float>(argConv, &arg->farg, curNode->value, arg, vTable, &curNode);
        }
        else if(argType == "bool")
        {
            bool argConv;
            if(curNode->value == "True")
                argConv = true;
            if(curNode->value == "False")
                argConv = false;
            TreeConversion<bool>(argConv, &arg->barg, curNode->value, arg, vTable);
        }
        else if(argType == "string")
            TreeConversion<string>(curNode->value, &arg->sarg, curNode->value, arg, vTable);
        else if(argType == "parentheses")
        {
            TreeConversion<string>(curNode->value, &arg->sarg, curNode->value, arg, vTable, &curNode);
            ExecuteTree(expr, curNode, arg, vTable, varDec);
        }
        else if(argType == "named_token")
        {
            if(varDec) CreateVar(curNode->value, arg, vTable);
            else if(FunEval(curNode->value, expr, arg));
            else if(VarEval(curNode->value, arg, vTable));
        }
    }

    //If node is a token
    else if(curNode->priority > 0)
    {
        if(argType == "float")
            MathTreeEval(&arg->farg, curNode->value);

        else if(argType == "boolLiteral")
            BoolTreeEval(&arg->farg, &arg->barg, curNode->value, &vTable->floatVarTable, &vTable->noTypeVarTable, vTable->loadedVariable, vTable);

        else if(argType == "bool")
        {
            string childArgType = GetType(curNode->rChild->value, vTable);
            if(childArgType == "named_token")
                childArgType = NamedTokenConv(curNode->rChild->value, vTable);
            if(childArgType == "bool" || childArgType == "boolLiteral")
                BoolTreeEval<bool>(&arg->barg, &arg->barg, curNode->value, &vTable->boolVarTable, &vTable->noTypeVarTable, vTable->loadedVariable, vTable);
            else if(childArgType == "float")
                BoolTreeEval<float>(&arg->farg, &arg->barg, curNode->value, &vTable->floatVarTable, &vTable->noTypeVarTable, vTable->loadedVariable, vTable);
            else if(childArgType == "string")
                BoolTreeEval<string>(&arg->sarg, &arg->barg, curNode->value, &vTable->stringVarTable, &vTable->noTypeVarTable, vTable->loadedVariable, vTable);
        }
    }
}

void ExecuteTree(expression* expr, node* curNode, args* arg, variableTable* vTable, bool varDec)
{
    //Runs the argument tree unless bFunCall is true
    //If bFunCall is true then only an argument tree is made and not run
    if(!bFunCall)
        RunTree(expr, curNode, arg, vTable, varDec);
    ConvertStrings(&arg->sarg);
}

node* SwimDown(node* curNode, node* newNode)
{
    //Creates arg trees
    if(curNode == nullptr || (curNode->priority <= newNode->priority))
    {
        newNode->lChild = curNode;
        return newNode;
    }

    else
    {
        curNode->rChild = SwimDown(curNode->rChild, newNode);
        return curNode;
    }
}

void AddNode(node** root, string value, int priority)
{
    //Adds node to argument tree
    node* newNode = new node;
    newNode->value = value; 
    newNode->priority = priority;
    newNode->lChild = nullptr;
    newNode->rChild = nullptr;

    if(*root == nullptr)
        *root = newNode;

    else
        *root = SwimDown(*root, newNode);
}

bool OpEval(vector<string>* op, expression* expr, int* i, args* arg,  variableTable* vTable=&globalVarTable)
{
    //checks if part of operator array
    for(int j=0; j<OPERATORSIZE; j++)
    {
        if(op->at(*i) == OPERATORS[j].op)
        {
            AddNode(&(expr->args), static_cast<string>(op->at(*i)), OPERATORS[j].priority);
            return true;
        }
    }

    //Groups () and {} together as one token to be parsed later
    if(op->at(*i) == "(" || op->at(*i) == "{")
    {
        string temp = op->at(*i);
        int pNested = 1;
        for(*i = *i+1; pNested != 0; (*i)++)
        {
            temp += op->at(*i);
            temp += " "; 
            if(op->at(*i) == ")" || op->at(*i) == "}")
                pNested--;
            if(op->at(*i) == "(" || op->at(*i) == "{")
                pNested++;
        }
        *i = *i-1;
        temp.pop_back();

        if(temp[0] == '{')
        {
            expr->braceArgs = temp;
            Parse(expr, &expr->args, arg, vTable);
        }
        else
            AddNode(&(expr->args), temp);
        return true;
    }
    return false;
}

bool TokenEval(string token, expression* expr)
{
    //Checks if part of token array
    for(int i=0; i<TOKENSIZE; i++)
    {
        if(token == TOKENS[i])
        {
            expr->func = TOKENS[i];
            return true;
        }
    }
    return false;
}

void Parse(expression* expr, node** copyRoot, args* arg, variableTable* vTable)
{
    //Executes passed in line of code

    bFunCall = false;

    //immediately runs else function because expr->args will be null
    if(expr->func == "else")
        ExpressionEval<bool>(expr, arg, &arg->barg, &arg->barg, vTable);

    if((expr->args) == nullptr)
        return;

    //Pads out tree with zeroes for - operator where no first argument is provided
    AddZeroes(expr->args);

    *copyRoot = expr->args;

    string argtype = GetType(expr->args->value, vTable);

    //Checks if a fucntion is being called

    if(expr->args->lChild != nullptr)
    {
        if(FunEval(expr->args->lChild->value, expr, arg, false))
        {
            ExecuteTree(expr, expr->args, arg, vTable); 
            bFunCall = true;
            argtype = loadedReturnType;
        }
    }

    //Calls token functions depending on type

    if(argtype == "string")
        ExpressionEval<string>(expr, arg, &arg->barg, &arg->sarg, vTable);
    else if(argtype == "float")
        ExpressionEval<float>(expr, arg, &arg->barg, &arg->farg, vTable);
    else if(argtype == "bool")
        ExpressionEval<bool>(expr, arg ,&arg->barg, &arg->barg, vTable);
    else if(argtype == "named_token")
        NamedTokenEval(expr, arg, vTable);
    else if(argtype == "parentheses")
        ExpressionEval<bool>(expr, arg, &arg->barg, &arg->barg, vTable);

    expr->args = nullptr;
    expr->func = "";
}

void EvalString(vector<string> codeLines, node** root, expression* expr, args* arg, variableTable* vTable=&globalVarTable)
{
    //Determines where each token should be added to in the expression
    for(int i=0; i<codeLines.size(); i++)
    {
        if(TokenEval(codeLines.at(i), expr));
        else if(OpEval(&codeLines, expr, &i, arg, vTable));
        else if(codeLines.at(i) == ";") Parse(expr, root, arg, vTable);
        else AddNode(&expr->args, codeLines.at(i));
    }
}

void RunCode(vector<string> codeLines, node** root, args* arg, variableTable* vTable, bool parse)
{
    //Entry function for interpreting code
    //Expression stores the argument tree, potential {} contents, and prefix token
    expression expr = {.args = *root};
    EvalString(codeLines, root, &expr, arg, vTable);
    if(parse)
    {
        *root = expr.args;
        Parse(&expr, root, arg, vTable);
    }
}

void StringAdd(vector<string>* lst, string* str)
{
    //Add string token to vector
    if(*str != "")
        lst->push_back(*str);
    *str = "";
}

vector<string> ParseTokens(string line)
{
    vector<string> tokens;
    string temp;

    //Split tokens and store them into vector
    //Seperate on "", '', (, ), " "
    for(int i=0; i<line.length(); i++)
    {
        for(int j=0; j<OPERATORSIZE; j++)
        {
            if((line[i] == OPERATORS[j].op[0] || line[i] == '!'))
            {
                if(line[i + 1] == '=')
                {
                    StringAdd(&tokens, &temp);
                    temp.push_back(line[i]);
                    temp.push_back(line[i+1]);
                    i += 2;
                    break;
                }
                else if(line[i] == OPERATORS[j].op[0] && OPERATORS[j].op.size() == 1)
                {
                    StringAdd(&tokens, &temp);
                    tokens.push_back(OPERATORS[j].op);
                    i++;
                    break;
                }
            }     
        }

        if(line[i] == '(')
        {
            StringAdd(&tokens, &temp);
            tokens.push_back("(");
        }
            
        else if(line[i] == ')')
        {
            StringAdd(&tokens, &temp);  
            tokens.push_back(")");
        }

        else if(line[i] == '{')
        {
            StringAdd(&tokens, &temp);
            tokens.push_back("{");
        }
            
        else if(line[i] == '}')
        {
            StringAdd(&tokens, &temp);
            tokens.push_back("}");
        }

        else if(line[i] == ';')
        {
            StringAdd(&tokens, &temp);
            tokens.push_back(";");
        }
        
        else if(line[i] == '\"')
        {
            for(i = i+1; line[i] != '\"'; i++)
                temp.push_back(line[i]);
            temp.insert(0, "\"");
            temp.push_back('\"');
        }

        else if(line[i] == '\'')
        {
            for(i = i+1; line[i] != '\''; i++)
                temp.push_back(line[i]);
            temp.insert(0, "\'");
            temp.push_back('\'');
        }

        else if(line[i] == ' ')
            StringAdd(&tokens, &temp);
            
        else
            temp.push_back(line[i]);
    }
    StringAdd(&tokens, &temp);
    return tokens;
}

int main()
{
    string path = "Lox_Tests/";
    string codeLines;
    string temp;
    ifstream testFile;
    node* root = nullptr;
    args* arg = new args;

    //Read through Lox Test Files 
    for (const auto & testFilePath : filesystem::directory_iterator(path))
    {
        cout << testFilePath.path() << endl;
        testFile.open(testFilePath.path());
        cout << "Testing " << testFilePath.path() << "////////////////////" << endl << endl;
        while(getline(testFile, temp))
        {
            cout << temp << endl;
            codeLines += temp;
        }
        cout << endl << "Results*******************" << endl << endl;
        AddNewlines(&codeLines);
        RunCode(ParseTokens(codeLines), &root, arg);
        DeleteTree(root);
        codeLines = "";
        delete arg;
        arg = new args;
        testFile.close();
        cout << endl;
    }

    return 0;
}