/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

#include "./HermitianTridiagEig/Sort.hpp"

// NOTE: eReal and ZReal could be packed into their complex counterparts

namespace El {

// Return eigenvalues
// ==================

namespace herm_tridiag_eig {

template<typename Real>
inline void Helper
( Matrix<Real>& d, Matrix<Real>& e, Matrix<Real>& w, SortType sort,
  const HermitianEigSubset<Real>& subset )
{
    const Int n = d.Height();
    w.Resize( n, 1 );
    if( subset.rangeSubset )
    {
         const Int k = lapack::SymmetricTridiagEig
          ( n, d.Buffer(), e.Buffer(), w.Buffer(), 
            subset.lowerBound, subset.upperBound );
         w.Resize( k, 1 );
    }
    else if( subset.indexSubset )
    {
        const Int numEig = subset.upperIndex-subset.lowerIndex+1;
        lapack::SymmetricTridiagEig
        ( n, d.Buffer(), e.Buffer(), w.Buffer(), 
          subset.lowerIndex, subset.upperIndex );
        w.Resize( numEig, 1 );
    }
    else
        lapack::SymmetricTridiagEig( n, d.Buffer(), e.Buffer(), w.Buffer() );
    Sort( w, sort );
}

template<typename Real>
inline void Helper
( Matrix<Real>& d, Matrix<Complex<Real>>& e, Matrix<Real>& w, SortType sort,
  const HermitianEigSubset<Real>& subset )
{
    typedef Complex<Real> C;
    const Int n = d.Height();
    Matrix<Real> eReal( n-1, 1 );
    C yLast = 1;
    for( Int j=0; j<n-1; ++j )
    {
        const C psi = e.Get(j,0);
        const Real psiAbs = Abs(psi);
        if( psiAbs == Real(0) )
            yLast = 1;
        else
            yLast = Polar(Real(1),Arg(psi*yLast));
        eReal.Set( j, 0, psiAbs );
    }
    HermitianTridiagEig( d, eReal, w, sort, subset );
}

} // namespace herm_tridiag_eig

template<typename F>
void HermitianTridiagEig
( Matrix<Base<F>>& d, Matrix<F>& e, Matrix<Base<F>>& w, SortType sort, 
  const HermitianEigSubset<Base<F>>& subset )
{
    DEBUG_ONLY(CallStackEntry cse("HermitianTridiagEig"))
    herm_tridiag_eig::Helper( d, e, w, sort, subset );
}

namespace herm_tridiag_eig {

template<typename Real,Dist U,Dist V,Dist X>
inline void Helper
( const DistMatrix<Real,U,V   >& d,
  const DistMatrix<Real,U,V   >& e,
        DistMatrix<Real,X,STAR>& w, SortType sort,
  const HermitianEigSubset<Real>& subset )
{
    const Int n = d.Height();

    DistMatrix<Real,STAR,STAR> d_STAR_STAR( d );
    DistMatrix<Real,STAR,STAR> e_STAR_STAR( d.Grid() );
    e_STAR_STAR.Resize( n-1, 1, n );
    e_STAR_STAR = e;

    if( w.Participating() )
    {
        Int k;
        if( subset.rangeSubset )
        {
            std::vector<Real> dVector(n), eVector(n), wVector(n);
            MemCopy( dVector.data(), d_STAR_STAR.Buffer(), n );
            MemCopy( eVector.data(), e_STAR_STAR.Buffer(), n-1 );
            auto estimate = herm_tridiag_eig::EigEstimate
              ( int(n), dVector.data(), eVector.data(), 
                wVector.data(), w.ColComm(),
                subset.lowerBound, subset.upperBound );
            SwapClear( dVector );
            SwapClear( eVector );
            k = estimate.numGlobalEigenvalues;
        }
        else if( subset.indexSubset )
            k = ( n==0 ? 0 : subset.upperIndex-subset.lowerIndex+1 );
        else
            k = n;
        w.AlignCols( 0 );
        w.Resize( k, 1 );

        std::vector<Real> wVector(n);
        herm_tridiag_eig::Info info;
        if( subset.rangeSubset )
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), e_STAR_STAR.Buffer(), 
                wVector.data(), w.ColComm(), 
                subset.lowerBound, subset.upperBound );
        else if( subset.indexSubset )
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), e_STAR_STAR.Buffer(), 
                wVector.data(), w.ColComm(), 
                int(subset.lowerIndex), int(subset.upperIndex) );
        else
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), e_STAR_STAR.Buffer(), 
                wVector.data(), w.ColComm() );
        w.Resize( info.numGlobalEigenvalues, 1 );
        for( Int iLoc=0; iLoc<w.LocalHeight(); ++iLoc )
            w.SetLocal( iLoc, 0, wVector[iLoc] );
    }
    w.MakeConsistent( true );
    Sort( w, sort );
}

