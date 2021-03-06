/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_PROPS_HPP
#define EL_PROPS_HPP

namespace El {

enum PseudospecNorm {
  PS_TWO_NORM,
  PS_ONE_NORM
  /* For now, handle the infinity norm by using the adjoint matrix */
};

// Configurations for how often and what format numerical (num) and image (img)
// snapshots of the pseudospectral estimates should be saved
struct SnapshotCtrl
{
    Int realSize, imagSize;

    Int imgSaveFreq, numSaveFreq, imgDispFreq;
    Int imgSaveCount, numSaveCount, imgDispCount;
    std::string imgBase, numBase;
    FileFormat imgFormat, numFormat;
    bool itCounts;

    SnapshotCtrl()
    : realSize(0), imagSize(0),
      imgSaveFreq(-1), numSaveFreq(-1), imgDispFreq(-1),
      imgSaveCount(0), numSaveCount(0), imgDispCount(0),
      imgBase("ps"), numBase("ps"), imgFormat(PNG), numFormat(ASCII_MATLAB),
      itCounts(true)
    { }

    void ResetCounts()
    {
        imgSaveCount = 0;
        numSaveCount = 0;
        imgDispCount = 0;
    }
    void Iterate()
    {
        ++imgSaveCount;
        ++numSaveCount;
        ++imgDispCount;
    }
};

template<typename Real>
struct PseudospecCtrl
{
    PseudospecNorm norm;
    Int blockWidth; // block width for block 1-norm estimator

    // Preprocessing configuration
    bool schur; // begin with reduction to Schur form?
    bool forceComplexSchur;
    bool forceComplexPs;
    SchurCtrl<Real> schurCtrl;

    // Convergence and deflation criteria
    Int maxIts;
    Real tol;
    bool deflate;

    // (Implicitly Restarted) Arnoldi/Lanczos. If basisSize > 1, then
    // there is implicit restarting
    bool arnoldi;
    Int basisSize;
    bool reorthog; // only matters for IRL, which isn't currently used

    // Whether or not to print progress information at each iteration
    bool progress;

    SnapshotCtrl snapCtrl;

