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

#pragma once

#include "syntax.h"

#include <deque>

class SyntaxStack
{
public:

    void push(Syntax::Syntagma* syntagma);
    Syntax::Syntagma* pop();
    Syntax::Syntagma*& peek();

    static bool isEmpty(SyntaxStack* stack);
    bool empty();

    ~SyntaxStack();

private:


    std::deque<Syntax::Syntagma*> m_stack;

};
