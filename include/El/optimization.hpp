/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_OPTIMIZATION_HPP
#define EL_OPTIMIZATION_HPP

namespace El {

namespace RegularizationNS {
enum Regularization {
  NO_PENALTY,
  L1_PENALTY,
  L2_PENALTY
};
}
using namespace RegularizationNS;

// Basis pursuit: min || z ||_1 such that A z = b
// ==============================================
template<typename F>
Int BasisPursuit
( const Matrix<F>& A, const Matrix<F>& b,
  Matrix<F>& z,
  Base<F> rho=1., Base<F> alpha=1.2, Int maxIter=500, Base<F> absTol=1e-6, 
  Base<F> relTol=1e-4, bool usePinv=false, Base<F> pinvTol=0, 
  bool progress=true );
template<typename F>
Int BasisPursuit
( const DistMatrix<F>& A, const DistMatrix<F>& b,
  DistMatrix<F>& z,
  Base<F> rho=1., Base<F> alpha=1.2, Int maxIter=500, Base<F> absTol=1e-6, 
  Base<F> relTol=1e-4, bool usePinv=false, Base<F> pinvTol=0,
  bool progress=true );

// Least Absolute Shrinkage and Selection Operator (LASSO)
// =======================================================
template<typename F>
Int Lasso
( const Matrix<F>& A, const Matrix<F>& b, Base<F> lambda,
  Matrix<F>& z,
  Base<F> rho=1, Base<F> alpha=1.2, Int maxIter=500, Base<F> absTol=1e-6, 
  Base<F> relTol=1e-4, bool inv=true, bool progress=true );
template<typename F>
Int Lasso
( const DistMatrix<F>& A, const DistMatrix<F>& b, Base<F> lambda,
  DistMatrix<F>& z,
  Base<F> rho=1, Base<F> alpha=1.2, Int maxIter=500, Base<F> absTol=1e-6, 
  Base<F> relTol=1e-4, bool inv=true, bool progress=true );

// Linear program
// ==============
template<typename Real>
Int LinearProgram
( const Matrix<Real>& A, const Matrix<Real>& b, const Matrix<Real>& c,
  Matrix<Real>& z,
  Real rho=1., Real alpha=1.2, Int maxIter=500,
  Real absTol=1e-6, Real relTol=1e-4, bool inv=false, bool progress=true );
template<typename Real>
Int LinearProgram
( const DistMatrix<Real>& A, const DistMatrix<Real>& b,
  const DistMatrix<Real>& c, DistMatrix<Real>& z,
  Real rho=1, Real alpha=1.2, Int maxIter=500, Real absTol=1e-6,
  Real relTol=1e-4, bool inv=true, bool progress=true );

// Logistic Regression
// ===================
template<typename Real>
Int LogisticRegression
( const Matrix<Real>& G, const Matrix<Real>& q, Matrix<Real>& z,
  Real gamma, Regularization penalty=L1_PENALTY,
  Real rho=1, Int maxIter=500, bool inv=true, bool progress=true );
template<typename Real>
Int LogisticRegression
( const DistMatrix<Real>& G, const DistMatrix<Real>& q, DistMatrix<Real>& z,
  Real gamma, Regularization penalty=L1_PENALTY,
  Real rho=1, Int maxIter=500, bool inv=true, bool progress=true );

// Fit a model with using a loss function plus regularization
// ==========================================================
// TODO: Implement these functions
template<typename Real>
Int ModelFit
( std::function<void(Matrix<Real>&,Real)> lossProx,
  std::function<void(Matrix<Real>&,Real)> regProx,
  const Matrix<Real>& A, const Matrix<Real>& b, Matrix<Real>& w,
  Real rho, Int maxIter=1000, bool inv=true, bool progress=true );
template<typename Real>
Int ModelFit
( std::function<void(DistMatrix<Real>&,Real)> lossProx,
  std::function<void(DistMatrix<Real>&,Real)> regProx,
  const DistMatrix<Real>& A, const DistMatrix<Real>& b, DistMatrix<Real>& w,
  Real rho, Int maxIter=1000, bool inv=true, bool progress=true );

// Non-negative matrix factorization
// =================================
// TODO: Generalize to complex
template<typename Real>
void NMF( const Matrix<Real>& A, Matrix<Real>& X, Matrix<Real>& Y );
template<typename Real>
void NMF( const DistMatrix<Real>& A, DistMatrix<Real>& X, DistMatrix<Real>& Y );

// Non-negative least squares
// ==========================
// TODO: Generalize to complex
template<typename Real>
Int NonNegativeLeastSquares
( const Matrix<Real>& A, const Matrix<Real>& Y, Matrix<Real>& Z,
  Real rho=1., Real alpha=1.2, Int maxIter=500, Real absTol=1e-6,
  Real relTol=1e-4, bool inv=true, bool progress=true );
template<typename Real>
Int NonNegativeLeastSquares
( const DistMatrix<Real>& A, const DistMatrix<Real>& Y, DistMatrix<Real>& Z, 
  Real rho=1., Real alpha=1.2, Int maxIter=500, Real absTol=1e-6,
  Real relTol=1e-4, bool inv=true, bool progress=true );

// Quadratic program
// =================
// TODO: Generalize to complex
template<typename Real>
Int QuadraticProgram
( const Matrix<Real>& P, const Matrix<Real>& S, Real lb, Real ub,
  Matrix<Real>& Z, 
  Real rho=1., Real alpha=1.2, Int maxIter=500, Real absTol=1e-6,
  Real relTol=1e-4, bool inv=true, bool progress=true );
template<typename Real>
Int QuadraticProgram
( const DistMatrix<Real>& P, const DistMatrix<Real>& S, Real lb, Real ub,
  DistMatrix<Real>& Z,
  Real rho=1., Real alpha=1.2, Int maxIter=500, Real absTol=1e-6,
  Real relTol=1e-4, bool inv=true, bool progress=true );

// Robust Principal Component Analysis (RPCA)
// ==========================================

template<typename Real>
struct RpcaCtrl
{
    bool useALM;
    bool usePivQR;
    bool progress;

