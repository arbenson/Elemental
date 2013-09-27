/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_LAPACK_LDL_PIVOTED_HPP
#define ELEM_LAPACK_LDL_PIVOTED_HPP

#include "elemental/blas-like/level1/Max.hpp"
#include "elemental/blas-like/level1/Scale.hpp"
#include "elemental/blas-like/level1/Swap.hpp"
#include "elemental/blas-like/level2/Syr.hpp"
#include "elemental/blas-like/level2/Trr.hpp"
#include "elemental/blas-like/level2/Trr2.hpp"
#include "elemental/matrices/Zeros.hpp"

// TODO: Reference LAPACK's dsytf2 and zhetf2

namespace elem {
namespace ldl {

// TODO: Add support for Algorithm D, Bunch-Parlett, etc.. 
// Currently using Algorithm A Bunch-Kaufman.
template<typename F>
inline Int
ChoosePivot( const Matrix<F>& A, LDLPivotType pivotType, BASE(F) gamma )
{
#ifndef RELEASE
    CallStackEntry cse("ldl::ChoosePivot");
#endif
    typedef BASE(F) Real;
    const Int n = A.Height();
    if( pivotType != BUNCH_KAUFMAN_A )
        LogicError("So far, only Bunch-Kaufman Algorithm A is supported");

    const Real alpha11Abs = Abs(A.Get(0,0));
    auto a21Pair = VectorMax( LockedViewRange(A,1,0,n,1) );
    const Int r = a21Pair.index + 1;
    const Real colMax = a21Pair.value;
    if( colMax == Real(0) && alpha11Abs == Real(0) )
        throw SingularMatrixException();

    if( alpha11Abs >= gamma*colMax )
        return 0;

    // Find maximum off-diag value in row r (exploit symmetry)
    auto leftPair   = VectorMax( LockedViewRange(A,r,  0,r+1,r  ) );
    auto bottomPair = VectorMax( LockedViewRange(A,r+1,r,n,  r+1) );
    const Real rowMax = Max(leftPair.value,bottomPair.value);

    if( alpha11Abs >= gamma*colMax*(colMax/rowMax) )
        return 0;

    if( Abs(A.Get(r,r)) >= gamma*rowMax )
        return r;

    // Default to a 2x2 pivot with 0 and r
    return -r;
}

template<typename F>
inline Int
ChoosePivot( const DistMatrix<F>& A, LDLPivotType pivotType, BASE(F) gamma )
{
#ifndef RELEASE
    CallStackEntry cse("ldl::ChoosePivot");
#endif
    typedef BASE(F) Real;
    const Int n = A.Height();
    if( pivotType != BUNCH_KAUFMAN_A )
        LogicError("So far, only Bunch-Kaufman Algorithm A is supported");

    const Real alpha11Abs = Abs(A.Get(0,0));
    auto a21Pair = VectorMax( LockedViewRange(A,1,0,n,1) );
    const Int r = a21Pair.index + 1;
    const Real colMax = a21Pair.value;
    if( colMax == Real(0) && alpha11Abs == Real(0) )
        throw SingularMatrixException();

    if( alpha11Abs >= gamma*colMax )
        return 0;

    // Find maximum off-diag value in row r (exploit symmetry)
    auto leftPair   = VectorMax( LockedViewRange(A,r,  0,r+1,r  ) );
    auto bottomPair = VectorMax( LockedViewRange(A,r+1,r,n,  r+1) );
    const Real rowMax = Max(leftPair.value,bottomPair.value);

    if( alpha11Abs >= gamma*colMax*(colMax/rowMax) )
        return 0;

    if( Abs(A.Get(r,r)) >= gamma*rowMax )
        return r;

    // Default to a 2x2 pivot with 0 and r
    return -r;
}

template<typename F>
inline Int
ChoosePivot
( const Matrix<F>& A, const Matrix<F>& X, const Matrix<F>& Y, 
  Int k, LDLPivotType pivotType, BASE(F) gamma )
{
#ifndef RELEASE
    CallStackEntry cse("ldl::ChoosePivot");
#endif
    typedef BASE(F) Real;
    const Int n = A.Height();
    if( pivotType != BUNCH_KAUFMAN_A )
        LogicError("So far, only Bunch-Kaufman Algorithm A is supported");

    auto aB1 = LockedViewRange( A, k, k, n, k+1 );
    auto zB1( aB1 );
    // A(k:n-1,k) -= X(k:n-1,0:k-1) Y(k,0:k-1)^T
    {
        auto XBL  = LockedViewRange( X, k, 0, n,   k );
        auto yRow = LockedViewRange( Y, k, 0, k+1, k );
        Gemv( NORMAL, F(-1), XBL, yRow, F(1), zB1 );
    } 

    const Real alpha11Abs = Abs(zB1.Get(0,0));
    auto a21Pair = VectorMax( LockedViewRange(zB1,1,0,n-k,1) );
    const Int r = a21Pair.index + (k+1);
    const Real colMax = a21Pair.value;
    if( colMax == Real(0) && alpha11Abs == Real(0) )
        throw SingularMatrixException();

    if( alpha11Abs >= gamma*colMax )
        return k;

    // Find maximum off-diag value in row r (exploit symmetry)
    auto aLeft   = LockedViewRange( A, r, k, r+1, r   );
    auto aBottom = LockedViewRange( A, r, r, n,   r+1 );
        
    auto zLeft( aLeft );
    auto zBottom( aBottom );
    auto zStrictBottom = ViewRange( zBottom, 1, 0, n-r, 1 );

    //
    // Update necessary components out-of-place
    //

    // A(r,k:r-1) -= X(r,0:k-1) Y(k:r-1,0:k-1)^T
    {
        auto xMid = LockedViewRange( X, r, 0, r+1, k );
        auto YBL = LockedViewRange( Y, k, 0, r, k );
        Gemv( NORMAL, F(-1), YBL, xMid, F(1), zLeft );
    }

    // A(r:n-1,r) -= X(r:n-1,0:k-1) Y(r,0:k-1)^T
    {
        auto XBL = LockedViewRange( X, r, 0, n, k );
        auto yRow = LockedViewRange( Y, r, 0, r+1, k );
        Gemv( NORMAL, F(-1), XBL, yRow, F(1), zBottom );
    } 

    auto leftPair   = VectorMax( zLeft );
    auto bottomPair = VectorMax( zStrictBottom );
    const Real rowMax = Max(leftPair.value,bottomPair.value);

    if( alpha11Abs >= gamma*colMax*(colMax/rowMax) )
        return k;

    if( Abs(zBottom.Get(0,0)) >= gamma*rowMax )
        return r;

    // Default to a 2x2 pivot with k and r
    return -r;
}

// A := A inv(D)
template<typename F>
inline void
SolveAgainstSymmetric2x2
( UpperOrLower uplo, const Matrix<F>& D, Matrix<F>& A, bool conjugated=false )
{
#ifndef RELEASE    
    CallStackEntry cse("SolveAgainstSymmetric2x2");
    if( A.Width() != 2 )
        LogicError("A must have width 2");
#endif
    const Int m = A.Height();
    if( uplo == LOWER )
    {
        if( conjugated )     
        {
            const F delta11 = D.GetRealPart(0,0);
            const F delta21 = D.Get(1,0);
            const F delta22 = D.GetRealPart(1,1);
            const F delta21Abs = SafeAbs( delta21 );
            const F phi21To11 = delta22 / delta21Abs;
            const F phi21To22 = delta11 / delta21Abs;
            const F phi21 = delta21 / delta21Abs;
            const F xi = (F(1)/(phi21To11*phi21To22-F(1)))/delta21Abs;

            for( Int j=0; j<m; ++j )
            {
                const F eta0 = xi*(phi21To11*A.Get(j,0)-phi21      *A.Get(j,1));
                const F eta1 = xi*(phi21To22*A.Get(j,1)-Conj(phi21)*A.Get(j,0));
                A.Set( j, 0, eta0 );
                A.Set( j, 1, eta1 );
            }
        }
        else
        {
            const F delta11 = D.Get(0,0);
            const F delta21 = D.Get(1,0);
            const F delta22 = D.Get(1,1);
            const F chi21To11 = -delta22 / delta21;
            const F chi21To22 = -delta11 / delta21;
            const F chi21 = (F(1)/(F(1)-chi21To11*chi21To22))/delta21;

            for( Int j=0; j<m; ++j )
            {
                const F eta0 = chi21*(chi21To11*A.Get(j,0)+A.Get(j,1));
                const F eta1 = chi21*(chi21To22*A.Get(j,1)+A.Get(j,0));
                A.Set( j, 0, eta0 );
                A.Set( j, 1, eta1 );
            }
        }
    }
    else
        LogicError("This option not yet supported");
}

template<typename F>
inline void
SolveAgainstSymmetric2x2
( UpperOrLower uplo, 
  const DistMatrix<F>& D, DistMatrix<F>& A, bool conjugated=false )
{
#ifndef RELEASE    
    CallStackEntry cse("SolveAgainstSymmetric2x2");
    if( A.Width() != 2 )
        LogicError("A must have width 2");
#endif
    DistMatrix<F,STAR,STAR> D_STAR_STAR( D );

    const Int mLocal = A.LocalHeight();
    const bool inFirstCol = ( A.RowRank() == A.ColOwner(0) );
    const bool inSecondCol = ( A.RowRank() == A.ColOwner(1) );
    if( !inFirstCol && !inSecondCol )
        return;

    F *ALocCol0, *ALocCol1;
    std::vector<F> buffer;
    {
        if( inFirstCol && inSecondCol )
        {
            ALocCol0 = A.Buffer(0,0);
            ALocCol1 = A.Buffer(0,1);
        }
        else if( inFirstCol )
        {
            buffer.resize( mLocal );
            ALocCol0 = A.Buffer();
            ALocCol1 = buffer.data();
            mpi::SendRecv
            ( ALocCol0, mLocal, A.ColOwner(1),
              ALocCol1, mLocal, A.ColOwner(1), A.RowComm() );
        }
        else if( inSecondCol )
        {
            buffer.resize( mLocal ); 
            ALocCol0 = buffer.data();
            ALocCol1 = A.Buffer();
            mpi::SendRecv
            ( ALocCol1, mLocal, A.ColOwner(0),
              ALocCol0, mLocal, A.ColOwner(0), A.RowComm() );
        }
    }
    if( uplo == LOWER )
    {
        if( conjugated )     
        {
            const F delta11 = D_STAR_STAR.GetLocalRealPart(0,0);
            const F delta21 = D_STAR_STAR.GetLocal(1,0);
            const F delta22 = D_STAR_STAR.GetLocalRealPart(1,1);
            const F delta21Abs = SafeAbs( delta21 );
            const F phi21To11 = delta22 / delta21Abs;
            const F phi21To22 = delta11 / delta21Abs;
            const F phi21 = delta21 / delta21Abs;
            const F xi = (F(1)/(phi21To11*phi21To22-F(1)))/delta21Abs;

            for( Int iLoc=0; iLoc<mLocal; ++iLoc )
            {
                const F alpha0 = ALocCol0[iLoc];
                const F alpha1 = ALocCol1[iLoc]; 
                ALocCol0[iLoc] = xi*(phi21To11*alpha0-phi21      *alpha1);
                ALocCol1[iLoc] = xi*(phi21To22*alpha1-Conj(phi21)*alpha0);
            }
        }
        else
        {
            const F delta11 = D_STAR_STAR.GetLocal(0,0);
            const F delta21 = D_STAR_STAR.GetLocal(1,0);
            const F delta22 = D_STAR_STAR.GetLocal(1,1);
            const F chi21To11 = -delta22 / delta21;
            const F chi21To22 = -delta11 / delta21;
            const F chi21 = (F(1)/(F(1)-chi21To11*chi21To22))/delta21;

            for( Int iLoc=0; iLoc<mLocal; ++iLoc )
            {
                const F alpha0 = ALocCol0[iLoc];
                const F alpha1 = ALocCol1[iLoc]; 
                ALocCol0[iLoc] = chi21*(chi21To11*alpha0+alpha1);
                ALocCol1[iLoc] = chi21*(chi21To22*alpha1+alpha0);
            }
        }
    }
    else
        LogicError("This option not yet supported");
}

// Unblocked sequential pivoted LDL
template<typename F>
inline void
UnblockedPivoted
( Orientation orientation, Matrix<F>& A, Matrix<Int>& p, 
  LDLPivotType pivotType=BUNCH_KAUFMAN_A,
  BASE(F) gamma=(1+Sqrt(BASE(F)(17)))/8 )
{
#ifndef RELEASE
    CallStackEntry entry("ldl::UnblockedPivoted");
    if( A.Height() != A.Width() )
        LogicError("A must be square");
    if( orientation == NORMAL )
        LogicError("Can only perform LDL^T or LDL^H");
#endif
    const bool conjugate = ( orientation==ADJOINT );
    const Int n = A.Height();
    p.ResizeTo( n, 1 );

    Matrix<F> Y21;

    Int k=0;
    while( k < n )
    {
        // Determine the pivot (block)
        auto ABR = LockedViewRange( A, k, k, n, n );
        const Int pivot = ChoosePivot( ABR, pivotType, gamma );
        const Int nb   = ( pivot >= 0 ? 1       : 2      );
        const Int from = ( pivot >= 0 ? k+pivot : k-pivot );
        const Int to = k + (nb-1);

        // Apply the symmetric pivot
        SymmetricSwap( LOWER, A, to, from, conjugate );
        if( conjugate )
        {
            // Force the active diagonal entries to be real
            A.MakeReal( k,    k    );
            A.MakeReal( to,   to   );
            A.MakeReal( from, from );
        }

        // Update trailing submatrix and store pivots
        if( nb == 1 )
        {
            // Rank-one update: A22 -= a21 inv(delta11) a21'
            const F delta11Inv = F(1)/ABR.Get(0,0);
            auto a21 = ViewRange( ABR, 1, 0, n-k, 1   );
            auto A22 = ViewRange( ABR, 1, 1, n-k, n-k );
            Syr( LOWER, -delta11Inv, a21, A22, conjugate );
            Scale( delta11Inv, a21 );

            p.Set( k, 0, from );
        }
        else
        {
            // Rank-two update: A22 -= A21 inv(D11) A21'
            auto D11 = ViewRange( ABR, 0, 0, 2,   2   );
            auto A21 = ViewRange( ABR, 2, 0, n-k, 2   );
            auto A22 = ViewRange( ABR, 2, 2, n-k, n-k );
            Y21 = A21;
            SolveAgainstSymmetric2x2( LOWER, D11, A21, conjugate );
            Trr2( LOWER, F(-1), A21, Y21, A22, conjugate );

            p.Set( k,   0, -from );
            p.Set( k+1, 0, -from );
        }

        k += nb;
    }
}

template<typename F>
inline void
UnblockedPivoted
( Orientation orientation, DistMatrix<F>& A, DistMatrix<Int,VC,STAR>& p, 
  LDLPivotType pivotType=BUNCH_KAUFMAN_A,
  BASE(F) gamma=(1+Sqrt(BASE(F)(17)))/8 )
{
#ifndef RELEASE
    CallStackEntry entry("ldl::UnblockedPivoted");
    if( A.Height() != A.Width() )
        LogicError("A must be square");
    if( A.Grid() != p.Grid() )
        LogicError("A and p must share the same grid");
    if( orientation == NORMAL )
        LogicError("Can only perform LDL^T or LDL^H");
#endif
    const bool conjugate = ( orientation==ADJOINT );
    const Int n = A.Height();
    p.ResizeTo( n, 1 );

    DistMatrix<F> Y21( A.Grid() );

    Int k=0;
    while( k < n )
    {
        // Determine the pivot (block)
        auto ABR = LockedViewRange( A, k, k, n, n );
        const Int pivot = ChoosePivot( ABR, pivotType, gamma );
        const Int nb   = ( pivot >= 0 ? 1       : 2      );
        const Int from = ( pivot >= 0 ? k+pivot : k-pivot );
        const Int to = k + (nb-1);

        // Apply the symmetric pivot
        SymmetricSwap( LOWER, A, to, from, conjugate );
        if( conjugate )
        {
            // Force the active diagonal entries to be real
            A.MakeReal( k,    k    );
            A.MakeReal( to,   to   );
            A.MakeReal( from, from );
        }

        // Update trailing submatrix and store pivots
        if( nb == 1 )
        {
            // Rank-one update: A22 -= a21 inv(delta11) a21'
            const F delta11Inv = F(1)/ABR.Get(0,0);
            auto a21 = ViewRange( ABR, 1, 0, n-k, 1   );
            auto A22 = ViewRange( ABR, 1, 1, n-k, n-k );
            Syr( LOWER, -delta11Inv, a21, A22, conjugate );
            Scale( delta11Inv, a21 );

            p.Set( k, 0, from );
        }
        else
        {
            // Rank-two update: A22 -= A21 inv(D11) A21'
            auto D11 = ViewRange( ABR, 0, 0, 2,   2   );
            auto A21 = ViewRange( ABR, 2, 0, n-k, 2   );
            auto A22 = ViewRange( ABR, 2, 2, n-k, n-k );
            Y21 = A21;
            SolveAgainstSymmetric2x2( LOWER, D11, A21, conjugate );
            Trr2( LOWER, F(-1), A21, Y21, A22, conjugate );

            p.Set( k,   0, -from );
            p.Set( k+1, 0, -from );
        }

        k += nb;
    }
}

// We must use a lazy algorithm so that the symmetric pivoting does not move
// data from a fully-updated to partially-updated region (and vice-versa)
template<typename F>
inline void
PanelPivoted
( Orientation orientation, Matrix<F>& A, Matrix<Int>& p, 
  Matrix<F>& X, Matrix<F>& Y, Int bsize, Int off=0,
  LDLPivotType pivotType=BUNCH_KAUFMAN_A,
  BASE(F) gamma=(1+Sqrt(BASE(F)(17)))/8 )
{
#ifndef RELEASE
    CallStackEntry entry("ldl::PanelPivoted");
#endif
    const Int n = A.Height();
#ifndef RELEASE
    if( A.Width() != n )
        LogicError("A must be square");
    if( p.Height() != n || p.Width() != 1 )
        LogicError("pivot vector is the wrong size");
    if( orientation == NORMAL )
        LogicError("Can only perform LDL^T or LDL^H");
#endif
    const bool conjugate = ( orientation==ADJOINT );
    auto ABR = LockedViewRange( A, off, off, n, n );
    Zeros( X, n-off, bsize );
    Zeros( Y, n-off, bsize );

    Int k=0;
    while( k < bsize )
    {
        // Determine the pivot (block)
        const Int pivot = ChoosePivot( ABR, X, Y, k, pivotType, gamma );
        const Int nb   = ( pivot >= 0 ? 1         : 2         );
        const Int from = ( pivot >= 0 ? off+pivot : off-pivot );
        const Int to = (off+k) + (nb-1);
        if( k+nb > bsize )
        {
            X.ResizeTo( n-off, bsize-1 );
            Y.ResizeTo( n-off, bsize-1 );
            break;
        }

        // Apply the symmetric pivot
        SymmetricSwap( LOWER, A, to, from, conjugate );
        RowSwap( X, to-off, from-off );
        RowSwap( Y, to-off, from-off );

        // Update the active columns and then store the new update factors
        // TODO: Reuse updates from pivot selection where possible
        if( nb == 1 ) 
        {
            // Update ABR(k:end,k) -= X(k:n-off-1,0:k-1) Y(k,0:k-1)^T
            auto XB0 = LockedViewRange( X,   k, 0, n-off, k   );
            auto y10 = LockedViewRange( Y,   k, 0, k+1,   k   );
            auto aB1 =       ViewRange( ABR, k, k, n-off, k+1 );
            Gemv( NORMAL, F(-1), XB0, y10, F(1), aB1 );

            // Store x21 := a21/delta11 and y21 := a21
            const F delta11Inv = F(1)/ABR.Get(k,k);
            auto a21 = ViewRange( ABR, k+1, k, n-off, k+1 );
            auto x21 = ViewRange( X,   k+1, k, n-off, k+1 );
            auto y21 = ViewRange( Y,   k+1, k, n-off, k+1 );
            if( conjugate )
                Conjugate( a21, y21 );
            else
                y21 = a21;
            Scale( delta11Inv, a21 );
            x21 = a21;

            p.Set( off+k, 0, from );
        }
        else
        {
            // Update ABR(k:end,k:k+1) -= X(k:n-off-1,0:k-1) Y(k:k+1,0:k-1)^T
            // NOTE: top-right entry of AB1 is above-diagonal
            auto XB0 = LockedViewRange( X,   k, 0, n-off, k   );
            auto Y10 = LockedViewRange( Y,   k, 0, k+2,   k   );
            auto AB1 =       ViewRange( ABR, k, k, n-off, k+2 );
            const F psi = AB1.Get(0,1);
            Gemm( NORMAL, TRANSPOSE, F(-1), XB0, Y10, F(1), AB1 );
            AB1.Set(0,1,psi);

            // Store X21 := A21/D11 and Y21 := A21 or Y21 := Conj(A21)
            auto D11 = ViewRange( ABR, k,   k, k+2,   k+2 );
            auto A21 = ViewRange( ABR, k+2, k, n-off, k+2 );
            auto X21 = ViewRange( X,   k+2, k, n-off, k+2 );
            auto Y21 = ViewRange( Y,   k+2, k, n-off, k+2 );
            if( conjugate )
                Conjugate( A21, Y21 );
            else
                Y21 = A21;
            SolveAgainstSymmetric2x2( LOWER, D11, A21, conjugate );
            X21 = A21;

            p.Set( off+k,   0, -from );
            p.Set( off+k+1, 0, -from );
        }

        if( conjugate )
        {
            // Force the active diagonal entries to be real
            A.MakeReal( off+k, off+k );
            A.MakeReal( to,    to    );
            A.MakeReal( from,  from  );
        }

        k += nb;
    }
}

template<typename F>
inline void
Pivoted
( Orientation orientation, Matrix<F>& A, Matrix<Int>& p, 
  LDLPivotType pivotType=BUNCH_KAUFMAN_A,
  BASE(F) gamma=(1+Sqrt(BASE(F)(17)))/8 )
{
#ifndef RELEASE
    CallStackEntry entry("ldl::Pivoted");
    if( A.Height() != A.Width() )
        LogicError("A must be square");
    if( orientation == NORMAL )
        LogicError("Can only perform LDL^T or LDL^H");
#endif
    const Int n = A.Height();
    p.ResizeTo( n, 1 );

    Matrix<F> X, Y;
    const Int bsize = Blocksize();
    Int k=0;
    while( k < n )
    {
        const Int nbProp = Min(bsize,n-k);
        PanelPivoted( orientation, A, p, X, Y, nbProp, k, pivotType, gamma );
        const Int nb = X.Width();

        // Update the bottom-right panel
        auto X21B  = ViewRange( X, nb,   0,    n-k, nb );
        auto Y21B  = ViewRange( Y, nb,   0,    n-k, nb );
        auto A22BR = ViewRange( A, k+nb, k+nb, n,   n  );
        Trrk( LOWER, NORMAL, TRANSPOSE, F(-1), X21B, Y21B, F(1), A22BR );

        k += nb;
    }
}

template<typename F>
inline void
Pivoted
( Orientation orientation, DistMatrix<F>& A, DistMatrix<Int,VC,STAR>& p, 
  LDLPivotType pivotType=BUNCH_KAUFMAN_A,
  BASE(F) gamma=(1+Sqrt(BASE(F)(17)))/8 )
{
#ifndef RELEASE
    CallStackEntry entry("ldl::Pivoted"); 
#endif
    // TODO: Distributed blocked implementation
    UnblockedPivoted( orientation, A, p, pivotType, gamma );
}

} // namespace ldl
} // namespace elem

#endif // ifndef ELEM_LAPACK_LDL_PIVOTED_HPP