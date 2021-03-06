/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

// NOTE: This is adapted from a MATLAB script written by AJ Friend.

namespace El {

template<typename Real>
Int SVM
( const Matrix<Real>& G, const Matrix<Real>& q, Matrix<Real>& w,
  Real gamma, Real rho, Int maxIter, bool inv, bool progress )
{
    DEBUG_ONLY(CallStackEntry cse("SVM"))
    const Int numExamples = G.Height();
    const Int numFeatures = G.Width();
    // A = [repmat(q,1,n).*G,q]
    // TODO: Add repmat support into Elemental? It's in Julia.
    Matrix<Real> A( numExamples, numFeatures+1 );
    auto AL = View( A, 0, 0, numExamples, numFeatures );
    AL = G;
    for( Int j=0; j<numFeatures; ++j )
        for( Int i=0; i<numExamples; ++i )
            AL.Set( i, j, AL.Get(i,j)*q.Get(i,0) );
    for( Int i=0; i<numExamples; ++i )
        A.Set( i, numFeatures, q.Get(i,0) );

    auto hingeProx = [=]( Matrix<Real>& y, Real rho )
                     { HingeLossProx( y, y.Height()*rho ); };
    auto frobProx = 
        [=]( Matrix<Real>& x, Real rho ) 
        { auto xT = View( x, 0, 0, x.Height()-1, 1 );
          FrobeniusProx( xT, gamma/rho ); };

    Matrix<Real> b;
    Zeros( b, numExamples, 1 );

    return ModelFit
    ( std::function<void(Matrix<Real>&,Real)>(hingeProx),
      std::function<void(Matrix<Real>&,Real)>(frobProx),
      A, b, w, rho, maxIter, inv, progress );
}

template<typename Real>
Int SVM
( const DistMatrix<Real>& G, const DistMatrix<Real>& q, DistMatrix<Real>& w, 
  Real gamma, Real rho, Int maxIter, bool inv, bool progress )
{
    DEBUG_ONLY(CallStackEntry cse("SVM"))
    const Int numExamples = G.Height();
    const Int numFeatures = G.Width();
    // A = [repmat(q,1,n).*G,q]
    // TODO: Add repmat support into Elemental? It's in Julia.
    DistMatrix<Real> A( numExamples, numFeatures+1, G.Grid() );
    auto AL = View( A, 0, 0, numExamples, numFeatures );
    AL = G;
    DistMatrix<Real,MC,STAR> q_MC_STAR(G.Grid());
    q_MC_STAR.AlignWith( A );
    q_MC_STAR = q;
    for( Int jLoc=0; jLoc<AL.LocalWidth(); ++jLoc )
        for( Int iLoc=0; iLoc<AL.LocalHeight(); ++iLoc )
            AL.SetLocal
            ( iLoc, jLoc, AL.GetLocal(iLoc,jLoc)*q_MC_STAR.GetLocal(iLoc,0) );
    if( A.IsLocalCol(numFeatures) )
    {
        const Int jLoc = A.LocalCol(numFeatures);
        for( Int iLoc=0; iLoc<A.LocalHeight(); ++iLoc )
            A.SetLocal( iLoc, jLoc, q_MC_STAR.GetLocal(iLoc,0) );
    }

    auto hingeProx = [=]( DistMatrix<Real>& y, Real rho )
                     { HingeLossProx( y, y.Height()*rho ); };
    auto frobProx =
        [=]( DistMatrix<Real>& x, Real rho )
        { auto xT = View( x, 0, 0, x.Height()-1, 1 );
          FrobeniusProx( xT, gamma/rho ); };

    DistMatrix<Real> b(G.Grid());
    Zeros( b, numExamples, 1 );

    return ModelFit
    ( std::function<void(DistMatrix<Real>&,Real)>(hingeProx),
      std::function<void(DistMatrix<Real>&,Real)>(frobProx),
      A, b, w, rho, maxIter, inv, progress );
}

#define PROTO(Real) \
  template Int SVM \
  ( const Matrix<Real>& G, const Matrix<Real>& q, Matrix<Real>& w, \
    Real gamma, Real rho, Int maxIter, bool inv, bool progress ); \
  template Int SVM \
  ( const DistMatrix<Real>& G, const DistMatrix<Real>& q, DistMatrix<Real>& w, \
    Real gamma, Real rho, Int maxIter, bool inv, bool progress );

#define EL_NO_INT_PROTO
#define EL_NO_COMPLEX_PROTO
#include "El/macros/Instantiate.h"

} // namepace elem
