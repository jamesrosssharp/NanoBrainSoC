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

    Notes
    =====

    Intermediate representation, use for compiling expressions into
    a form which can be optimised to ensure CPU pipeline is utilised
    to optimal capacity etc.

=============================================================================*/

#pragma once

#include "codegendefs.h"
#include "types.h"

#include <iostream>
#include <stdint.h>

namespace IntRep
{
    class IntRep;
}

std::ostream& operator << (std::ostream& os, const IntRep::IntRep& i);

namespace IntRep
{

    enum class ElementType
    {
        kDeclareTemporary,
        kLoadImm,
        kDeleteTemporary,
        kOutput,

        kAdd,
    };

    struct Element
    {
        ElementType element;

        CodeGen::Variable* v1 = nullptr;
        CodeGen::Variable* v2 = nullptr;
        CodeGen::Variable* v3 = nullptr;

        ImmediateValue     immval;

    };


    class IntRep
    {
    public:

        void addVar(CodeGen::Variable* left, CodeGen::Variable* right, CodeGen::Variable* out);

        CodeGen::Variable* declareTemporary();

        void loadImm(CodeGen::Variable* var, int32_t intval);
        void loadImm(CodeGen::Variable* var, uint32_t intval);

        void deleteTemporary(CodeGen::Variable* temp);

        void output(CodeGen::Variable* out);

        friend std::ostream& (::operator <<) (std::ostream& os, const IntRep& i);

    private:

        void loadImm(CodeGen::Variable* var, ImmediateValue& v);


        std::vector<CodeGen::Variable> m_temporaries;
        std::vector<std::string> m_labels;

        uint32_t m_tempvarIdx = 0;

        std::vector<Element> m_elements;

    };

}

