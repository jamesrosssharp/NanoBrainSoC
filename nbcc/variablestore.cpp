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
    for (std::vector<std::string>::iterator ss = m_scopeStack.begin(); ss != m_scopeStack.end(); ++ss)
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

VariableStore::Var VariableStore::findVar(const std::string& name)
{
    // search for variable name in all scopes from current down to global scope.
    // Return if found, else null variable

    for (auto scope = m_scopeStack.rbegin(); scope != m_scopeStack.rend(); scope ++)
    {
        std::string s;
        for (std::vector<std::string>::reverse_iterator ss = scope; ss != m_scopeStack.rend(); ++ss)
            s = '.' + *ss  + s;

        std::string nameToFind = s + "." + name;

        int i = 1;
        for (auto v : m_variables)
        {

            if (v.name == nameToFind)
            {
                return Var{i};
            }
            else if (v.name == name)
            {
                return Var{i};
            }
            i ++;
        }
    }

    return Var{0};
}