template<typename Real,Dist U,Dist V,Dist X>
inline void Helper
( const DistMatrix<Real,         U,V   >& d,
  const DistMatrix<Complex<Real>,U,V   >& e,
        DistMatrix<Real,         X,STAR>& w, 
  SortType sort, const HermitianEigSubset<Real>& subset )
{
    typedef Complex<Real> C;
    const Int n = d.Height();

    DistMatrix<Real,STAR,STAR> d_STAR_STAR( d );
    DistMatrix<C,STAR,STAR> e_STAR_STAR( d.Grid() );
    e_STAR_STAR.Resize( n-1, 1, n );
    e_STAR_STAR = e;

    DistMatrix<Real,STAR,STAR> eReal( d.Grid() );
    eReal.Resize( n-1, 1, n );
    if( w.Participating() )
    {
        C yLast = 1;
        for( Int j=0; j<n-1; ++j )
        {
            const C psi = e_STAR_STAR.GetLocal(j,0);
            const Real psiAbs = Abs(psi);
            if( psiAbs == Real(0) )
                yLast = 1;
            else
                yLast = Polar(Real(1),Arg(psi*yLast));
            eReal.SetLocal( j, 0, psiAbs );
        }

        Int k;
        if( subset.rangeSubset )
        {
            std::vector<Real> dVector(n), eVector(n), wVector(n);
            MemCopy( dVector.data(), d_STAR_STAR.Buffer(), n );
            MemCopy( eVector.data(), eReal.Buffer(), n-1 );
            auto estimate = herm_tridiag_eig::EigEstimate
              ( int(n), dVector.data(), eVector.data(), 
                wVector.data(), w.ColComm(),
                subset.lowerBound, subset.upperBound );
            SwapClear( dVector );
            SwapClear( eVector );
            k = estimate.numGlobalEigenvalues;
        }
        else if( subset.indexSubset )
            k = ( n==0 ? 0 : subset.upperIndex-subset.lowerIndex+1 );
        else
            k = n;
        w.AlignCols( 0 );
        w.Resize( k, 1 );

        herm_tridiag_eig::Info info;
        std::vector<Real> wVector(n);
        if( subset.rangeSubset )
        {
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), eReal.Buffer(),
                wVector.data(), w.ColComm(),
                subset.lowerBound, subset.upperBound );
        }
        else if( subset.indexSubset )
        {
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), eReal.Buffer(),
                wVector.data(), w.ColComm(),
                int(subset.lowerIndex), int(subset.upperIndex) );
        }
        else
        {
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), eReal.Buffer(),
                wVector.data(), w.ColComm() );
        }
        w.Resize( info.numGlobalEigenvalues, 1 );
        for( Int iLoc=0; iLoc<w.LocalHeight(); ++iLoc )
            w.SetLocal( iLoc, 0, wVector[iLoc] );
    }
    w.MakeConsistent( true );
    Sort( w, sort );
}

} // namespace herm_tridiag_eig

template<typename F,Dist U,Dist V,Dist X>
void HermitianTridiagEig
( const DistMatrix<Base<F>,U,V   >& d,
  const DistMatrix<F,      U,V   >& e,
        DistMatrix<Base<F>,X,STAR>& w, 
  SortType sort, const HermitianEigSubset<Base<F>>& subset )
{
    DEBUG_ONLY(CallStackEntry cse("HermitianTridiagEig"))
    herm_tridiag_eig::Helper( d, e, w, sort, subset );
}

// Return eigenpairs
// =================

