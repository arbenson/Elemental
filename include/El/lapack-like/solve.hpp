/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_SOLVE_HPP
#define EL_SOLVE_HPP

namespace El {

// B := inv(A) B for a general square A
// ====================================
template<typename F>
void GaussianElimination( Matrix<F>& A, Matrix<F>& B );
template<typename F>
void GaussianElimination( DistMatrix<F>& A, DistMatrix<F>& B );

// min_{X,Y} || Y ||_F subject to D = A X + B Y
// ============================================
template<typename F>
void GLM
( Matrix<F>& A, Matrix<F>& B, Matrix<F>& D, Matrix<F>& Y );
template<typename F>
void GLM
( DistMatrix<F>& A, DistMatrix<F>& B, DistMatrix<F>& D, DistMatrix<F>& Y );

// B := inv(A) B for a Hermitian A
// ===============================
template<typename F>
void HermitianSolve
( UpperOrLower uplo, Orientation orientation, 
  Matrix<F>& A, Matrix<F>& B, LDLPivotType pivotType=BUNCH_KAUFMAN_A );
template<typename F>
void HermitianSolve
( UpperOrLower uplo, Orientation orientation,
  DistMatrix<F>& A, DistMatrix<F>& B, LDLPivotType pivotType=BUNCH_KAUFMAN_A );

// B := inv(A) for Hermitian Positive-Definite (HPD) A
// ===================================================
template<typename F>
void HPDSolve
( UpperOrLower uplo, Orientation orientation, 
  Matrix<F>& A, Matrix<F>& B );
template<typename F>
void HPDSolve
( UpperOrLower uplo, Orientation orientation,
  DistMatrix<F>& A, DistMatrix<F>& B );

// min_X || A X - B ||_F
// =====================
template<typename F>
void LeastSquares
( Orientation orientation, Matrix<F>& A, const Matrix<F>& B, 
  Matrix<F>& X );
template<typename F>
void LeastSquares
( Orientation orientation, DistMatrix<F>& A, const DistMatrix<F>& B, 
  DistMatrix<F>& X );

// min_X || A X - C ||_F subject to B X = D
// ========================================
template<typename F>
void LSE
( Matrix<F>& A, Matrix<F>& B, Matrix<F>& C, Matrix<F>& D, 
  Matrix<F>& X, bool computeResidual=false );
template<typename F>
void LSE
( DistMatrix<F>& A, DistMatrix<F>& B, DistMatrix<F>& C, DistMatrix<F>& D, 
  DistMatrix<F>& X, bool computeResidual=false );

// Solve for B in op(H) B - B op(D) = X, where H is Hessenberg
// ===========================================================
template<typename F>
void MultiShiftHessSolve
( UpperOrLower uplo, Orientation orientation,
  F alpha, const Matrix<F>& H, const Matrix<F>& shifts,
  Matrix<F>& X );
template<typename F,Dist UH,Dist VH,Dist VX>
void MultiShiftHessSolve
( UpperOrLower uplo, Orientation orientation,
  F alpha, const DistMatrix<F,UH,VH>& H, const DistMatrix<F,VX,STAR>& shifts,
  DistMatrix<F,STAR,VX>& X );

// Ridge regression
// ================
// NOTE: This is simply Tikhonov regularization for cases where 
//       Gamma = alpha I

namespace RidgeAlgNS {
enum RidgeAlg {
    RIDGE_CHOLESKY,
    RIDGE_QR,
    RIDGE_SVD
};
}
using namespace RidgeAlgNS;

template<typename F>
void Ridge
( const Matrix<F>& A, const Matrix<F>& B, 
  Base<F> alpha, Matrix<F>& X, RidgeAlg alg=RIDGE_CHOLESKY );
template<typename F>
void Ridge
( const DistMatrix<F>& A, const DistMatrix<F>& B, 
  Base<F> alpha, DistMatrix<F>& X, RidgeAlg alg=RIDGE_CHOLESKY );

// B := inv(A) B for a symmetric A
// ===============================
template<typename F>
void SymmetricSolve
( UpperOrLower uplo, Orientation orientation, 
  Matrix<F>& A, Matrix<F>& B, bool conjugate=false, 
  LDLPivotType pivotType=BUNCH_KAUFMAN_A );
template<typename F>
void SymmetricSolve
( UpperOrLower uplo, Orientation orientation,
  DistMatrix<F>& A, DistMatrix<F>& B, bool conjugate=false, 
  LDLPivotType pivotType=BUNCH_KAUFMAN_A );

// Tikhonov regularization
// =======================
// Solve arg min_X || op(A) X - B ||_2^2 + || Gamma X ||_2^2
// where op(A) is A, A^T, or A^H.

namespace TikhonovAlgNS {
enum TikhonovAlg {
    TIKHONOV_CHOLESKY,
    TIKHONOV_QR
};
}
using namespace TikhonovAlgNS;

template<typename F>
void Tikhonov
( const Matrix<F>& A, const Matrix<F>& B, 
  const Matrix<F>& Gamma, Matrix<F>& X, 
  TikhonovAlg alg=TIKHONOV_CHOLESKY );
template<typename F>
void Tikhonov
( const DistMatrix<F>& A, const DistMatrix<F>& B, 
  const DistMatrix<F>& Gamma, DistMatrix<F>& X, 
  TikhonovAlg alg=TIKHONOV_CHOLESKY );

// TODO: Generalized Tikhonov regularization

} // namespace El

#endif // ifndef EL_SOLVE_HPP
