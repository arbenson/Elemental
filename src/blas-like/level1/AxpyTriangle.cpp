/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

namespace El {

template<typename T,typename S>
void AxpyTriangle
( UpperOrLower uplo, S alphaS, const Matrix<T>& X, Matrix<T>& Y )
{
    DEBUG_ONLY(
        CallStackEntry cse("AxpyTriangle");
        if( X.Height() != X.Width() || Y.Height() != Y.Width() || 
            X.Height() != Y.Height() )
            LogicError("Nonconformal AxpyTriangle");
    )
    const T alpha = T(alphaS);
    if( uplo == UPPER )
    {
        for( Int j=0; j<X.Width(); ++j )
            blas::Axpy( j+1, alpha, X.LockedBuffer(0,j), 1, Y.Buffer(0,j), 1 );
    }
    else
    {
        const Int n = X.Height();
        for( Int j=0; j<X.Width(); ++j )
            blas::Axpy( n-j, alpha, X.LockedBuffer(j,j), 1, Y.Buffer(j,j), 1 );
    }
}

template<typename T,typename S>
void AxpyTriangle
( UpperOrLower uplo, S alphaS, 
  const AbstractDistMatrix<T>& X, AbstractDistMatrix<T>& Y )
{
    DEBUG_ONLY(
        CallStackEntry cse("AxpyTriangle");
        if( X.Grid() != Y.Grid() )
            LogicError
            ("X and Y must be distributed over the same grid");
        if( X.Height() != X.Width() || Y.Height() != Y.Width() || 
            X.Height() != Y.Height() )
            LogicError("Nonconformal AxpyTriangle");
    )
    const T alpha = T(alphaS);

    const DistData XDistData = X.DistData();
    const DistData YDistData = Y.DistData();

    if( XDistData == YDistData )
    {
        const Int localHeight = X.LocalHeight();
        const Int localWidth = X.LocalWidth();
        const T* XBuffer = X.LockedBuffer();
        T* YBuffer = Y.Buffer();
        const Int XLDim = X.LDim();
        const Int YLDim = Y.LDim();
        if( uplo == UPPER )
        {
            for( Int jLoc=0; jLoc<localWidth; ++jLoc )
            {
                const Int j = X.GlobalCol(jLoc);
                const Int localHeightAbove = X.LocalRowOffset(j+1);
                blas::Axpy
                ( localHeightAbove, alpha, 
                  &XBuffer[jLoc*XLDim], 1, &YBuffer[jLoc*YLDim], 1 );
            }
        }
        else
        {
            for( Int jLoc=0; jLoc<localWidth; ++jLoc )
            {
                const Int j = X.GlobalCol(jLoc);
                const Int localHeightAbove = X.LocalRowOffset(j);
                const Int localHeightBelow = localHeight - localHeightAbove;
                blas::Axpy
                ( localHeightBelow, alpha, 
                  &XBuffer[localHeightAbove+jLoc*XLDim], 1,
                  &YBuffer[localHeightAbove+jLoc*YLDim], 1 );
            }
        }
    }
    else
    {
        std::unique_ptr<AbstractDistMatrix<T>> XCopy( Y.Construct(X.Grid()) );
        XCopy->AlignWith( YDistData );
        Copy( X, *XCopy );
        AxpyTriangle( uplo, alpha, *XCopy, Y );
    }
}

#define PROTO_TYPES(T,S) \
  template void AxpyTriangle \
  ( UpperOrLower uplo, S alpha, const Matrix<T>& A, Matrix<T>& B ); \
  template void AxpyTriangle \
  ( UpperOrLower uplo, S alpha, \
    const AbstractDistMatrix<T>& A, AbstractDistMatrix<T>& B );

#define PROTO_INT(T) PROTO_TYPES(T,T)

#define PROTO_REAL(T) \
  PROTO_TYPES(T,Int) \
  PROTO_TYPES(T,T)

#define PROTO_COMPLEX(T) \
  PROTO_TYPES(T,Int) \
  PROTO_TYPES(T,Base<T>) \
  PROTO_TYPES(T,T)

#include "El/macros/Instantiate.h"

} // namespace El