namespace herm_tridiag_eig {

template<typename Real>
inline void Helper
( Matrix<Real>& d, Matrix<Real>& e, Matrix<Real>& w, Matrix<Real>& Z,
  SortType sort, const HermitianEigSubset<Real>& subset )
{
    const Int n = d.Height();
    w.Resize( n, 1 );
    if( subset.rangeSubset )
    {
         Z.Resize( n, n );
         const Int k = lapack::SymmetricTridiagEig
          ( n, d.Buffer(), e.Buffer(), w.Buffer(), Z.Buffer(), Z.LDim(),
            subset.lowerBound, subset.upperBound );
         w.Resize( k, 1 );
         Z.Resize( n, k );
    }
    else if( subset.indexSubset )
    {
        const Int numEig = subset.upperIndex-subset.lowerIndex+1;
        Z.Resize( n, numEig );
        lapack::SymmetricTridiagEig
        ( n, d.Buffer(), e.Buffer(), w.Buffer(), Z.Buffer(), Z.LDim(),
          subset.lowerIndex, subset.upperIndex );
        w.Resize( numEig, 1 );
    }
    else
    {
        Z.Resize( n, n );
        lapack::SymmetricTridiagEig
        ( n, d.Buffer(), e.Buffer(), w.Buffer(), Z.Buffer(), Z.LDim() );
    }
    herm_eig::Sort( w, Z, sort );
}

template<typename Real>
inline void Helper
( Matrix<Real>& d, Matrix<Complex<Real>>& e, Matrix<Real>& w, 
  Matrix<Complex<Real>>& Z, SortType sort,
  const HermitianEigSubset<Real>& subset )
{
    typedef Complex<Real> C;
    const Int n = d.Height();
    Matrix<Real> eReal( n-1, 1 );
    Matrix<C> y( n, 1 );
    y.Set( 0, 0, 1 );
    for( Int j=0; j<n-1; ++j )
    {
        const C psi = e.Get(j,0);
        const Real psiAbs = Abs(psi);
        if( psiAbs == Real(0) )
            y.Set( j+1, 0, 1 );
        else
            y.Set( j+1, 0, Polar(Real(1),Arg(psi*y.Get(j,0))) );
        eReal.Set( j, 0, psiAbs );
    }
    Matrix<Real> ZReal;
    HermitianTridiagEig( d, eReal, w, ZReal, sort, subset );
    Z.Resize( n, ZReal.Width() );
    for( Int j=0; j<ZReal.Width(); ++j )
        for( Int i=0; i<n; ++i )
            Z.Set( i, j, y.Get(i,0)*ZReal.Get(i,j) );
}

} // namespace herm_tridiag_eig

template<typename F>
void HermitianTridiagEig
( Matrix<Base<F>>& d, Matrix<F>& e, Matrix<Base<F>>& w, Matrix<F>& Z, 
  SortType sort, const HermitianEigSubset<Base<F>>& subset )
{
    DEBUG_ONLY(CallStackEntry cse("HermitianTridiagEig"))
    herm_tridiag_eig::Helper( d, e, w, Z, sort, subset );
}

