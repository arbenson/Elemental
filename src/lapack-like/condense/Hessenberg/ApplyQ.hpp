/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_HESSENBERG_APPLYQ_HPP
#define EL_HESSENBERG_APPLYQ_HPP

namespace El {
namespace hessenberg {

template<typename F>
void ApplyQ
( LeftOrRight side, UpperOrLower uplo, Orientation orientation, 
  const Matrix<F>& A, const Matrix<F>& t, Matrix<F>& B )
{
    DEBUG_ONLY(CallStackEntry cse("hessenberg::ApplyQ"))
    const bool normal = (orientation==NORMAL);
    const bool onLeft = (side==LEFT);
    const ForwardOrBackward direction = ( normal==onLeft ? BACKWARD : FORWARD );
    if( uplo == LOWER )
    {
        const Conjugation conjugation = ( normal ? UNCONJUGATED : CONJUGATED );
        ApplyPackedReflectors
        ( side, UPPER, HORIZONTAL, direction, conjugation, 1, A, t, B );
    }
    else
    {
        const Conjugation conjugation = ( normal ? CONJUGATED : UNCONJUGATED );
        ApplyPackedReflectors
        ( side, LOWER, VERTICAL, direction, conjugation, -1, A, t, B );
    }
}

template<typename F>
void ApplyQ
( LeftOrRight side, UpperOrLower uplo, Orientation orientation, 
  const DistMatrix<F>& A, const DistMatrix<F,MD,STAR>& t, DistMatrix<F>& B )
{
    DEBUG_ONLY(CallStackEntry cse("hessenberg::ApplyQ"))
    const bool normal = (orientation==NORMAL);
    const bool onLeft = (side==LEFT);
    const ForwardOrBackward direction = ( normal==onLeft ? BACKWARD : FORWARD );
    if( uplo == LOWER )
    {
        const Conjugation conjugation = ( normal ? UNCONJUGATED : CONJUGATED );
        ApplyPackedReflectors
        ( side, UPPER, HORIZONTAL, direction, conjugation, 1, A, t, B );
    }
    else
    {
        const Conjugation conjugation = ( normal ? CONJUGATED : UNCONJUGATED );
        ApplyPackedReflectors
        ( side, LOWER, VERTICAL, direction, conjugation, -1, A, t, B );
    }
}

template<typename F>
void ApplyQ
( LeftOrRight side, UpperOrLower uplo, Orientation orientation, 
  const DistMatrix<F>& A, const DistMatrix<F,STAR,STAR>& t, DistMatrix<F>& B )
{
    DEBUG_ONLY(CallStackEntry cse("hessenberg::ApplyQ"))
    const Int offset = ( uplo==LOWER ? 1 : -1 );
    DistMatrix<F,MD,STAR> tDiag(A.Grid());
    tDiag.SetRoot( A.DiagonalRoot(offset) );
    tDiag.AlignCols( A.DiagonalAlign(offset) );
    tDiag = t;
    ApplyQ( side, uplo, orientation, A, tDiag, B );
}

} // namespace hessenberg
} // namespace El

#endif // ifndef EL_HESSENBERG_APPLYQ_HPP
