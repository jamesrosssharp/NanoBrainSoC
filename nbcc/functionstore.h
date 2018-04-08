#pragma once

#include "codegendefs.h"

class FunctionStore
{
public:

    using FunctionHandle = int;

    class Func
    {
    private:
        FunctionStore::FunctionHandle m_handle;

    public:
        Func(FunctionStore::FunctionHandle handle) : m_handle(handle) { }

        const CodeGen::Function& operator * () const
        {
            if (m_handle == 0)
                throw std::runtime_error("Deref bad handle!");
            FunctionStore* f = FunctionStore::getInstance();
            return *f->getFunction(m_handle);
        }

        const CodeGen::Function* operator -> () const
        {
            if (m_handle == 0)
                throw std::runtime_error("Deref bad handle!");
            FunctionStore* f = FunctionStore::getInstance();
            return f->getFunction(m_handle);
        }
    };

    static FunctionStore* getInstance();

    FunctionHandle declareFunction(CodeGen::Function f);

    FunctionHandle findFunctionByName(std::string name);

    CodeGen::Function* getFunction(FunctionHandle handle) { return &m_functions.at(handle - 1); }

private:
    FunctionStore();

    std::vector<CodeGen::Function> m_functions;

};