    PseudospecCtrl()
    : norm(PS_TWO_NORM), blockWidth(10),
      schur(true), forceComplexSchur(false), forceComplexPs(false), schurCtrl(),
      maxIts(200), tol(1e-6), deflate(true),
      arnoldi(true), basisSize(10), reorthog(true),
      progress(false), snapCtrl()
    { }
};

// Condition number
// ================
template<typename F>
Base<F> Condition( const Matrix<F>& A, NormType type=TWO_NORM );
template<typename F>
Base<F> Condition( const AbstractDistMatrix<F>& A, NormType type=TWO_NORM );

template<typename F>
Base<F> FrobeniusCondition( const Matrix<F>& A );
template<typename F>
Base<F> FrobeniusCondition( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> InfinityCondition( const Matrix<F>& A );
template<typename F>
Base<F> InfinityCondition( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> MaxCondition( const Matrix<F>& A );
template<typename F>
Base<F> MaxCondition( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> OneCondition( const Matrix<F>& A );
template<typename F>
Base<F> OneCondition( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> TwoCondition( const Matrix<F>& A );
template<typename F>
Base<F> TwoCondition( const AbstractDistMatrix<F>& A );

// Determinant
// ===========
template<typename F>
SafeProduct<F> SafeDeterminant( const Matrix<F>& A );
template<typename F>
SafeProduct<F> SafeDeterminant( const AbstractDistMatrix<F>& A );
template<typename F>
SafeProduct<F> SafeDeterminant( Matrix<F>& A, bool canOverwrite=false );
template<typename F>
SafeProduct<F> SafeDeterminant( DistMatrix<F>& A, bool canOverwrite=false );

template<typename F>
SafeProduct<Base<F>> SafeHPDDeterminant
( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
SafeProduct<Base<F>> SafeHPDDeterminant
( UpperOrLower uplo, const AbstractDistMatrix<F>& A );
template<typename F>
SafeProduct<Base<F>> SafeHPDDeterminant
( UpperOrLower uplo, Matrix<F>& A, bool canOverwrite=false );
template<typename F>
SafeProduct<Base<F>> SafeHPDDeterminant
( UpperOrLower uplo, DistMatrix<F>& A, bool canOverwrite=false );

template<typename F>
F Determinant( const Matrix<F>& A );
template<typename F>
F Determinant( const AbstractDistMatrix<F>& A );
template<typename F>
F Determinant( Matrix<F>& A, bool canOverwrite=false );
template<typename F>
F Determinant( DistMatrix<F>& A, bool canOverwrite=false );

template<typename F>
Base<F> HPDDeterminant( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> HPDDeterminant( UpperOrLower uplo, const AbstractDistMatrix<F>& A );
template<typename F>
Base<F> HPDDeterminant
( UpperOrLower uplo, Matrix<F>& A, bool canOverwrite=false );
template<typename F>
Base<F> HPDDeterminant
( UpperOrLower uplo, DistMatrix<F>& A, bool canOverwrite=false );

namespace hpd_det {

template<typename F>
SafeProduct<Base<F>> AfterCholesky( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
SafeProduct<Base<F>> AfterCholesky( UpperOrLower uplo, const DistMatrix<F>& A );

} // namespace hpd_det

namespace det {

template<typename F>
SafeProduct<F> AfterLUPartialPiv
( const Matrix<F>& A, const Matrix<Int>& pPerm );
template<typename F,Dist UPerm>
SafeProduct<F> AfterLUPartialPiv
( const DistMatrix<F>& A, const DistMatrix<Int,UPerm,STAR>& pPerm );

} // namespace det

// Inertia
// =======
template<typename F>
InertiaType Inertia
( UpperOrLower uplo, Matrix<F>& A, LDLPivotType pivotType=BUNCH_PARLETT );
template<typename F>
InertiaType Inertia
( UpperOrLower uplo, DistMatrix<F>& A, LDLPivotType pivotType=BUNCH_PARLETT );

// Norm
// ====
template<typename F>
Base<F> Norm( const Matrix<F>& A, NormType type=FROBENIUS_NORM );
template<typename F>
Base<F> Norm( const AbstractDistMatrix<F>& A, NormType type=FROBENIUS_NORM );

template<typename F>
Base<F> SymmetricNorm
( UpperOrLower uplo, const Matrix<F>& A, NormType type=FROBENIUS_NORM );
template<typename F>
Base<F> SymmetricNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A, 
  NormType type=FROBENIUS_NORM );

template<typename F>
Base<F> HermitianNorm
( UpperOrLower uplo, const Matrix<F>& A, NormType type=FROBENIUS_NORM );
template<typename F>
Base<F> HermitianNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A, 
  NormType type=FROBENIUS_NORM );

// Entrywise norm
// --------------
template<typename F>
Base<F> EntrywiseNorm( const Matrix<F>& A, Base<F> p );
template<typename F>
Base<F> EntrywiseNorm( const AbstractDistMatrix<F>& A, Base<F> p );

template<typename F>
Base<F> HermitianEntrywiseNorm
( UpperOrLower uplo, const Matrix<F>& A, Base<F> p );
template<typename F>
Base<F> HermitianEntrywiseNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A, Base<F> p );

template<typename F>
Base<F> SymmetricEntrywiseNorm
( UpperOrLower uplo, const Matrix<F>& A, Base<F> p );
template<typename F>
Base<F> SymmetricEntrywiseNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A, Base<F> p );

// Entrywise one-norm
// ------------------
template<typename F>
Base<F> EntrywiseOneNorm( const Matrix<F>& A );
template<typename F>
Base<F> EntrywiseOneNorm( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> HermitianEntrywiseOneNorm
( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> HermitianEntrywiseOneNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> SymmetricEntrywiseOneNorm
( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> SymmetricEntrywiseOneNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

// Frobenius norm
// --------------
template<typename F>
Base<F> FrobeniusNorm( const Matrix<F>& A );
template<typename F>
Base<F> FrobeniusNorm( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> HermitianFrobeniusNorm
( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> HermitianFrobeniusNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> SymmetricFrobeniusNorm
( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> SymmetricFrobeniusNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

// Infinity norm
// -------------
template<typename F>
Base<F> InfinityNorm( const Matrix<F>& A );
template<typename F>
Base<F> InfinityNorm( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> HermitianInfinityNorm
( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> HermitianInfinityNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> SymmetricInfinityNorm
( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> SymmetricInfinityNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

// Ky-Fan norms
// ------------
template<typename F>
Base<F> KyFanNorm( const Matrix<F>& A, Int k );
template<typename F>
Base<F> KyFanNorm( const AbstractDistMatrix<F>& A, Int k );

template<typename F>
Base<F> HermitianKyFanNorm
( UpperOrLower uplo, const Matrix<F>& A, Int k );
template<typename F>
Base<F> HermitianKyFanNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A, Int k );

template<typename F>
Base<F> SymmetricKyFanNorm
( UpperOrLower uplo, const Matrix<F>& A, Int k );
template<typename F>
Base<F> SymmetricKyFanNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A, Int k );

// Ky-Fan-Schatten norms
// ---------------------
template<typename F>
Base<F> KyFanSchattenNorm( const Matrix<F>& A, Int k, Base<F> p );
template<typename F>
Base<F> KyFanSchattenNorm( const AbstractDistMatrix<F>& A, Int k, Base<F> p );

template<typename F>
Base<F> HermitianKyFanSchattenNorm
( UpperOrLower uplo, const Matrix<F>& A, Int k, Base<F> p );
template<typename F>
Base<F> HermitianKyFanSchattenNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A, Int k, Base<F> p );

template<typename F>
Base<F> SymmetricKyFanSchattenNorm
( UpperOrLower uplo, const Matrix<F>& A, Int k, Base<F> p );
template<typename F>
Base<F> SymmetricKyFanSchattenNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A, Int k, Base<F> p );

// Max norm
// --------
template<typename F>
Base<F> MaxNorm( const Matrix<F>& A );
template<typename F>
Base<F> MaxNorm( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> HermitianMaxNorm( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> HermitianMaxNorm( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> SymmetricMaxNorm( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> SymmetricMaxNorm( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

// Nuclear norm
// ------------
template<typename F>
Base<F> NuclearNorm( const Matrix<F>& A );
template<typename F>
Base<F> NuclearNorm( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> HermitianNuclearNorm( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> HermitianNuclearNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> SymmetricNuclearNorm( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> SymmetricNuclearNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

// One norm
// --------
template<typename F>
Base<F> OneNorm( const Matrix<F>& A );
template<typename F>
Base<F> OneNorm( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> HermitianOneNorm( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> HermitianOneNorm( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> SymmetricOneNorm( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> SymmetricOneNorm( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

// Schatten norm
// -------------
template<typename F>
Base<F> SchattenNorm( const Matrix<F>& A, Base<F> p );
template<typename F>
Base<F> SchattenNorm( const AbstractDistMatrix<F>& A, Base<F> p );

template<typename F>
Base<F> HermitianSchattenNorm
( UpperOrLower uplo, const Matrix<F>& A, Base<F> p );
template<typename F>
Base<F> HermitianSchattenNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A, Base<F> p );

template<typename F>
Base<F> SymmetricSchattenNorm
( UpperOrLower uplo, const Matrix<F>& A, Base<F> p );
template<typename F>
Base<F> SymmetricSchattenNorm
( UpperOrLower uplo, const AbstractDistMatrix<F>& A, Base<F> p );

// Two norm
// --------
template<typename F>
Base<F> TwoNorm( const Matrix<F>& A );
template<typename F>
Base<F> TwoNorm( const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> HermitianTwoNorm( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> HermitianTwoNorm( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

template<typename F>
Base<F> SymmetricTwoNorm( UpperOrLower uplo, const Matrix<F>& A );
template<typename F>
Base<F> SymmetricTwoNorm( UpperOrLower uplo, const AbstractDistMatrix<F>& A );

// Zero "norm"
// -----------
template<typename F>
Int ZeroNorm( const Matrix<F>& A, Base<F> tol=0 );
template<typename F>
Int ZeroNorm( const AbstractDistMatrix<F>& A, Base<F> tol=0 );

// Two-norm estimate
// -----------------
template<typename F>
Base<F> TwoNormEstimate
( const Matrix<F>& A, Base<F> tol=1e-6, Int maxIts=1000 );
template<typename F>
Base<F> TwoNormEstimate
( const DistMatrix<F>& A, Base<F> tol=1e-6, Int maxIts=1000 );

template<typename F>
Base<F> HermitianTwoNormEstimate
( UpperOrLower uplo, const Matrix<F>& A, 
  Base<F> tol=1e-6, Int maxIts=1000 );
template<typename F>
Base<F> HermitianTwoNormEstimate
( UpperOrLower uplo, const DistMatrix<F>& A, 
  Base<F> tol=1e-6, Int maxIts=1000 );

template<typename F>
Base<F> SymmetricTwoNormEstimate
( UpperOrLower uplo, const Matrix<F>& A, 
  Base<F> tol=1e-6, Int maxIts=1000 );
template<typename F>
Base<F> SymmetricTwoNormEstimate
( UpperOrLower uplo, const DistMatrix<F>& A, 
  Base<F> tol=1e-6, Int maxIts=1000 );


// Pseudospectra
// =============

// TODO: Pick unique names for the automatic window, manual window,
//       and point cloud interfaces. 

// Automatic window
// ---------------
// Treat each pixel as being located a cell center and tesselate a box with
// said square cells

template<typename F>
Matrix<Int> Pseudospectra
( const Matrix<F>& A, Matrix<Base<F>>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int> Pseudospectra
( const DistMatrix<F>& A, DistMatrix<Base<F>>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, Matrix<Base<F>>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int> TriangularPseudospectra
( const DistMatrix<F>& U, DistMatrix<Base<F>>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int> TriangularPseudospectra
( const DistMatrix<F>& U, const DistMatrix<F>& Q, 
  DistMatrix<Base<F>>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U, Matrix<Real>& invNormMap, 
  Int realSize, Int imagSize,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );
template<typename Real>
DistMatrix<Int> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U, DistMatrix<Real>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U, const Matrix<Real>& Q, Matrix<Real>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );
template<typename Real>
DistMatrix<Int> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U, const DistMatrix<Real>& Q,
  DistMatrix<Real>& invNormMap, Int realSize, Int imagSize,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, Matrix<Base<F>>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int> HessenbergPseudospectra
( const DistMatrix<F>& H, DistMatrix<Base<F>>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int> HessenbergPseudospectra
( const DistMatrix<F>& H, const DistMatrix<F>& Q, 
  DistMatrix<Base<F>>& invNormMap, Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

// Manual window
// -------------
// Treat each pixel as being located a cell center and tesselate a box with
// said square cells
template<typename F>
Matrix<Int> Pseudospectra
( const Matrix<F>& A, Matrix<Base<F>>& invNormMap,
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int> Pseudospectra
( const DistMatrix<F>& A, DistMatrix<Base<F>>& invNormMap,
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, Matrix<Base<F>>& invNormMap,
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int> TriangularPseudospectra
( const DistMatrix<F>& U, DistMatrix<Base<F>>& invNormMap,
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap,
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int> TriangularPseudospectra
( const DistMatrix<F>& U, const DistMatrix<F>& Q,
  DistMatrix<Base<F>>& invNormMap,
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U,
  Matrix<Real>& invNormMap,
  Complex<Real> center, Real realWidth, Real imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );
template<typename Real>
DistMatrix<Int> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U,
  DistMatrix<Real>& invNormMap,
  Complex<Real> center, Real realWidth, Real imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U,
  const Matrix<Real>& Q,
  Matrix<Real>& invNormMap,
  Complex<Real> center, Real realWidth, Real imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );
template<typename Real>
DistMatrix<Int> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U,
  const DistMatrix<Real>& Q,
  DistMatrix<Real>& invNormMap,
  Complex<Real> center, Real realWidth, Real imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, Matrix<Base<F>>& invNormMap,
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int> HessenbergPseudospectra
( const DistMatrix<F>& H, DistMatrix<Base<F>>& invNormMap,
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap,
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int> HessenbergPseudospectra
( const DistMatrix<F>& H,
  const DistMatrix<F>& Q,
  DistMatrix<Base<F>>& invNormMap,
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

// Point cloud
// -----------
template<typename F>
Matrix<Int> Pseudospectra
( const Matrix<F>& A, const Matrix<Complex<Base<F>>>& shifts,
  Matrix<Base<F>>& invNorms,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int,VR,STAR> Pseudospectra
( const DistMatrix<F>& A, const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts,
  DistMatrix<Base<F>,VR,STAR>& invNorms,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, const Matrix<Complex<Base<F>>>& shifts,
  Matrix<Base<F>>& invNorms,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int,VR,STAR> TriangularPseudospectra
( const DistMatrix<F>& U,
  const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts,
  DistMatrix<Base<F>,VR,STAR>& invNorms,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, const Matrix<F>& Q,
  const Matrix<Complex<Base<F>>>& shifts, Matrix<Base<F>>& invNorms,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int,VR,STAR> TriangularPseudospectra
( const DistMatrix<F>& U, const DistMatrix<F>& Q,
  const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts,
  DistMatrix<Base<F>,VR,STAR>& invNorms,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U,
  const Matrix<Complex<Real>>& shifts,
  Matrix<Real>& invNorms,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );
template<typename Real>
DistMatrix<Int,VR,STAR> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U,
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U, const Matrix<Real>& Q,
  const Matrix<Complex<Real>>& shifts,
  Matrix<Real>& invNorms,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );
template<typename Real>
DistMatrix<Int,VR,STAR> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U,
  const DistMatrix<Real>& Q,
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms,
  PseudospecCtrl<Real> psCtrl=PseudospecCtrl<Real>() );

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, const Matrix<Complex<Base<F>>>& shifts,
  Matrix<Base<F>>& invNorms,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int,VR,STAR> HessenbergPseudospectra
( const DistMatrix<F>& H, const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts,
  DistMatrix<Base<F>,VR,STAR>& invNorms,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, const Matrix<F>& Q,
  const Matrix<Complex<Base<F>>>& shifts, Matrix<Base<F>>& invNorms,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );
template<typename F>
DistMatrix<Int,VR,STAR> HessenbergPseudospectra
( const DistMatrix<F>& H, const DistMatrix<F>& Q,
  const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts,
  DistMatrix<Base<F>,VR,STAR>& invNorms,
  PseudospecCtrl<Base<F>> psCtrl=PseudospecCtrl<Base<F>>() );

// Trace
// =====
template<typename T>
T Trace( const Matrix<T>& A );
template<typename T>
T Trace( const AbstractDistMatrix<T>& A );

} // namespace El

#endif // ifndef EL_PROPS_HPP
