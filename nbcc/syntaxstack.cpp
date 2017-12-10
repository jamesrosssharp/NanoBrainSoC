/*=============================================================================
                                                    _
         .-==-.                   ..--/\/\--..     (_)
    __      ___    __      ____  ___  ____  ___    ____
   /  \  /\/ _ \/\/  \  /\/ _  \/ _ \/ _  \/ _ \/\/ /  \  /\
  / /\ \/ / (_)  / /\ \/ / (_) / (_)/ (_) / (_)  / / /\ \/ /
  \/  \__/\__/\_/\/  \__/\____/ (_) \ __ (\__/\_/_/\/  \__/
                              \_____//  \_\
   .-==-.           --===/\/\/\===---             ..--/\/\--..

    NanoBrain C Compiler (c) 2017 James Ross Sharp


=============================================================================*/

#include "syntaxstack.h"


SyntaxStack::~SyntaxStack()
{

    for (Syntax::Syntagma *s : m_stack)
        delete s;
}

void SyntaxStack::push(Syntax::Syntagma* syntagma)
{
    m_stack.push_back(syntagma);
}

Syntax::Syntagma* SyntaxStack::pop()
{
   if (empty())
       return nullptr;

   Syntax::Syntagma* s = m_stack.back();
   m_stack.pop_back();
   return s;
}

Syntax::Syntagma*& SyntaxStack::peek()
{
    return m_stack.back();
}

bool SyntaxStack::isEmpty(SyntaxStack* stack)
{
    if (stack == nullptr)
        return true;

    return stack->empty();
}

bool SyntaxStack::empty()
{
    return m_stack.empty();
}
