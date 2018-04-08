#pragma once

#include "codegendefs.h"

#include <vector>

class VariableStore
{

public:

    using VariableHandle = int;

    class Var
    {
    private:
        VariableStore::VariableHandle m_handle;

    public:
        Var(VariableStore::VariableHandle handle) : m_handle(handle) { }

        const CodeGen::Variable& operator * () const
        {
            if (m_handle == 0)
                throw std::runtime_error("Deref bad handle!");
            VariableStore* v = VariableStore::getInstance();
            return *v->getVariable(m_handle);
        }

        const CodeGen::Variable* operator -> () const
        {
            if (m_handle == 0)
                throw std::runtime_error("Deref bad handle!");
            VariableStore* v = VariableStore::getInstance();
            return v->getVariable(m_handle);
        }

        friend bool ::operator == (const Var&, const Var&);
    };

    static VariableStore* getInstance();

    void pushScope(const std::string& scopeName);
    void popScope();
    VariableHandle declareVariable(const std::string& name, const CodeGen::Type& type);
    VariableHandle declareTemporary(const CodeGen::Type& type);

    Var declareVar(const std::string& name, const CodeGen::Type& type) { return Var(declareVariable(name, type)); }
    Var declareTemporaryVar(const CodeGen::Type& type) { return Var(declareTemporary(type)); }
    Var findVar(const std::string& name);

    CodeGen::Variable* getVariable(VariableHandle handle) { return &m_variables.at(handle - 1); }

private:
    VariableStore();
    std::string catScopeStack();

    std::vector<CodeGen::Variable> m_variables;
    std::vector<std::string> m_scopeStack;  // we need to iterate the stack so we use a vector
    int m_temporaryIdx;   // temporaries are declared of form tXYZ where XYZ is temporaryIdx, an incrementing counter

};

inline bool operator == (const VariableStore::Var& v1, const VariableStore::Var& v2)
{
    return v1.m_handle == v2.m_handle;
}