namespace herm_tridiag_eig {

template<typename Real,Dist U,Dist V,Dist X>
inline void Helper
( const DistMatrix<Real,U,  V   >& d,
  const DistMatrix<Real,U,  V   >& e,
        DistMatrix<Real,X,  STAR>& w, 
        DistMatrix<Real,STAR,X  >& Z, 
  SortType sort, const HermitianEigSubset<Real>& subset )
{
    const Int n = d.Height();

    DistMatrix<Real,STAR,STAR> d_STAR_STAR( d );
    DistMatrix<Real,STAR,STAR> e_STAR_STAR( d.Grid() );
    e_STAR_STAR.Resize( n-1, 1, n );
    e_STAR_STAR = e;
    if( w.Participating() )
    {
        Int k;
        if( subset.rangeSubset )
        {
            std::vector<Real> dVector(n), eVector(n), wVector(n);
            MemCopy( dVector.data(), d_STAR_STAR.Buffer(), n );
            MemCopy( eVector.data(), e_STAR_STAR.Buffer(), n-1 );
            auto estimate = herm_tridiag_eig::EigEstimate
              ( int(n), dVector.data(), eVector.data(),
                wVector.data(), w.ColComm(),
                subset.lowerBound, subset.upperBound );
            SwapClear( dVector );
            SwapClear( eVector );
            k = estimate.numGlobalEigenvalues;
        }
        else if( subset.indexSubset )
            k = ( n==0 ? 0 : subset.upperIndex-subset.lowerIndex+1 );
        else
            k = n;

        w.AlignCols( 0 );
        w.Resize( n, 1 );
        Z.AlignRows( 0 );
        Z.Resize( n, k );

        herm_tridiag_eig::Info info;
        std::vector<Real> wVector(n);
        if( subset.rangeSubset )
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), e_STAR_STAR.Buffer(), 
                wVector.data(), Z.Buffer(), Z.LDim(), w.ColComm(),
                subset.lowerBound, subset.upperBound );
        else if( subset.indexSubset )
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), e_STAR_STAR.Buffer(), 
                wVector.data(), Z.Buffer(), Z.LDim(), w.ColComm(),
                int(subset.lowerIndex), int(subset.upperIndex) );
        else
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), e_STAR_STAR.Buffer(), 
                wVector.data(), Z.Buffer(), Z.LDim(), w.ColComm() );
        w.Resize( info.numGlobalEigenvalues, 1 );
        Z.Resize( n, info.numGlobalEigenvalues );
        for( Int iLoc=0; iLoc<w.LocalHeight(); ++iLoc )
            w.SetLocal( iLoc, 0, wVector[iLoc] );
    }
    w.MakeConsistent( true );
    Z.MakeConsistent( true );
    herm_eig::Sort( w, Z, sort );
}

template<typename Real,Dist U,Dist V,Dist X>
inline void Helper
( const DistMatrix<Real,         U,  V   >& d,
  const DistMatrix<Complex<Real>,U,  V   >& e,
        DistMatrix<Real,         X,  STAR>& w, 
        DistMatrix<Complex<Real>,STAR,X  >& Z, 
  SortType sort, const HermitianEigSubset<Real>& subset )
{
    const Int n = d.Height();
    const Grid& g = d.Grid();
    typedef Complex<Real> C;

    DistMatrix<Real,STAR,STAR> d_STAR_STAR( d );
    DistMatrix<C,STAR,STAR> e_STAR_STAR(g);
    e_STAR_STAR.Resize( n-1, 1, n );
    e_STAR_STAR = e;
    DistMatrix<Real,STAR,X   > ZReal(g);
    DistMatrix<C,   STAR,STAR> y(n,1,g);

    DistMatrix<Real,STAR,STAR> eReal(g);
    eReal.Resize( n-1, 1, n );
    if( w.Participating() )
    {
        y.SetLocal(0,0,1);
        for( Int j=0; j<n-1; ++j )
        {
            const C psi = e_STAR_STAR.GetLocal(j,0);
            const Real psiAbs = Abs(psi);
            if( psiAbs == Real(0) )
                y.SetLocal( j+1, 0, 1 );
            else
                y.SetLocal( j+1, 0, Polar(Real(1),Arg(psi*y.GetLocal(j,0))) );
            eReal.SetLocal( j, 0, psiAbs );
        }

        Int k;
        if( subset.rangeSubset )
        {
            std::vector<Real> dVector(n), eVector(n), wVector(n);
            MemCopy( dVector.data(), d_STAR_STAR.Buffer(), n );
            MemCopy( eVector.data(), eReal.Buffer(), n-1 );
            auto estimate = herm_tridiag_eig::EigEstimate
              ( int(n), dVector.data(), eVector.data(),
                wVector.data(), w.ColComm(),
                subset.lowerBound, subset.upperBound );
            SwapClear( dVector );
            SwapClear( eVector );
            k = estimate.numGlobalEigenvalues;
        }
        else if( subset.indexSubset )
            k = ( n==0 ? 0 : subset.upperIndex-subset.lowerIndex+1 );
        else
            k = n;

        w.AlignCols( 0 );
        w.Resize( n, 1 );
        ZReal.AlignRows( 0 );
        ZReal.Resize( n, k );
        Z.AlignWith( ZReal );

        herm_tridiag_eig::Info info;
        std::vector<Real> wVector(n);
        if( subset.rangeSubset )
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), eReal.Buffer(), 
                wVector.data(), ZReal.Buffer(), ZReal.LDim(), w.ColComm(),
                subset.lowerBound, subset.upperBound );
        else if( subset.indexSubset )
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), eReal.Buffer(), 
                wVector.data(), ZReal.Buffer(), ZReal.LDim(), w.ColComm(),
                int(subset.lowerIndex), int(subset.upperIndex) );
        else
            info = herm_tridiag_eig::Eig
              ( int(n), d_STAR_STAR.Buffer(), eReal.Buffer(), 
                wVector.data(), ZReal.Buffer(), ZReal.LDim(), w.ColComm() );

        w.Resize( info.numGlobalEigenvalues, 1 );
        Z.Resize( n, info.numGlobalEigenvalues );
        ZReal.Resize( n, info.numGlobalEigenvalues );
        for( Int iLoc=0; iLoc<w.LocalHeight(); ++iLoc )
            w.SetLocal( iLoc, 0, wVector[iLoc] );
    }
    w.MakeConsistent( true );
    Z.MakeConsistent( true );
    ZReal.MakeConsistent( true );
    herm_eig::Sort( w, ZReal, sort );
    for( Int jLoc=0; jLoc<Z.LocalWidth(); ++jLoc )
        for( Int i=0; i<n; ++i )
            Z.SetLocal( i, jLoc, y.GetLocal(i,0)*ZReal.GetLocal(i,jLoc) );
}

} // namespace herm_tridiag_eig

