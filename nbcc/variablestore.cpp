#include "variablestore.h"

#include <sstream>

VariableStore* VariableStore::getInstance()
{
    static VariableStore s_theInstance;

    return &s_theInstance;
}


VariableStore::VariableStore()
{

}

void VariableStore::pushScope(const std::string& scopeName)
{
    m_scopeStack.push_back(scopeName);
}

void VariableStore::popScope()
{
    m_scopeStack.pop_back();
}

std::string VariableStore::catScopeStack()
{
    std::string s;
    for (std::vector<std::string>::reverse_iterator ss = m_scopeStack.rbegin(); ss != m_scopeStack.rend(); ++ss)
        s += "." + *ss;
    return s;
}

VariableStore::VariableHandle  VariableStore::declareVariable(const std::string& name, const CodeGen::Type& type)
{
    std::string prefix = catScopeStack();
    if (prefix != "")
        prefix = prefix + ".";
    std::string varname = prefix + name;

    CodeGen::Variable v;
    v.asmName = "v_" + varname;
    v.name = varname;
    v.initialValue.u = 0;
    v.type = type;

    m_variables.push_back(v);

    return m_variables.size();
}

VariableStore::VariableHandle VariableStore::declareTemporary(const CodeGen::Type& type)
{
    std::stringstream ss;

    ss << "t" << m_temporaryIdx++;

    return declareVariable(ss.str(), type);
}
