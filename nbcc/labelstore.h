#pragma once

#include "codegendefs.h"

#include <vector>

class LabelStore
{

public:

    using LabelHandle = int;

    class Label
    {
    private:
        LabelStore::LabelHandle m_handle;

    public:
        Label(LabelStore::LabelHandle handle) : m_handle(handle) { }

        const CodeGen::Label& operator * () const
        {
            if (m_handle == 0)
                throw std::runtime_error("Deref bad handle!");
            LabelStore* l = LabelStore::getInstance();
            return *l->getLabel(m_handle);
        }

        const CodeGen::Label* operator -> () const
        {
            if (m_handle == 0)
                throw std::runtime_error("Deref bad handle!");
            LabelStore* l = LabelStore::getInstance();
            return l->getLabel(m_handle);
        }

        friend bool ::operator == (const Label&, const Label&);
        friend bool ::operator != (const Label&, const Label&);
    };

    static LabelStore* getInstance();

    void pushScope(const std::string& scopeName);
    void popScope();
    LabelHandle declareLabel(const std::string& name);
    LabelHandle declareTemporary();

    Label declareLab(const std::string& name) { return Label(declareLabel(name)); }
    Label declareTempLab() { return Label(declareTemporary()); }
    Label findLab(const std::string& name);

    CodeGen::Label* getLabel(LabelHandle handle) { return &m_labels.at(handle - 1); }

private:
    LabelStore();
    std::string catScopeStack();

    std::vector<CodeGen::Label> m_labels;
    std::vector<std::string> m_scopeStack;  // we need to iterate the stack so we use a vector
    int m_temporaryIdx;   // temporaries are declared of form tXYZ where XYZ is temporaryIdx, an incrementing counter

};

inline bool operator == (const LabelStore::Label& l1, const LabelStore::Label& l2)
{
    return l1.m_handle == l2.m_handle;
}

inline bool operator != (const LabelStore::Label& l1, const LabelStore::Label& l2)
{
    return l1.m_handle != l2.m_handle;
}