template<typename F,Dist U,Dist V,Dist X>
void HermitianTridiagEig
( const DistMatrix<Base<F>,U,   V   >& d,
  const DistMatrix<F,      U,   V   >& e,
        DistMatrix<Base<F>,X,   STAR>& w, 
        DistMatrix<F,      STAR,X   >& Z, 
  SortType sort, const HermitianEigSubset<Base<F>>& subset )
{
    DEBUG_ONLY(CallStackEntry cse("HermitianTridiagEig"))
    herm_tridiag_eig::Helper( d, e, w, Z, sort, subset );
}

template<typename Real,Dist U,Dist V>
Int HermitianTridiagEigEstimate
( const DistMatrix<Real,U,V>& d,
  const DistMatrix<Real,U,V>& e,
        mpi::Comm wColComm, Real vl, Real vu )
{
    DEBUG_ONLY(CallStackEntry cse("HermitianTridiagEigEstimate"))
    const Int n = d.Height();
    DistMatrix<Real,STAR,STAR> d_STAR_STAR( d );
    DistMatrix<Real,STAR,STAR> e_STAR_STAR( d.Grid() );
    e_STAR_STAR.Resize( n-1, 1, n );
    e_STAR_STAR = e;
    std::vector<Real> dVector(n), eVector(n), wVector(n);
    MemCopy( dVector.data(), d_STAR_STAR.Buffer(), n );
    MemCopy( eVector.data(), e_STAR_STAR.Buffer(), n-1 );
    auto estimate = herm_tridiag_eig::EigEstimate
    ( int(n), dVector.data(), eVector.data(), wVector.data(), wColComm,
      vl, vu );
    return estimate.numGlobalEigenvalues;
}

// Z is assumed to be sufficiently large and properly aligned
template<typename Real,Dist U,Dist V,Dist X>
void HermitianTridiagEigPostEstimate
( const DistMatrix<Real,U,   V   >& d,
  const DistMatrix<Real,U,   V   >& e,
        DistMatrix<Real,X,   STAR>& w, 
        DistMatrix<Real,STAR,X   >& Z, 
  SortType sort, Real vl, Real vu )
{
    DEBUG_ONLY(CallStackEntry cse("HermitianTridiagEigPostEstimate"))
    const Int n = d.Height();
    w.AlignCols( 0 );
    if( Z.RowAlign() != 0 )
        LogicError("Z was not properly aligned");

    DistMatrix<Real,STAR,STAR> d_STAR_STAR( d );
    DistMatrix<Real,STAR,STAR> e_STAR_STAR( d.Grid() );
    e_STAR_STAR.Resize( n-1, 1, n );
    e_STAR_STAR = e;
    if( w.Participating() )
    {
        std::vector<Real> wVector(n);
        auto info = herm_tridiag_eig::Eig
        ( int(n), d_STAR_STAR.Buffer(), e_STAR_STAR.Buffer(), wVector.data(), 
          Z.Buffer(), Z.LDim(), w.ColComm(), vl, vu );
        const Int k = info.numGlobalEigenvalues;

        w.Resize( k, 1 );
        for( Int iLoc=0; iLoc<w.LocalHeight(); ++iLoc )
            w.SetLocal( iLoc, 0, wVector[iLoc] );
        Z.Resize( n, k );
    }
    w.MakeConsistent( true );
    Z.MakeConsistent( true );
    herm_eig::Sort( w, Z, sort );
}

