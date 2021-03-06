/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

namespace El {

template<typename F> 
void Lehmer( Matrix<F>& L, Int n )
{
    DEBUG_ONLY(CallStackEntry cse("Lehmer"))
    L.Resize( n, n );
    auto lehmerFill = 
      []( Int i, Int j )
      { if( i < j ) { return F(i+1)/F(j+1); }
        else        { return F(j+1)/F(i+1); } };
    IndexDependentFill( L, std::function<F(Int,Int)>(lehmerFill) );
}

template<typename F>
void Lehmer( AbstractDistMatrix<F>& L, Int n )
{
    DEBUG_ONLY(CallStackEntry cse("Lehmer"))
    L.Resize( n, n );
    auto lehmerFill = 
      []( Int i, Int j )
      { if( i < j ) { return F(i+1)/F(j+1); }
        else        { return F(j+1)/F(i+1); } };
    IndexDependentFill( L, std::function<F(Int,Int)>(lehmerFill) );
}

template<typename F>
void Lehmer( AbstractBlockDistMatrix<F>& L, Int n )
{
    DEBUG_ONLY(CallStackEntry cse("Lehmer"))
    L.Resize( n, n );
    auto lehmerFill = 
      []( Int i, Int j )
      { if( i < j ) { return F(i+1)/F(j+1); }
        else        { return F(j+1)/F(i+1); } };
    IndexDependentFill( L, std::function<F(Int,Int)>(lehmerFill) );
}

#define PROTO(F) \
  template void Lehmer( Matrix<F>& L, Int n ); \
  template void Lehmer( AbstractDistMatrix<F>& L, Int n ); \
  template void Lehmer( AbstractBlockDistMatrix<F>& L, Int n );

#define EL_NO_INT_PROTO
#include "El/macros/Instantiate.h"

} // namespace El
