/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_VIEWS_SLIDEPARTITION_HPP
#define EL_VIEWS_SLIDEPARTITION_HPP

#include "./View.hpp"

namespace El {

// To make our life easier. Undef'd at the bottom of the header
#define M   Matrix<T>
#define ADM AbstractDistMatrix<T>
#define DM  DistMatrix<T,U,V>

// Slide a partition upward
// ========================

template<typename T>
inline void
SlidePartitionUp
( M& AT, M& A0,
         M& A1,
  M& AB, M& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionUp"))
    View( AT, A0 );
    View2x1( AB, A1, A2 );
}

template<typename T>
inline void
SlidePartitionUp
( ADM& AT, ADM& A0,
           ADM& A1,
  ADM& AB, ADM& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionUp"))
    View( AT, A0 );
    View2x1( AB, A1, A2 );
}

template<typename T>
inline void
SlideLockedPartitionUp
( M& AT, const M& A0,
         const M& A1,
  M& AB, const M& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionUp"))
    LockedView( AT, A0 );
    LockedView2x1( AB, A1, A2 );
}

template<typename T>
inline void
SlideLockedPartitionUp
( ADM& AT, const ADM& A0,
           const ADM& A1,
  ADM& AB, const ADM& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionUp"))
    LockedView( AT, A0 );
    LockedView2x1( AB, A1, A2 );
}

// Slide a partition downward
// ==========================

template<typename T>
inline void
SlidePartitionDown
( M& AT, M& A0,
         M& A1,
  M& AB, M& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionDown"))
    View2x1( AT, A0, A1 );
    View( AB, A2 );
}

template<typename T>
inline void
SlidePartitionDown
( ADM& AT, ADM& A0,
           ADM& A1,
  ADM& AB, ADM& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionDown"))
    View2x1( AT, A0, A1 );
    View( AB, A2 );
}

template<typename T>
inline void
SlideLockedPartitionDown
( M& AT, const M& A0,
         const M& A1,
  M& AB, const M& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionDown"))
    LockedView2x1( AT, A0, A1 );
    LockedView( AB, A2 );
}

template<typename T>
inline void
SlideLockedPartitionDown
( ADM& AT, const ADM& A0,
           const ADM& A1,
  ADM& AB, const ADM& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionDown"))
    LockedView2x1( AT, A0, A1 );
    LockedView( AB, A2 );
}

// HERE

// Slide a partition leftward
// ==========================

template<typename T>
inline void
SlidePartitionLeft
( M& AL, M& AR,
  M& A0, M& A1, M& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionLeft"))
    View( AL, A0 );
    View1x2( AR, A1, A2 );
}

template<typename T,Dist U,Dist V>
inline void
SlidePartitionLeft
( DM& AL, DM& AR,
  DM& A0, DM& A1, DM& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionLeft"))
    View( AL, A0 );
    View1x2( AR, A1, A2 );
}

template<typename T>
inline void
SlideLockedPartitionLeft
( M& AL, M& AR,
  const M& A0, const M& A1, const M& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionLeft"))
    LockedView( AL, A0 );
    LockedView1x2( AR, A1, A2 );
}

template<typename T,Dist U,Dist V>
inline void
SlideLockedPartitionLeft
( DM& AL, DM& AR,
  const DM& A0, const DM& A1, const DM& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionLeft"))
    LockedView( AL, A0 );
    LockedView1x2( AR, A1, A2 );
}

// Slide a partition rightward
// ===========================

template<typename T>
inline void
SlidePartitionRight
( M& AL, M& AR,
  M& A0, M& A1, M& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionRight"))
    View1x2( AL, A0, A1 );
    View( AR, A2 );
}

template<typename T,Dist U,Dist V>
inline void
SlidePartitionRight
( DM& AL, DM& AR,
  DM& A0, DM& A1, DM& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionRight"))
    View1x2( AL, A0, A1 );
    View( AR, A2 );
}

template<typename T>
inline void
SlideLockedPartitionRight
( M& AL, M& AR,
  const M& A0, const M& A1, const M& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionRight"))
    LockedView1x2( AL, A0, A1 );
    LockedView( AR, A2 );
}

template<typename T,Dist U,Dist V>
inline void
SlideLockedPartitionRight
( DM& AL, DM& AR,
  const DM& A0, const DM& A1, const DM& A2 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionRight"))
    LockedView1x2( AL, A0, A1 );
    LockedView( AR, A2 );
}

// Slide a partition upward on a diagonal
// ======================================

template<typename T>
inline void
SlidePartitionUpDiagonal
( M& ATL, M& ATR, M& A00, M& A01, M& A02,
                  M& A10, M& A11, M& A12,
  M& ABL, M& ABR, M& A20, M& A21, M& A22 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionUpDiagonal"))
    View( ATL, A00 );
    View1x2( ATR, A01, A02 );
    View2x1( ABL, A10, A20 );
    View2x2( ABR, A11, A12,
                  A21, A22 );
}

template<typename T,Dist U,Dist V>
inline void
SlidePartitionUpDiagonal
( DM& ATL, DM& ATR, DM& A00, DM& A01, DM& A02,
                    DM& A10, DM& A11, DM& A12,
  DM& ABL, DM& ABR, DM& A20, DM& A21, DM& A22 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionUpDiagonal"))
    View( ATL, A00 );
    View1x2( ATR, A01, A02 );
    View2x1( ABL, A10, A20 );
    View2x2( ABR, A11, A12,
                  A21, A22 );
}

template<typename T>
inline void
SlideLockedPartitionUpDiagonal
( M& ATL, M& ATR, const M& A00, const M& A01, const M& A02,
                  const M& A10, const M& A11, const M& A12,
  M& ABL, M& ABR, const M& A20, const M& A21, const M& A22 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionUpDiagonal"))
    LockedView( ATL, A00 );
    LockedView1x2( ATR, A01, A02 );
    LockedView2x1( ABL, A10, A20 );
    LockedView2x2( ABR, A11, A12,
                        A21, A22 );
}

template<typename T,Dist U,Dist V>
inline void
SlideLockedPartitionUpDiagonal
( DM& ATL, DM& ATR, const DM& A00, const DM& A01, const DM& A02,
                    const DM& A10, const DM& A11, const DM& A12,
  DM& ABL, DM& ABR, const DM& A20, const DM& A21, const DM& A22 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionUpDiagonal"))
    LockedView( ATL, A00 );
    LockedView1x2( ATR, A01, A02 );
    LockedView2x1( ABL, A10, A20 );
    LockedView2x2( ABR, A11, A12,
                        A21, A22 );
}

// Slide a partition downward on a diagonal
// ========================================

template<typename T>
inline void
SlidePartitionDownDiagonal
( M& ATL, M& ATR, M& A00, M& A01, M& A02,
                  M& A10, M& A11, M& A12,
  M& ABL, M& ABR, M& A20, M& A21, M& A22 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionDownDiagonal"))
    View2x2( ATL, A00, A01,
                  A10, A11 );
    View2x1( ATR, A02, A12 );
    View1x2( ABL, A20, A21 );
    View( ABR, A22 );
}

template<typename T,Dist U,Dist V>
inline void
SlidePartitionDownDiagonal
( DM& ATL, DM& ATR, DM& A00, DM& A01, DM& A02,
                    DM& A10, DM& A11, DM& A12,
  DM& ABL, DM& ABR, DM& A20, DM& A21, DM& A22 )
{
    DEBUG_ONLY(CallStackEntry cse("SlidePartitionDownDiagonal"))
    View2x2( ATL, A00, A01,
                  A10, A11 );
    View2x1( ATR, A02, A12 );
    View1x2( ABL, A20, A21 );
    View( ABR, A22 );
}

template<typename T>
inline void
SlideLockedPartitionDownDiagonal
( M& ATL, M& ATR, const M& A00, const M& A01, const M& A02,
                  const M& A10, const M& A11, const M& A12,
  M& ABL, M& ABR, const M& A20, const M& A21, const M& A22 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionDownDiagonal"))
    LockedView2x2( ATL, A00, A01,
                        A10, A11 );
    LockedView2x1( ATR, A02, A12 );
    LockedView1x2( ABL, A20, A21 );
    LockedView( ABR, A22 );
}

template<typename T,Dist U,Dist V>
inline void
SlideLockedPartitionDownDiagonal
( DM& ATL, DM& ATR, const DM& A00, const DM& A01, const DM& A02,
                    const DM& A10, const DM& A11, const DM& A12,
  DM& ABL, DM& ABR, const DM& A20, const DM& A21, const DM& A22 )
{
    DEBUG_ONLY(CallStackEntry cse("SlideLockedPartitionDownDiagonal"))
    LockedView2x2( ATL, A00, A01,
                        A10, A11 );
    LockedView2x1( ATR, A02, A12 );
    LockedView1x2( ABL, A20, A21 );
    LockedView( ABR, A22 );
}

#undef DM
#undef ADM
#undef M

} // namespace El

#endif // ifndef EL_VIEWS_SLIDEPARTITION_HPP