#define PROTO_DIST_INNER(F,U,V,X) \
  template void HermitianTridiagEig \
  ( const DistMatrix<Base<F>,U,V   >& d, \
    const DistMatrix<F,      U,V   >& e, \
          DistMatrix<Base<F>,X,STAR>& w, SortType sort, \
    const HermitianEigSubset<Base<F>>& subset ); \
  template void HermitianTridiagEig \
  ( const DistMatrix<Base<F>,U,   V   >& d, \
    const DistMatrix<F,      U,   V   >& e, \
          DistMatrix<Base<F>,X,   STAR>& w, \
          DistMatrix<F,      STAR,X   >& Z, SortType sort, \
    const HermitianEigSubset<Base<F>>& subset );

#define PROTO_DIST(F,U,V) \
  PROTO_DIST_INNER(F,U,V,MC  ) \
  PROTO_DIST_INNER(F,U,V,MD  ) \
  PROTO_DIST_INNER(F,U,V,MR  ) \
  PROTO_DIST_INNER(F,U,V,STAR) \
  PROTO_DIST_INNER(F,U,V,VC  ) \
  PROTO_DIST_INNER(F,U,V,VR  ) \

#define PROTO_DIST_INNER_SORT(F,U,V,W,X) \
  template void herm_eig::Sort \
  ( DistMatrix<Base<F>,U,V>& w, DistMatrix<F,W,X>& Z, SortType sort );

#define PROTO_DIST_SORT(F,U,V) \
  PROTO_DIST_INNER_SORT(F,U,V,CIRC,CIRC) \
  PROTO_DIST_INNER_SORT(F,U,V,MC,  MR  ) \
  PROTO_DIST_INNER_SORT(F,U,V,MC,  STAR) \
  PROTO_DIST_INNER_SORT(F,U,V,MD,  STAR) \
  PROTO_DIST_INNER_SORT(F,U,V,MR,  MC  ) \
  PROTO_DIST_INNER_SORT(F,U,V,MR,  STAR) \
  PROTO_DIST_INNER_SORT(F,U,V,STAR,MC  ) \
  PROTO_DIST_INNER_SORT(F,U,V,STAR,MD  ) \
  PROTO_DIST_INNER_SORT(F,U,V,STAR,MR  ) \
  PROTO_DIST_INNER_SORT(F,U,V,STAR,STAR) \
  PROTO_DIST_INNER_SORT(F,U,V,STAR,VC  ) \
  PROTO_DIST_INNER_SORT(F,U,V,STAR,VR  ) \
  PROTO_DIST_INNER_SORT(F,U,V,VC,  STAR) \
  PROTO_DIST_INNER_SORT(F,U,V,VR,  STAR)

#define PROTO_DIST_REAL_INNER(Real,U,V,X) \
  template void HermitianTridiagEigPostEstimate \
  ( const DistMatrix<Real,U,   V   >& d, \
    const DistMatrix<Real,U,   V   >& e, \
          DistMatrix<Real,X,   STAR>& w, \
          DistMatrix<Real,STAR,X   >& Z, \
    SortType sort, Real vl, Real vu );

#define PROTO_DIST_REAL(Real,U,V) \
  template Int HermitianTridiagEigEstimate \
  ( const DistMatrix<Real,U,V>& d, \
    const DistMatrix<Real,U,V>& e, \
          mpi::Comm wColComm, Real vl, Real vu ); \
  PROTO_DIST_REAL_INNER(Real,U,V,MC  ) \
  PROTO_DIST_REAL_INNER(Real,U,V,MD  ) \
  PROTO_DIST_REAL_INNER(Real,U,V,MR  ) \
  PROTO_DIST_REAL_INNER(Real,U,V,STAR) \
  PROTO_DIST_REAL_INNER(Real,U,V,VC  ) \
  PROTO_DIST_REAL_INNER(Real,U,V,VR  )

