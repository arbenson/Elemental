/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

namespace El {

template<typename Real>
void HingeLossProx( Matrix<Real>& A, Real tau )
{
    DEBUG_ONLY(CallStackEntry cse("HingeLossProx"))
    auto hingeProx = 
      [=]( Real alpha )
      { if( alpha < 1 ) { return Min(alpha+1/tau,Real(1)); }
        else            { return alpha;                    } };
    EntrywiseMap( A, std::function<Real(Real)>(hingeProx) );
}

template<typename Real>
void HingeLossProx( AbstractDistMatrix<Real>& A, Real tau )
{
    DEBUG_ONLY(CallStackEntry cse("HingeLossProx"))
    auto hingeProx = 
      [=]( Real alpha )
      { if( alpha < 1 ) { return Min(alpha+1/tau,Real(1)); }
        else            { return alpha;                    } };
    EntrywiseMap( A, std::function<Real(Real)>(hingeProx) );
}

#define PROTO(Real) \
  template void HingeLossProx( Matrix<Real>& A, Real tau ); \
  template void HingeLossProx( AbstractDistMatrix<Real>& A, Real tau );

#define EL_NO_INT_PROTO
#define EL_NO_COMPLEX_PROTO
#include "El/macros/Instantiate.h"

} // namespace El