    Int numPivSteps;
    Int maxIts;

    Real tau;
    Real beta;
    Real rho;
    Real tol;

    RpcaCtrl() 
    : useALM(true), usePivQR(false), progress(true), 
      numPivSteps(75), maxIts(1000),
      tau(0), beta(1), rho(6), tol(1e-5)
    { }
};

template<typename F>
void RPCA
( const DistMatrix<F>& M, DistMatrix<F>& L, DistMatrix<F>& S,
  const RpcaCtrl<Base<F>>& ctrl=RpcaCtrl<Base<F>>() );

// Sparse inverse covariance selection
// ===================================
template<typename F>
Int SparseInvCov
( const Matrix<F>& D, Base<F> lambda, Matrix<F>& Z,
  Base<F> rho=1., Base<F> alpha=1.2, Int maxIter=500,
  Base<F> absTol=1e-6, Base<F> relTol=1e-4, bool progress=true );
template<typename F>
Int SparseInvCov
( const DistMatrix<F>& D, Base<F> lambda, DistMatrix<F>& Z,
  Base<F> rho=1., Base<F> alpha=1.2, Int maxIter=500,
  Base<F> absTol=1e-6, Base<F> relTol=1e-4, bool progress=true );

// Support Vector Machine
// ======================
template<typename Real>
Int SVM
( const Matrix<Real>& G, const Matrix<Real>& q, Matrix<Real>& z,
  Real gamma, Real rho=1, Int maxIter=500, bool inv=true, bool progress=true );
template<typename Real>
Int SVM
( const DistMatrix<Real>& G, const DistMatrix<Real>& q, DistMatrix<Real>& z,
  Real gamma, Real rho=1, Int maxIter=500, bool inv=true, bool progress=true );

// Utilities
// =========

// Clipping
// --------
template<typename Real>
void LowerClip( Matrix<Real>& X, Real lowerBound=0 );
template<typename Real,Dist U,Dist V>
void LowerClip( DistMatrix<Real,U,V>& X, Real lowerBound=0 );

template<typename Real>
void UpperClip( Matrix<Real>& X, Real upperBound=0 );
template<typename Real,Dist U,Dist V>
void UpperClip( DistMatrix<Real,U,V>& X, Real upperBound=0 );

template<typename Real>
void Clip( Matrix<Real>& X, Real lowerBound=0, Real upperBound=1 );
template<typename Real,Dist U,Dist V>
void Clip( DistMatrix<Real,U,V>& X, Real lowerBound=0, Real upperBound=1 );

// Covariance
// ----------
template<typename F>
void Covariance( const Matrix<F>& D, Matrix<F>& S );
template<typename F>
void Covariance( const DistMatrix<F>& D, DistMatrix<F>& S );

// Frobenius-norm proximal map
// ---------------------------
// The Frobenius norm prox returns the solution to
//     arg min || A ||_F + rho/2 || A - A0 ||_F^2
//        A
// where A0 in the input matrix.
template<typename F>
void FrobeniusProx( Matrix<F>& A, Base<F> rho );
template<typename F>
void FrobeniusProx( AbstractDistMatrix<F>& A, Base<F> rho );

// Hinge-loss proximal map
// -----------------------
// TODO: Description
template<typename Real>
void HingeLossProx( Matrix<Real>& A, Real rho );
template<typename Real>
void HingeLossProx( AbstractDistMatrix<Real>& A, Real rho );

// Log barrier
// -----------
template<typename F>
Base<F> LogBarrier( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> LogBarrier( UpperOrLower uplo, const DistMatrix<F>& A );

template<typename F>
Base<F> LogBarrier
( UpperOrLower uplo, Matrix<F>& A, bool canOverwrite=false );
template<typename F>
Base<F> LogBarrier
( UpperOrLower uplo, DistMatrix<F>& A, bool canOverwrite=false );

// Log-det divergence
// ------------------
template<typename F>
Base<F> LogDetDiv
( UpperOrLower uplo, const Matrix<F>& A, const Matrix<F>& B );
template<typename F>
Base<F> LogDetDiv
( UpperOrLower uplo, const DistMatrix<F>& A, const DistMatrix<F>& B );

// Logistic proximal map
// ---------------------
// The logistic proximal map returns the solution to
//    arg min sum_{i,j}[ log(1+exp(-A_{i,j})) ] + rho/2 || A - A0 ||_F^2
//       A
// where A0 is the input matrix.
template<typename Real>
void LogisticProx( Matrix<Real>& A, Real rho, Int numIts=5 );
template<typename Real>
void LogisticProx( AbstractDistMatrix<Real>& A, Real rho, Int numIts=5 );

// Singular-value soft thresholding
// --------------------------------
template<typename F>
Int SVT( Matrix<F>& A, Base<F> rho, bool relative=false );
template<typename F>
Int SVT( DistMatrix<F>& A, Base<F> rho, bool relative=false );
template<typename F>
Int SVT( Matrix<F>& A, Base<F> rho, Int relaxedRank, bool relative=false );
template<typename F>
Int SVT( DistMatrix<F>& A, Base<F> rho, Int relaxedRank, bool relative=false );
template<typename F,Dist U>
Int SVT( DistMatrix<F,U,STAR>& A, Base<F> rho, bool relative=false );

namespace svt {

template<typename F>
Int Cross( Matrix<F>& A, Base<F> rho, bool relative=false );
template<typename F>
Int Cross( DistMatrix<F>& A, Base<F> rho, bool relative=false );
template<typename F>
Int TallCross( DistMatrix<F,VC,STAR>& A, Base<F> rho, bool relative=false );

template<typename F>
Int Normal( Matrix<F>& A, Base<F> rho, bool relative=false );
template<typename F>
Int Normal( DistMatrix<F>& A, Base<F> rho, bool relative=false );

template<typename F>
Int PivotedQR
( Matrix<F>& A, Base<F> rho, Int numSteps, bool relative=false );
template<typename F>
Int PivotedQR
( DistMatrix<F>& A, Base<F> rho, Int numSteps, bool relative=false );

template<typename F,Dist U>
Int TSQR( DistMatrix<F,U,STAR>& A, Base<F> rho, bool relative=false );

} // namespace svt

// Soft-thresholding
// -----------------
// Returns the solution to
//     arg min || vec(A) ||_1 + rho/2 || A - A0 ||_F^2
//        A 
// where A0 is the input matrix.
template<typename F>
F SoftThreshold( F alpha, Base<F> rho );

template<typename F>
void SoftThreshold
( Matrix<F>& A, Base<F> rho, bool relative=false );
template<typename F>
void SoftThreshold
( AbstractDistMatrix<F>& A, Base<F> rho, bool relative=false );

} // namespace El

#endif // ifndef EL_OPTIMIZATION_HPP