#define PROTO_SORT(F) \
  template void herm_eig::Sort \
  ( Matrix<Base<F>>& w, Matrix<F>& Z, SortType sort ); \
  PROTO_DIST_SORT(F,CIRC,CIRC) \
  PROTO_DIST_SORT(F,MC,  MR  ) \
  PROTO_DIST_SORT(F,MC,  STAR) \
  PROTO_DIST_SORT(F,MD,  STAR) \
  PROTO_DIST_SORT(F,MR,  MC  ) \
  PROTO_DIST_SORT(F,STAR,MC  ) \
  PROTO_DIST_SORT(F,STAR,MD  ) \
  PROTO_DIST_SORT(F,STAR,MR  ) \
  PROTO_DIST_SORT(F,STAR,STAR) \
  PROTO_DIST_SORT(F,STAR,VC  ) \
  PROTO_DIST_SORT(F,STAR,VR  ) \
  PROTO_DIST_SORT(F,VC,  STAR) \
  PROTO_DIST_SORT(F,VR,  STAR)

#define PROTO_SEQEIG(F) \
  template void HermitianTridiagEig \
  ( Matrix<Base<F>>& d, Matrix<F>& e, Matrix<Base<F>>& w, SortType sort, \
    const HermitianEigSubset<Base<F>>& subset ); \
  template void HermitianTridiagEig \
  ( Matrix<Base<F>>& d, Matrix<F>& e, Matrix<Base<F>>& w, Matrix<F>& Z, \
    SortType sort, const HermitianEigSubset<Base<F>>& subset );

#define PROTO(F) \
  PROTO_SORT(F) \
  PROTO_SEQEIG(F) \
  PROTO_DIST(F,CIRC,CIRC) \
  PROTO_DIST(F,MC,  MR  ) \
  PROTO_DIST(F,MC,  STAR) \
  PROTO_DIST(F,MD,  STAR) \
  PROTO_DIST(F,MR,  MC  ) \
  PROTO_DIST(F,STAR,MC  ) \
  PROTO_DIST(F,STAR,MD  ) \
  PROTO_DIST(F,STAR,MR  ) \
  PROTO_DIST(F,STAR,STAR) \
  PROTO_DIST(F,STAR,VC  ) \
  PROTO_DIST(F,STAR,VR  ) \
  PROTO_DIST(F,VC,  STAR) \
  PROTO_DIST(F,VR,  STAR)

#define PROTO_REAL(Real) \
  PROTO(Real) \
  PROTO_DIST_REAL(Real,CIRC,CIRC) \
  PROTO_DIST_REAL(Real,MC,  MR  ) \
  PROTO_DIST_REAL(Real,MC,  STAR) \
  PROTO_DIST_REAL(Real,MD,  STAR) \
  PROTO_DIST_REAL(Real,MR,  MC  ) \
  PROTO_DIST_REAL(Real,STAR,MC  ) \
  PROTO_DIST_REAL(Real,STAR,MD  ) \
  PROTO_DIST_REAL(Real,STAR,MR  ) \
  PROTO_DIST_REAL(Real,STAR,STAR) \
  PROTO_DIST_REAL(Real,STAR,VC  ) \
  PROTO_DIST_REAL(Real,STAR,VR  ) \
  PROTO_DIST_REAL(Real,VC,  STAR) \
  PROTO_DIST_REAL(Real,VR,  STAR)

#define EL_NO_INT_PROTO
#define PROTO_FLOAT \
  PROTO_SORT(float) \
  PROTO_SEQEIG(float)
#define PROTO_DOUBLE  PROTO_REAL(double)
#define PROTO_COMPLEX_FLOAT \
  PROTO_SORT(Complex<float>) \
  PROTO_SEQEIG(Complex<float>)
#define PROTO_COMPLEX_DOUBLE PROTO(Complex<double>)
#include "El/macros/Instantiate.h"

} // namespace El
