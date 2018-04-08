#include "functionstore.h"

FunctionStore::FunctionStore()
{
}

FunctionStore* FunctionStore::getInstance()
{
    static FunctionStore s_theInstance;

    return &s_theInstance;
}

FunctionStore::FunctionHandle FunctionStore::declareFunction(CodeGen::Function f)
{
    m_functions.push_back(f);
    return m_functions.size();
}

FunctionStore::FunctionHandle FunctionStore::findFunctionByName(std::string name)
{

    // TODO: Use a map to store function names against handles to speed up the lookup.

    int i = 1;
    for (const CodeGen::Function& f : m_functions)
    {
        if (f.name == name)
            return i;

        i ++;
    }
    return 0;
}



