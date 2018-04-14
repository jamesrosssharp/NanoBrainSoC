#include "labelstore.h"

#include <sstream>

LabelStore* LabelStore::getInstance()
{
    static LabelStore s_theInstance;

    return &s_theInstance;
}

LabelStore::LabelStore()
{

}

void LabelStore::pushScope(const std::string& scopeName)
{
    m_scopeStack.push_back(scopeName);
}

void LabelStore::popScope()
{
    m_scopeStack.pop_back();
}

std::string LabelStore::catScopeStack()
{
    std::string s;
    for (std::vector<std::string>::iterator ss = m_scopeStack.begin(); ss != m_scopeStack.end(); ++ss)
        s += "." + *ss;
    return s;
}

LabelStore::LabelHandle  LabelStore::declareLabel(const std::string& name)
{
    std::string prefix = catScopeStack();
    if (prefix != "")
        prefix = prefix + ".";
    std::string varname = prefix + name;

    CodeGen::Label l;
    l.asmName = "l_" + varname;
    l.name = varname;

    m_labels.push_back(l);

    return m_labels.size();
}

LabelStore::LabelHandle LabelStore::declareTemporary()
{
    std::stringstream ss;

    ss << "l" << m_temporaryIdx++;

    return declareLabel(ss.str());
}

LabelStore::Label LabelStore::findLab(const std::string& name)
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
        for (auto l : m_labels)
        {

            if (l.name == nameToFind)
            {
                return Label{i};
            }
            i ++;
        }
    }

    return Label{0};
}
