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

#include <iostream>

namespace IntRep
{
    class IntRep;
}

std::ostream& operator << (std::ostream& os, const IntRep::IntRep& i);

namespace IntRep
{

    class IntRep
    {
    public:


        friend std::ostream& (::operator <<) (std::ostream& os, const IntRep& i);

    };

}

