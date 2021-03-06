/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

#include "./Pseudospectra/Util.hpp"
#include "./Pseudospectra/Power.hpp"
#include "./Pseudospectra/Lanczos.hpp"
#include "./Pseudospectra/IRA.hpp"
#include "./Pseudospectra/IRL.hpp"
#include "./Pseudospectra/Analytic.hpp"

// For one-norm pseudospectra. An adaptation of the more robust algorithm of
// Higham and Tisseur will hopefully be implemented soon.
#include "./Pseudospectra/HagerHigham.hpp"

namespace El {

namespace pspec {

template<typename Real>
Matrix<Int> TriangularHelper
( const Matrix<Complex<Real>>& U, const Matrix<Complex<Real>>& shifts, 
  Matrix<Real>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::TriangularHelper"))

    // Check if the off-diagonal is sufficiently small; if so, compute the 
    // pseudospectrum analytically from the eigenvalues. This also takes care
    // of the case where the matrix is a constant multiple of the identity 
    // matrix, which, after shifting, can lead to the zero matrix, which would 
    // cause problems for the Lanczos convergence criteria.
    if( pspec::TriangIsNormal( U, psCtrl.tol ) )
    {
        Matrix<Int> itCounts;
        if( psCtrl.progress )
            std::cout << "Matrix was numerically normal" << std::endl;
        auto w = U.GetDiagonal();
        if( psCtrl.norm == PS_TWO_NORM )
            pspec::Analytic( w, shifts, invNorms, psCtrl.snapCtrl );
        else
            LogicError("Analytic one-norm pseudospectra not yet supported");
        Zeros( itCounts, shifts.Height(), 1 );        
        return itCounts;
    }

    psCtrl.schur = true;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.arnoldi )
        {
            if( psCtrl.basisSize > 1 )
                return pspec::IRA( U, shifts, invNorms, psCtrl );
            else
                return pspec::Lanczos( U, shifts, invNorms, psCtrl );
        }
        else
            return pspec::Power( U, shifts, invNorms, psCtrl );
    }
    else
        return pspec::HagerHigham( U, shifts, invNorms, psCtrl ); 
        // Q is assumed to be the identity
}

template<typename Real>
Matrix<Int> TriangularHelper
( const Matrix<Complex<Real>>& U, const Matrix<Complex<Real>>& Q, 
  const Matrix<Complex<Real>>& shifts, 
  Matrix<Real>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::TriangularHelper"))

    // Check if the off-diagonal is sufficiently small; if so, compute the 
    // pseudospectrum analytically from the eigenvalues. This also takes care
    // of the case where the matrix is a constant multiple of the identity 
    // matrix, which, after shifting, can lead to the zero matrix, which would 
    // cause problems for the Lanczos convergence criteria.
    if( pspec::TriangIsNormal( U, psCtrl.tol ) )
    {
        Matrix<Int> itCounts;
        if( psCtrl.progress )
            std::cout << "Matrix was numerically normal" << std::endl;
        auto w = U.GetDiagonal();
        if( psCtrl.norm == PS_TWO_NORM )
            pspec::Analytic( w, shifts, invNorms, psCtrl.snapCtrl );
        else
            LogicError("Analytic one-norm pseudospectra not yet supported");
        Zeros( itCounts, shifts.Height(), 1 );        
        return itCounts;
    }

    psCtrl.schur = true;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.arnoldi )
        {
            if( psCtrl.basisSize > 1 )
                return pspec::IRA( U, shifts, invNorms, psCtrl );
            else
                return pspec::Lanczos( U, shifts, invNorms, psCtrl );
        }
        else
            return pspec::Power( U, shifts, invNorms, psCtrl );
    }
    else
        return pspec::HagerHigham( U, Q, shifts, invNorms, psCtrl );
}

template<typename Real>
Matrix<Int> TriangularHelper
( const Matrix<Real>& U, const Matrix<Complex<Real>>& shifts, 
  Matrix<Real>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::TriangularHelper"))
    Matrix<Complex<Real>> UCpx;
    Copy( U, UCpx );

    // TODO: Use a real multi-shift TRSM instead
    return TriangularHelper( UCpx, shifts, invNorms, psCtrl );
}

template<typename Real>
Matrix<Int> TriangularHelper
( const Matrix<Real>& U, const Matrix<Real>& Q, 
  const Matrix<Complex<Real>>& shifts, Matrix<Real>& invNorms, 
  PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::TriangularHelper"))
    Matrix<Complex<Real>> UCpx, QCpx;
    Copy( U, UCpx );
    Copy( Q, QCpx );

    // TODO: Use a real multi-shift TRSM instead
    return TriangularHelper( UCpx, QCpx, shifts, invNorms, psCtrl );
}

} // namespace pspec

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, 
  const Matrix<Complex<Base<F>>>& shifts, Matrix<Base<F>>& invNorms, 
  PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    return pspec::TriangularHelper( U, shifts, invNorms, psCtrl );
}

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, const Matrix<F>& Q, 
  const Matrix<Complex<Base<F>>>& shifts, Matrix<Base<F>>& invNorms, 
  PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    return pspec::TriangularHelper( U, Q, shifts, invNorms, psCtrl );
}

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U, 
  const Matrix<Complex<Real>>& shifts, 
  Matrix<Real>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))

    // Check if the off-diagonal is sufficiently small; if so, compute the 
    // pseudospectrum analytically from the eigenvalues. This also takes care
    // of the case where the matrix is a constant multiple of the identity 
    // matrix, which, after shifting, can lead to the zero matrix, which would 
    // cause problems for the Lanczos convergence criteria.
    if( pspec::QuasiTriangIsNormal( U, psCtrl.tol ) )
    {
        Matrix<Int> itCounts;
        if( psCtrl.progress )
            std::cout << "Matrix was numerically normal" << std::endl;
        const auto w = schur::QuasiTriangEig( U );
        if( psCtrl.norm == PS_TWO_NORM )
            pspec::Analytic( w, shifts, invNorms, psCtrl.snapCtrl );
        else
            LogicError("Analytic one-norm pseudospectra not yet supported");
        Zeros( itCounts, shifts.Height(), 1 );        
        return itCounts;
    }

    psCtrl.schur = true;
    if( psCtrl.norm == PS_ONE_NORM )
        LogicError("This option is not yet written");
    return pspec::IRA( U, shifts, invNorms, psCtrl );
}

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U,  const Matrix<Real>& Q,
  const Matrix<Complex<Real>>& shifts,
  Matrix<Real>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))

    // Check if the off-diagonal is sufficiently small; if so, compute the 
    // pseudospectrum analytically from the eigenvalues. This also takes care
    // of the case where the matrix is a constant multiple of the identity 
    // matrix, which, after shifting, can lead to the zero matrix, which would 
    // cause problems for the Lanczos convergence criteria.
    if( pspec::QuasiTriangIsNormal( U, psCtrl.tol ) )
    {
        Matrix<Int> itCounts;
        if( psCtrl.progress )
            std::cout << "Matrix was numerically normal" << std::endl;
        auto w = schur::QuasiTriangEig( U );
        if( psCtrl.norm == PS_TWO_NORM )
            pspec::Analytic( w, shifts, invNorms, psCtrl.snapCtrl );
        else
            LogicError("Analytic one-norm pseudospectra not yet supported");
        Zeros( itCounts, shifts.Height(), 1 );
        return itCounts;
    }

    psCtrl.schur = true;
    if( psCtrl.norm == PS_ONE_NORM )
        LogicError("This option is not yet written");
    return pspec::IRA( U, shifts, invNorms, psCtrl );
}

namespace pspec {

template<typename Real>
Matrix<Int> HessenbergHelper
( const Matrix<Complex<Real>>& H, const Matrix<Complex<Real>>& shifts, 
  Matrix<Real>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::HessenbergHelper"))

    // TODO: Check if the subdiagonal is numerically zero, and, if so, revert to
    //       triangular version of Pseudospectra?
    psCtrl.schur = false;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.arnoldi )
        {
            if( psCtrl.basisSize > 1 )
                return pspec::IRA( H, shifts, invNorms, psCtrl );
            else
                return pspec::Lanczos( H, shifts, invNorms, psCtrl );
        }
        else
            return pspec::Power( H, shifts, invNorms, psCtrl );
    }
    else
        return pspec::HagerHigham( H, shifts, invNorms, psCtrl ); 
        // Q is assumed to be the identity
}

template<typename Real>
Matrix<Int> HessenbergHelper
( const Matrix<Complex<Real>>& H, const Matrix<Complex<Real>>& Q, 
  const Matrix<Complex<Real>>& shifts, Matrix<Real>& invNorms, 
  PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::HessenbergHelper"))

    // TODO: Check if the subdiagonal is numerically zero, and, if so, revert to
    //       triangular version of Pseudospectra?
    psCtrl.schur = false;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.arnoldi )
        {
            if( psCtrl.basisSize > 1 )
                return pspec::IRA( H, shifts, invNorms, psCtrl );
            else
                return pspec::Lanczos( H, shifts, invNorms, psCtrl );
        }
        else
            return pspec::Power( H, shifts, invNorms, psCtrl );
    }
    else
        return pspec::HagerHigham( H, Q, shifts, invNorms, psCtrl );
}

template<typename Real>
Matrix<Int> HessenbergHelper
( const Matrix<Real>& H, const Matrix<Complex<Real>>& shifts, 
  Matrix<Real>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::HessenbergHelper"))
    Matrix<Complex<Real>> HCpx;
    Copy( H, HCpx );

    // TODO: Use a real multi-shift Hess. solve instead
    return HessenbergHelper( HCpx, shifts, invNorms, psCtrl );
}

template<typename Real>
Matrix<Int> HessenbergHelper
( const Matrix<Real>& H, const Matrix<Real>& Q, 
  const Matrix<Complex<Real>>& shifts, Matrix<Real>& invNorms, 
  PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::HessenbergHelper"))
    Matrix<Complex<Real>> HCpx, QCpx;
    Copy( H, HCpx );
    Copy( Q, QCpx );

    // TODO: Use a real multi-shift Hess. solve instead
    return HessenbergHelper( HCpx, QCpx, shifts, invNorms, psCtrl );
}

} // namespace pspec

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, const Matrix<Complex<Base<F>>>& shifts, 
  Matrix<Base<F>>& invNorms, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    return pspec::HessenbergHelper( H, shifts, invNorms, psCtrl );
}

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, const Matrix<F>& Q, 
  const Matrix<Complex<Base<F>>>& shifts, Matrix<Base<F>>& invNorms, 
  PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    return pspec::HessenbergHelper( H, Q, shifts, invNorms, psCtrl );
}

namespace pspec {

template<typename Real>
DistMatrix<Int,VR,STAR> TriangularHelper
( const DistMatrix<Complex<Real>>& U, 
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::TriangularHelper"))
    const Grid& g = U.Grid();

    // Check if the off-diagonal is sufficiently small; if so, compute the 
    // pseudospectrum analytically from the eigenvalues. This also takes care
    // of the case where the matrix is a constant multiple of the identity 
    // matrix, which, after shifting, can lead to the zero matrix, which would 
    // cause problems for the Lanczos convergence criteria.
    if( pspec::TriangIsNormal( U, psCtrl.tol ) )
    {
        DistMatrix<Int,VR,STAR> itCounts(g);
        if( psCtrl.progress && g.Rank() == 0 )
            std::cout << "Matrix was numerically normal" << std::endl;
        auto w = U.GetDiagonal();
        if( psCtrl.norm == PS_TWO_NORM )
            pspec::Analytic( w, shifts, invNorms, psCtrl.snapCtrl );
        else
            LogicError("Analytic one-norm pseudospectra not yet supported");
        itCounts.AlignWith( shifts );
        Zeros( itCounts, shifts.Height(), 1 );
        return itCounts;
    }

    psCtrl.schur = true;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.arnoldi )
        {
            if( psCtrl.basisSize > 1 )
                return pspec::IRA( U, shifts, invNorms, psCtrl );
            else
                return pspec::Lanczos( U, shifts, invNorms, psCtrl );
        }
        else
            return pspec::Power( U, shifts, invNorms, psCtrl );
    }
    else
        return pspec::HagerHigham( U, shifts, invNorms, psCtrl );
}

template<typename Real>
DistMatrix<Int,VR,STAR> TriangularHelper
( const DistMatrix<Complex<Real>>& U, 
  const DistMatrix<Complex<Real>>& Q, 
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::TriangularHelper"))
    const Grid& g = U.Grid();

    // Check if the off-diagonal is sufficiently small; if so, compute the 
    // pseudospectrum analytically from the eigenvalues. This also takes care
    // of the case where the matrix is a constant multiple of the identity 
    // matrix, which, after shifting, can lead to the zero matrix, which would 
    // cause problems for the Lanczos convergence criteria.
    if( pspec::TriangIsNormal( U, psCtrl.tol ) )
    {
        DistMatrix<Int,VR,STAR> itCounts(g);
        if( psCtrl.progress && g.Rank() == 0 )
            std::cout << "Matrix was numerically normal" << std::endl;
        auto w = U.GetDiagonal();
        if( psCtrl.norm == PS_TWO_NORM )
            pspec::Analytic( w, shifts, invNorms, psCtrl.snapCtrl );
        else
            LogicError("Analytic one-norm pseudospectra not yet supported");
        itCounts.AlignWith( shifts );
        Zeros( itCounts, shifts.Height(), 1 );
        return itCounts;
    }

    psCtrl.schur = true;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.arnoldi )
        {
            if( psCtrl.basisSize > 1 )
                return pspec::IRA( U, shifts, invNorms, psCtrl );
            else
                return pspec::Lanczos( U, shifts, invNorms, psCtrl );
        }
        else
            return pspec::Power( U, shifts, invNorms, psCtrl );
    }
    else
        return pspec::HagerHigham( U, Q, shifts, invNorms, psCtrl );
}

template<typename Real>
DistMatrix<Int,VR,STAR> TriangularHelper
( const DistMatrix<Real>& U, const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::TriangularHelper"))
    DistMatrix<Complex<Real>> UCpx(U.Grid());
    Copy( U, UCpx );

    // TODO: Use a real multi-shift TRSM instead
    return TriangularHelper( UCpx, shifts, invNorms, psCtrl );
}

template<typename Real>
DistMatrix<Int,VR,STAR> TriangularHelper
( const DistMatrix<Real>& U, 
  const DistMatrix<Real>& Q,
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::TriangularHelper"))
    DistMatrix<Complex<Real>> UCpx(U.Grid()), QCpx(U.Grid());
    Copy( U, UCpx );
    Copy( Q, QCpx );

    // TODO: Use a real multi-shift TRSM instead
    return TriangularHelper( UCpx, QCpx, shifts, invNorms, psCtrl );
}

} // namespace pspec

template<typename F>
DistMatrix<Int,VR,STAR> TriangularPseudospectra
( const DistMatrix<F>& U, 
  const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts,
  DistMatrix<Base<F>,VR,STAR>& invNorms, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    return pspec::TriangularHelper( U, shifts, invNorms, psCtrl );
}

template<typename F>
DistMatrix<Int,VR,STAR> TriangularPseudospectra
( const DistMatrix<F>& U, const DistMatrix<F>& Q,
  const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts,
  DistMatrix<Base<F>,VR,STAR>& invNorms, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    return pspec::TriangularHelper( U, Q, shifts, invNorms, psCtrl );
}

template<typename Real>
DistMatrix<Int,VR,STAR> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U, 
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))
    const Grid& g = U.Grid();

    // Check if the off-diagonal is sufficiently small; if so, compute the 
    // pseudospectrum analytically from the eigenvalues. This also takes care
    // of the case where the matrix is a constant multiple of the identity 
    // matrix, which, after shifting, can lead to the zero matrix, which would 
    // cause problems for the Lanczos convergence criteria.
    if( pspec::QuasiTriangIsNormal( U, psCtrl.tol ) )
    {
        DistMatrix<Int,VR,STAR> itCounts(g);
        if( psCtrl.progress && g.Rank() == 0 )
            std::cout << "Matrix was numerically normal" << std::endl;
        auto w = schur::QuasiTriangEig( U );
        if( psCtrl.norm == PS_TWO_NORM )
            pspec::Analytic( w, shifts, invNorms, psCtrl.snapCtrl );
        else
            LogicError("Analytic one-norm pseudospectra not yet supported");
        itCounts.AlignWith( shifts );
        Zeros( itCounts, shifts.Height(), 1 );
        return itCounts;
    }

    psCtrl.schur = true;
    if( psCtrl.norm == PS_ONE_NORM )
        LogicError("This option is not yet written");
    return pspec::IRA( U, shifts, invNorms, psCtrl );
}

template<typename Real>
DistMatrix<Int,VR,STAR> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U, 
  const DistMatrix<Real>& Q,
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))
    const Grid& g = U.Grid();

    // Check if the off-diagonal is sufficiently small; if so, compute the 
    // pseudospectrum analytically from the eigenvalues. This also takes care
    // of the case where the matrix is a constant multiple of the identity 
    // matrix, which, after shifting, can lead to the zero matrix, which would 
    // cause problems for the Lanczos convergence criteria.
    if( pspec::QuasiTriangIsNormal( U, psCtrl.tol ) )
    {
        DistMatrix<Int,VR,STAR> itCounts(g);
        if( psCtrl.progress && g.Rank() == 0 )
            std::cout << "Matrix was numerically normal" << std::endl;
        auto w = schur::QuasiTriangEig( U );
        if( psCtrl.norm == PS_TWO_NORM )
            pspec::Analytic( w, shifts, invNorms, psCtrl.snapCtrl );
        else
            LogicError("Analytic one-norm pseudospectra not yet supported");
        itCounts.AlignWith( shifts );
        Zeros( itCounts, shifts.Height(), 1 );
        return itCounts;
    }

    psCtrl.schur = true;
    if( psCtrl.norm == PS_ONE_NORM )
        LogicError("This option is not yet written");
    return pspec::IRA( U, shifts, invNorms, psCtrl );
}

namespace pspec {

template<typename Real>
DistMatrix<Int,VR,STAR> HessenbergHelper
( const DistMatrix<Complex<Real>>& H, 
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::HessenbergHelper"))

    // TODO: Check if the subdiagonal is sufficiently small, and, if so, revert
    //       to TriangularPseudospectra
    psCtrl.schur = false;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.arnoldi )
        {
            if( psCtrl.basisSize > 1 )
                return pspec::IRA( H, shifts, invNorms, psCtrl );
            else
                return pspec::Lanczos( H, shifts, invNorms, psCtrl );
        }
        else
            return pspec::Power( H, shifts, invNorms, psCtrl );
    }
    else
        return pspec::HagerHigham( H, shifts, invNorms, psCtrl );
}

template<typename Real>
DistMatrix<Int,VR,STAR> HessenbergHelper
( const DistMatrix<Complex<Real>>& H, 
  const DistMatrix<Complex<Real>>& Q,
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::HessenbergHelper"))

    // TODO: Check if the subdiagonal is sufficiently small, and, if so, revert
    //       to TriangularPseudospectra
    psCtrl.schur = false;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.arnoldi )
        {
            if( psCtrl.basisSize > 1 )
                return pspec::IRA( H, shifts, invNorms, psCtrl );
            else
                return pspec::Lanczos( H, shifts, invNorms, psCtrl );
        }
        else
            return pspec::Power( H, shifts, invNorms, psCtrl );
    }
    else
        return pspec::HagerHigham( H, Q, shifts, invNorms, psCtrl );
}

template<typename Real>
DistMatrix<Int,VR,STAR> HessenbergHelper
( const DistMatrix<Real>& H, const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::HessenbergHelper"))
    DistMatrix<Complex<Real>> HCpx(H.Grid());
    Copy( H, HCpx );

    return HessenbergHelper( HCpx, shifts, invNorms, psCtrl );
}

template<typename Real>
DistMatrix<Int,VR,STAR> HessenbergHelper
( const DistMatrix<Real>& H, 
  const DistMatrix<Real>& Q,
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::HessenbergHelper"))
    DistMatrix<Complex<Real>> HCpx(H.Grid()), QCpx(H.Grid());
    Copy( H, HCpx );
    Copy( Q, QCpx );

    return HessenbergHelper( HCpx, QCpx, shifts, invNorms, psCtrl );
}

} // namespace pspec

template<typename F>
DistMatrix<Int,VR,STAR> HessenbergPseudospectra
( const DistMatrix<F>& H, 
  const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts,
  DistMatrix<Base<F>,VR,STAR>& invNorms, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    return pspec::HessenbergHelper( H, shifts, invNorms, psCtrl );
}

template<typename F>
DistMatrix<Int,VR,STAR> HessenbergPseudospectra
( const DistMatrix<F>& H, const DistMatrix<F>& Q,
  const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts,
  DistMatrix<Base<F>,VR,STAR>& invNorms, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    return pspec::HessenbergHelper( H, Q, shifts, invNorms, psCtrl );
}

namespace pspec {

template<typename Real>
Matrix<Int> Helper
( const Matrix<Real>& A, const Matrix<Complex<Real>>& shifts, 
  Matrix<Real>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::Helper"))
    typedef Complex<Real> C;

    if( psCtrl.forceComplexSchur )
    {
        Matrix<C> ACpx;
        Copy( A, ACpx );
        return Pseudospectra( ACpx, shifts, invNorms, psCtrl );
    }

    if( !psCtrl.schur )
        LogicError("Real Hessenberg algorithm not yet supported");
    Matrix<Real> U( A );
    Matrix<C> w;
    const bool fullTriangle = true;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        Schur( U, w, fullTriangle, psCtrl.schurCtrl );
        if( psCtrl.forceComplexPs )
        {
            Matrix<C> UCpx;
            schur::RealToComplex( U, UCpx );
            return TriangularPseudospectra( UCpx, shifts, invNorms, psCtrl );
        }
        return QuasiTriangularPseudospectra( U, shifts, invNorms, psCtrl );
    }
    else
    {
        Matrix<Real> Q;
        Schur( U, w, Q, fullTriangle, psCtrl.schurCtrl );
        if( psCtrl.forceComplexPs )
        {
            LogicError("Real to complex full Schur not yet supported");
            /*
            Matrix<C> UCpx, QCpx;
            schur::RealToComplex( U, Q, UCpx, QCpx );
            return TriangularPseudospectra
                   ( UCpx, QCpx, shifts, invNorms, psCtrl );
            */
        }
        return QuasiTriangularPseudospectra( U, Q, shifts, invNorms, psCtrl );
    }
}

template<typename Real>
DistMatrix<Int,VR,STAR> Helper
( const DistMatrix<Real>& A, const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::Helper"))
    typedef Complex<Real> C;
    const Grid& g = A.Grid();

    if( psCtrl.forceComplexSchur )
    {
        DistMatrix<C> ACpx(g);
        Copy( A, ACpx );
        return Pseudospectra( ACpx, shifts, invNorms, psCtrl );
    }

    if( !psCtrl.schur )
        LogicError("Real Hessenberg algorithm not yet supported");
    DistMatrix<Real> U( A );
    DistMatrix<C,VR,STAR> w(g);
    const bool fullTriangle = true;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        Schur( U, w, fullTriangle, psCtrl.schurCtrl );
        if( psCtrl.forceComplexPs )
        {
            DistMatrix<C> UCpx(g);
            schur::RealToComplex( U, UCpx );
            return TriangularPseudospectra( UCpx, shifts, invNorms, psCtrl );
        }
        return QuasiTriangularPseudospectra( U, shifts, invNorms, psCtrl );
    }
    else
    {
        DistMatrix<Real> Q(g);
        Schur( U, w, Q, fullTriangle, psCtrl.schurCtrl );
        if( psCtrl.forceComplexPs )
        {
            LogicError("Real to complex full Schur not yet supported");
            /*
            DistMatrix<C> UCpx(g), QCpx(g);
            schur::RealToComplex( U, Q, UCpx, QCpx );
            return TriangularPseudospectra
                   ( UCpx, QCpx, shifts, invNorms, psCtrl );
            */
        }
        return QuasiTriangularPseudospectra( U, Q, shifts, invNorms, psCtrl );
    }
}

template<typename Real>
Matrix<Int> Helper
( const Matrix<Complex<Real>>& A, const Matrix<Complex<Real>>& shifts, 
  Matrix<Real>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::Helper"))
    typedef Complex<Real> C;

    Matrix<C> U( A );
    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.schur )
        {
            Matrix<C> w;
            const bool fullTriangle = true;
            Schur( U, w, fullTriangle, psCtrl.schurCtrl );
            return TriangularPseudospectra( U, shifts, invNorms, psCtrl );
        }
        else
        {
            Hessenberg( UPPER, U );
            return HessenbergPseudospectra( U, shifts, invNorms, psCtrl );
        }
    }
    else
    {
        Matrix<C> Q;
        if( psCtrl.schur )
        {
            Matrix<C> w;
            const bool fullTriangle = true;
            Schur( U, w, Q, fullTriangle, psCtrl.schurCtrl );
            return TriangularPseudospectra( U, Q, shifts, invNorms, psCtrl );
        }
        else
        {
            Matrix<C> t;
            Hessenberg( UPPER, U, t );
            Identity( Q, A.Height(), A.Height() );
            hessenberg::ApplyQ( LEFT, UPPER, NORMAL, U, t, Q );
            return HessenbergPseudospectra( U, Q, shifts, invNorms, psCtrl );
        }
    }
}

template<typename Real>
DistMatrix<Int,VR,STAR> Helper
( const DistMatrix<Complex<Real>>& A, 
  const DistMatrix<Complex<Real>,VR,STAR>& shifts,
  DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::Helper"))
    typedef Complex<Real> C;

    const Grid& g = A.Grid();
    DistMatrix<C> U( A );

    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.schur )
        {
            DistMatrix<C,VR,STAR> w(g);
            const bool fullTriangle = true;
            Schur( U, w, fullTriangle, psCtrl.schurCtrl );
            return TriangularPseudospectra( U, shifts, invNorms, psCtrl );
        }
        else
        {
            Hessenberg( UPPER, U );
            return HessenbergPseudospectra( U, shifts, invNorms, psCtrl );
        }
    }
    else
    {
        DistMatrix<C> Q(g);
        if( psCtrl.schur )
        {
            DistMatrix<C,VR,STAR> w(g);
            const bool fullTriangle = true;
            Schur( U, w, Q, fullTriangle, psCtrl.schurCtrl );
            return TriangularPseudospectra( U, Q, shifts, invNorms, psCtrl );
        }
        else
        {
            DistMatrix<C,STAR,STAR> t(g);
            Hessenberg( UPPER, U, t );
            Identity( Q, U.Height(), U.Height() );
            hessenberg::ApplyQ( LEFT, UPPER, NORMAL, U, t, Q );
            return HessenbergPseudospectra( U, Q, shifts, invNorms, psCtrl );
        }
    }
}

} // namespace pspec

template<typename F>
Matrix<Int> Pseudospectra
( const Matrix<F>& A, const Matrix<Complex<Base<F>>>& shifts,
  Matrix<Base<F>>& invNorms, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("Pseudospectra"))
    return pspec::Helper( A, shifts, invNorms, psCtrl );
}

template<typename F>
DistMatrix<Int,VR,STAR> Pseudospectra
( const DistMatrix<F>& A, const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts,
  DistMatrix<Base<F>,VR,STAR>& invNorms, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("Pseudospectra"))
    return pspec::Helper( A, shifts, invNorms, psCtrl );
}

// Treat each pixel as being located a cell center and tesselate a box with
// said square cells
template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, Matrix<Base<F>>& invNormMap, 
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    typedef Base<F> Real;
    typedef Complex<Real> C;

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    Matrix<C> shifts( realSize*imagSize, 1 );
    for( Int j=0; j<realSize*imagSize; ++j )
    {
        const Int x = j / imagSize;
        const Int y = j % imagSize;
        shifts.Set( j, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    Matrix<Real> invNorms;
    auto itCounts = 
        TriangularPseudospectra( U, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    Matrix<Int> itCountMap; 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap, 
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    typedef Base<F> Real;
    typedef Complex<Real> C;

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    Matrix<C> shifts( realSize*imagSize, 1 );
    for( Int j=0; j<realSize*imagSize; ++j )
    {
        const Int x = j / imagSize;
        const Int y = j % imagSize;
        shifts.Set( j, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    Matrix<Real> invNorms;
    auto itCounts = 
        TriangularPseudospectra( U, Q, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    Matrix<Int> itCountMap; 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U, Matrix<Real>& invNormMap, 
  Complex<Real> center, Real realWidth, Real imagWidth,
  Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))
    typedef Complex<Real> C;

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    Matrix<C> shifts( realSize*imagSize, 1 );
    for( Int j=0; j<realSize*imagSize; ++j )
    {
        const Int x = j / imagSize;
        const Int y = j % imagSize;
        shifts.Set( j, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    Matrix<Real> invNorms;
    auto itCounts = 
        QuasiTriangularPseudospectra( U, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    Matrix<Int> itCountMap; 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U, const Matrix<Real>& Q, Matrix<Real>& invNormMap, 
  Complex<Real> center, Real realWidth, Real imagWidth,
  Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))
    typedef Complex<Real> C;

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    Matrix<C> shifts( realSize*imagSize, 1 );
    for( Int j=0; j<realSize*imagSize; ++j )
    {
        const Int x = j / imagSize;
        const Int y = j % imagSize;
        shifts.Set( j, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    Matrix<Real> invNorms;
    auto itCounts = 
        QuasiTriangularPseudospectra( U, Q, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    Matrix<Int> itCountMap; 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, Matrix<Base<F>>& invNormMap, 
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    typedef Base<F> Real;
    typedef Complex<Real> C;

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    Matrix<C> shifts( realSize*imagSize, 1 );
    for( Int j=0; j<realSize*imagSize; ++j )
    {
        const Int x = j / imagSize;
        const Int y = j % imagSize;
        shifts.Set( j, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    Matrix<Real> invNorms;
    auto itCounts = HessenbergPseudospectra( H, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    Matrix<Int> itCountMap; 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap, 
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    typedef Base<F> Real;
    typedef Complex<Real> C;

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    Matrix<C> shifts( realSize*imagSize, 1 );
    for( Int j=0; j<realSize*imagSize; ++j )
    {
        const Int x = j / imagSize;
        const Int y = j % imagSize;
        shifts.Set( j, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    Matrix<Real> invNorms;
    auto itCounts = HessenbergPseudospectra( H, Q, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    Matrix<Int> itCountMap; 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename F>
DistMatrix<Int> TriangularPseudospectra
( const DistMatrix<F>& U, DistMatrix<Base<F>>& invNormMap, 
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    typedef Base<F> Real;
    typedef Complex<Real> C;
    const Grid& g = U.Grid();

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    DistMatrix<C,VR,STAR> shifts( realSize*imagSize, 1, g );
    const Int numLocShifts = shifts.LocalHeight();
    for( Int iLoc=0; iLoc<numLocShifts; ++iLoc )
    {
        const Int i = shifts.GlobalRow(iLoc);
        const Int x = i / imagSize;
        const Int y = i % imagSize;
        shifts.SetLocal
        ( iLoc, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    DistMatrix<Real,VR,STAR> invNorms(g);
    auto itCounts = TriangularPseudospectra( U, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    DistMatrix<Int> itCountMap(g); 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename F>
DistMatrix<Int> TriangularPseudospectra
( const DistMatrix<F>& U, const DistMatrix<F>& Q, 
  DistMatrix<Base<F>>& invNormMap, 
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    typedef Base<F> Real;
    typedef Complex<Real> C;
    const Grid& g = U.Grid();

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    DistMatrix<C,VR,STAR> shifts( realSize*imagSize, 1, g );
    const Int numLocShifts = shifts.LocalHeight();
    for( Int iLoc=0; iLoc<numLocShifts; ++iLoc )
    {
        const Int i = shifts.GlobalRow(iLoc);
        const Int x = i / imagSize;
        const Int y = i % imagSize;
        shifts.SetLocal
        ( iLoc, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    DistMatrix<Real,VR,STAR> invNorms(g);
    auto itCounts = TriangularPseudospectra( U, Q, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    DistMatrix<Int> itCountMap(g); 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename Real>
DistMatrix<Int> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U, DistMatrix<Real>& invNormMap, 
  Complex<Real> center, Real realWidth, Real imagWidth, 
  Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))
    typedef Complex<Real> C;
    const Grid& g = U.Grid();

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    DistMatrix<C,VR,STAR> shifts( realSize*imagSize, 1, g );
    const Int numLocShifts = shifts.LocalHeight();
    for( Int iLoc=0; iLoc<numLocShifts; ++iLoc )
    {
        const Int i = shifts.GlobalRow(iLoc);
        const Int x = i / imagSize;
        const Int y = i % imagSize;
        shifts.SetLocal
        ( iLoc, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    DistMatrix<Real,VR,STAR> invNorms(g);
    auto itCounts = 
        QuasiTriangularPseudospectra( U, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    DistMatrix<Int> itCountMap(g); 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename Real>
DistMatrix<Int> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U, const DistMatrix<Real>& Q,
  DistMatrix<Real>& invNormMap, 
  Complex<Real> center, Real realWidth, Real imagWidth, 
  Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))
    typedef Complex<Real> C;
    const Grid& g = U.Grid();

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    DistMatrix<C,VR,STAR> shifts( realSize*imagSize, 1, g );
    const Int numLocShifts = shifts.LocalHeight();
    for( Int iLoc=0; iLoc<numLocShifts; ++iLoc )
    {
        const Int i = shifts.GlobalRow(iLoc);
        const Int x = i / imagSize;
        const Int y = i % imagSize;
        shifts.SetLocal
        ( iLoc, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    DistMatrix<Real,VR,STAR> invNorms(g);
    auto itCounts = 
        QuasiTriangularPseudospectra( U, Q, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    DistMatrix<Int> itCountMap(g); 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename F>
DistMatrix<Int> HessenbergPseudospectra
( const DistMatrix<F>& H, DistMatrix<Base<F>>& invNormMap, 
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    typedef Base<F> Real;
    typedef Complex<Real> C;
    const Grid& g = H.Grid();

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    DistMatrix<C,VR,STAR> shifts( realSize*imagSize, 1, g );
    const Int numLocShifts = shifts.LocalHeight();
    for( Int iLoc=0; iLoc<numLocShifts; ++iLoc )
    {
        const Int i = shifts.GlobalRow(iLoc);
        const Int x = i / imagSize;
        const Int y = i % imagSize;
        shifts.SetLocal
        ( iLoc, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    DistMatrix<Real,VR,STAR> invNorms(g);
    auto itCounts = HessenbergPseudospectra( H, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    DistMatrix<Int> itCountMap(g); 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename F>
DistMatrix<Int> HessenbergPseudospectra
( const DistMatrix<F>& H, 
  const DistMatrix<F>& Q,
  DistMatrix<Base<F>>& invNormMap, 
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    typedef Base<F> Real;
    typedef Complex<Real> C;
    const Grid& g = H.Grid();

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    DistMatrix<C,VR,STAR> shifts( realSize*imagSize, 1, g );
    const Int numLocShifts = shifts.LocalHeight();
    for( Int iLoc=0; iLoc<numLocShifts; ++iLoc )
    {
        const Int i = shifts.GlobalRow(iLoc);
        const Int x = i / imagSize;
        const Int y = i % imagSize;
        shifts.SetLocal
        ( iLoc, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    DistMatrix<Real,VR,STAR> invNorms(g);
    auto itCounts = HessenbergPseudospectra( H, Q, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    DistMatrix<Int> itCountMap(g); 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename F>
Matrix<Int> Pseudospectra
( const Matrix<F>& A, Matrix<Base<F>>& invNormMap, 
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth,
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("Pseudospectra"))
    typedef Base<F> Real;
    typedef Complex<Real> C;

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    Matrix<C> shifts( realSize*imagSize, 1 );
    for( Int j=0; j<realSize*imagSize; ++j )
    {
        const Int x = j / imagSize;
        const Int y = j % imagSize;
        shifts.Set( j, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    Matrix<Real> invNorms;
    auto itCounts = Pseudospectra( A, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    Matrix<Int> itCountMap; 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename F>
DistMatrix<Int> Pseudospectra
( const DistMatrix<F>& A, DistMatrix<Base<F>>& invNormMap, 
  Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("Pseudospectra"))
    typedef Base<F> Real;
    typedef Complex<Real> C;
    const Grid& g = A.Grid();

    psCtrl.snapCtrl.realSize = realSize;
    psCtrl.snapCtrl.imagSize = imagSize;

    const Real realStep = realWidth/realSize;
    const Real imagStep = imagWidth/imagSize;
    const C corner = center + C(-realWidth/2,imagWidth/2);
    DistMatrix<C,VR,STAR> shifts( realSize*imagSize, 1, g );
    const Int numLocShifts = shifts.LocalHeight();
    for( Int iLoc=0; iLoc<numLocShifts; ++iLoc )
    {
        const Int i = shifts.GlobalRow(iLoc);
        const Int x = i / imagSize;
        const Int y = i % imagSize;
        shifts.SetLocal
        ( iLoc, 0, corner+C((x+0.5)*realStep,-(y+0.5)*imagStep) );
    }

    // Form the vector of invNorms
    DistMatrix<Real,VR,STAR> invNorms(g);
    auto itCounts = Pseudospectra( A, shifts, invNorms, psCtrl );

    // Rearrange the vectors into grids
    DistMatrix<Int> itCountMap(g); 
    pspec::ReshapeIntoGrid( realSize, imagSize, invNorms, invNormMap );
    pspec::ReshapeIntoGrid( realSize, imagSize, itCounts, itCountMap );
    return itCountMap;
}

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, Matrix<Base<F>>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    typedef Base<F> Real;
    auto diag = U.GetDiagonal();
    const Real radius = MaxNorm( diag );
    const Real oneNorm = OneNorm( U );

    // Essentially three cases are handled here:
    // 1) The zero matrix (force the pseudospectrum width to 1)
    // 2) Typical matrices (use a small multiple of the spectral radius)
    // 3) Highly non-normal matrices (e.g., triangular with zero main diagonal)
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress )
            std::cout << "Setting width to 1 to handle zero matrix" 
                      << std::endl;
    }
    else if( radius >= 0.2*oneNorm )
    {
        width = 2.5*radius;
        if( psCtrl.progress )
            std::cout << "Setting width to " << width 
                      << " based on the spectral radius, " << radius 
                      << std::endl;
    }
    else
    {
        width = 0.8*oneNorm;
        if( psCtrl.progress )
            std::cout << "Setting width to " << width 
                      << " based on the one norm, " << oneNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return TriangularPseudospectra
           ( U, invNormMap, center, width, width, realSize, imagSize, psCtrl );
}

template<typename F>
Matrix<Int> TriangularPseudospectra
( const Matrix<F>& U, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    typedef Base<F> Real;
    auto diag = U.GetDiagonal();
    const Real radius = MaxNorm( diag );
    const Real oneNorm = OneNorm( U );

    // Essentially three cases are handled here:
    // 1) The zero matrix (force the pseudospectrum width to 1)
    // 2) Typical matrices (use a small multiple of the spectral radius)
    // 3) Highly non-normal matrices (e.g., triangular with zero main diagonal)
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress )
            std::cout << "Setting width to 1 to handle zero matrix" 
                      << std::endl;
    }
    else if( radius >= oneNorm/5 )
    {
        width = 2.5*radius;
        if( psCtrl.progress )
            std::cout << "Setting width to " << width 
                      << " based on the spectral radius, " << radius 
                      << std::endl;
    }
    else
    {
        width = 0.8*oneNorm;
        if( psCtrl.progress )
            std::cout << "Setting width to " << width 
                      << " based on the one norm, " << oneNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return TriangularPseudospectra
           ( U, Q, invNormMap, center, width, width, realSize, imagSize, 
             psCtrl );
}

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U, Matrix<Real>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))

    const auto w = schur::QuasiTriangEig( U );
    const Real radius = MaxNorm( w );
    const Real oneNorm = OneNorm( U );

    // Essentially three cases are handled here:
    // 1) The zero matrix (force the pseudospectrum width to 1)
    // 2) Typical matrices (use a small multiple of the spectral radius)
    // 3) Highly non-normal matrices (e.g., triangular with zero main diagonal)
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress )
            std::cout << "Setting width to 1 to handle zero matrix" 
                      << std::endl;
    }
    else if( radius >= 0.2*oneNorm )
    {
        width = 2.5*radius;
        if( psCtrl.progress )
            std::cout << "Setting width to " << width 
                      << " based on the spectral radius, " << radius 
                      << std::endl;
    }
    else
    {
        width = 0.8*oneNorm;
        if( psCtrl.progress )
            std::cout << "Setting width to " << width 
                      << " based on the one norm, " << oneNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return QuasiTriangularPseudospectra
           ( U, invNormMap, center, width, width, realSize, imagSize, psCtrl );
}

template<typename Real>
Matrix<Int> QuasiTriangularPseudospectra
( const Matrix<Real>& U, const Matrix<Real>& Q,
  Matrix<Real>& invNormMap, Int realSize, Int imagSize, 
  PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))
 
    const auto w = schur::QuasiTriangEig( U );
    const Real radius = MaxNorm( w );
    const Real oneNorm = OneNorm( U );

    // Essentially three cases are handled here:
    // 1) The zero matrix (force the pseudospectrum width to 1)
    // 2) Typical matrices (use a small multiple of the spectral radius)
    // 3) Highly non-normal matrices (e.g., triangular with zero main diagonal)
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress )
            std::cout << "Setting width to 1 to handle zero matrix" 
                      << std::endl;
    }
    else if( radius >= 0.2*oneNorm )
    {
        width = 2.5*radius;
        if( psCtrl.progress )
            std::cout << "Setting width to " << width 
                      << " based on the spectral radius, " << radius 
                      << std::endl;
    }
    else
    {
        width = 0.8*oneNorm;
        if( psCtrl.progress )
            std::cout << "Setting width to " << width 
                      << " based on the one norm, " << oneNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return QuasiTriangularPseudospectra
           ( U, Q, invNormMap, center, width, width, realSize, imagSize, 
             psCtrl );
}

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, Matrix<Base<F>>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    typedef Base<F> Real;
    const Real infNorm = InfinityNorm( H );
    const Real oneNorm = OneNorm( H );
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress )
            std::cout << "Setting width to 1 to handle zero matrix" 
                      << std::endl;
    }
    else
    {
        width = 0.8*Max(oneNorm,infNorm);
        if( psCtrl.progress )
            std::cout << "Setting width to " << width 
                      << " based on the one norm, " << oneNorm 
                      << ", and infinity norm, " << infNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return HessenbergPseudospectra
           ( H, invNormMap, center, width, width, realSize, imagSize, psCtrl );
}

template<typename F>
Matrix<Int> HessenbergPseudospectra
( const Matrix<F>& H, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    typedef Base<F> Real;
    const Real infNorm = InfinityNorm( H );
    const Real oneNorm = OneNorm( H );
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress )
            std::cout << "Setting width to 1 to handle zero matrix" 
                      << std::endl;
    }
    else
    {
        width = 0.8*Max(oneNorm,infNorm);
        if( psCtrl.progress )
            std::cout << "Setting width to " << width 
                      << " based on the one norm, " << oneNorm 
                      << ", and infinity norm, " << infNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return HessenbergPseudospectra
           ( H, Q, invNormMap, center, width, width, realSize, imagSize, 
             psCtrl );
}

template<typename F>
DistMatrix<Int> TriangularPseudospectra
( const DistMatrix<F>& U, DistMatrix<Base<F>>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    typedef Base<F> Real;
    auto diag = U.GetDiagonal();
    const Real radius = MaxNorm( diag );
    const Real oneNorm = OneNorm( U );

    // Essentially three cases are handled here:
    // 1) The zero matrix (force the pseudospectrum width to 1)
    // 2) Typical matrices (use a small multiple of the spectral radius)
    // 3) Highly non-normal matrices (e.g., triangular with zero main diagonal)
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to 1 to handle zero matrix"
                      << std::endl;
    }
    else if( radius >= 0.2*oneNorm )
    {
        width = 2.5*radius;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to " << width 
                      << " based on the spectral radius, " << radius 
                      << std::endl;
    }
    else
    {
        width = 0.8*oneNorm;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to " << width
                      << " based on the one norm, " << oneNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return TriangularPseudospectra
           ( U, invNormMap, center, width, width, realSize, imagSize, psCtrl );
}

template<typename F>
DistMatrix<Int> TriangularPseudospectra
( const DistMatrix<F>& U, const DistMatrix<F>& Q, 
  DistMatrix<Base<F>>& invNormMap, Int realSize, Int imagSize,
  PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("TriangularPseudospectra"))
    typedef Base<F> Real;
    auto diag = U.GetDiagonal();
    const Real radius = MaxNorm( diag );
    const Real oneNorm = OneNorm( U );

    // Essentially three cases are handled here:
    // 1) The zero matrix (force the pseudospectrum width to 1)
    // 2) Typical matrices (use a small multiple of the spectral radius)
    // 3) Highly non-normal matrices (e.g., triangular with zero main diagonal)
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to 1 to handle zero matrix"
                      << std::endl;
    }
    else if( radius >= 0.2*oneNorm )
    {
        width = 2.5*radius;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to " << width 
                      << " based on the spectral radius, " << radius 
                      << std::endl;
    }
    else
    {
        width = 0.8*oneNorm;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to " << width
                      << " based on the one norm, " << oneNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return TriangularPseudospectra
           ( U, Q, invNormMap, center, width, width, realSize, imagSize, 
             psCtrl );
}

template<typename Real>
DistMatrix<Int> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U, DistMatrix<Real>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))

    const auto w = schur::QuasiTriangEig( U );
    const Real radius = MaxNorm( w );
    const Real oneNorm = OneNorm( U );

    // Essentially three cases are handled here:
    // 1) The zero matrix (force the pseudospectrum width to 1)
    // 2) Typical matrices (use a small multiple of the spectral radius)
    // 3) Highly non-normal matrices (e.g., triangular with zero main diagonal)
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to 1 to handle zero matrix"
                      << std::endl;
    }
    else if( radius >= 0.2*oneNorm )
    {
        width = 2.5*radius;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to " << width 
                      << " based on the spectral radius, " << radius 
                      << std::endl;
    }
    else
    {
        width = 0.8*oneNorm;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to " << width
                      << " based on the one norm, " << oneNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return QuasiTriangularPseudospectra
           ( U, invNormMap, center, width, width, realSize, imagSize, psCtrl );
}

template<typename Real>
DistMatrix<Int> QuasiTriangularPseudospectra
( const DistMatrix<Real>& U, const DistMatrix<Real>& Q,
  DistMatrix<Real>& invNormMap, Int realSize, Int imagSize,
  PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("QuasiTriangularPseudospectra"))

    const auto w = schur::QuasiTriangEig( U );
    const Real radius = MaxNorm( w );
    const Real oneNorm = OneNorm( U );

    // Essentially three cases are handled here:
    // 1) The zero matrix (force the pseudospectrum width to 1)
    // 2) Typical matrices (use a small multiple of the spectral radius)
    // 3) Highly non-normal matrices (e.g., triangular with zero main diagonal)
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to 1 to handle zero matrix"
                      << std::endl;
    }
    else if( radius >= 0.2*oneNorm )
    {
        width = 2.5*radius;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to " << width 
                      << " based on the spectral radius, " << radius 
                      << std::endl;
    }
    else
    {
        width = 0.8*oneNorm;
        if( psCtrl.progress && U.Grid().Rank() == 0 )
            std::cout << "Setting width to " << width
                      << " based on the one norm, " << oneNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return QuasiTriangularPseudospectra
           ( U, Q, invNormMap, center, width, width, realSize, imagSize, 
             psCtrl );
}

template<typename F>
DistMatrix<Int> HessenbergPseudospectra
( const DistMatrix<F>& H, DistMatrix<Base<F>>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    typedef Base<F> Real;
    const Real oneNorm = OneNorm( H );
    const Real infNorm = InfinityNorm( H );
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress && H.Grid().Rank() == 0 )
            std::cout << "Setting width to 1 to handle zero matrix"
                      << std::endl;
    }
    else
    {
        width = 0.8*Max(oneNorm,infNorm);
        if( psCtrl.progress && H.Grid().Rank() == 0 )
            std::cout << "Setting width to " << width 
                      << " based on the one norm, " << oneNorm 
                      << ", and infinity norm, " << infNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return HessenbergPseudospectra
           ( H, invNormMap, center, width, width, realSize, imagSize, psCtrl );
}

template<typename F>
DistMatrix<Int> HessenbergPseudospectra
( const DistMatrix<F>& H, 
  const DistMatrix<F>& Q, DistMatrix<Base<F>>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("HessenbergPseudospectra"))
    typedef Base<F> Real;
    const Real oneNorm = OneNorm( H );
    const Real infNorm = InfinityNorm( H );
    Real width;
    if( oneNorm == Real(0) )
    {
        width = 1;
        if( psCtrl.progress && H.Grid().Rank() == 0 )
            std::cout << "Setting width to 1 to handle zero matrix"
                      << std::endl;
    }
    else
    {
        width = 0.8*Max(oneNorm,infNorm);
        if( psCtrl.progress && H.Grid().Rank() == 0 )
            std::cout << "Setting width to " << width 
                      << " based on the one norm, " << oneNorm 
                      << ", and infinity norm, " << infNorm << std::endl;
    }
    Complex<Real> center(0,0);

    return HessenbergPseudospectra
           ( H, Q, invNormMap, center, width, width, realSize, imagSize, 
             psCtrl );
}

namespace pspec {

template<typename Real>
Matrix<Int> Helper
( const Matrix<Complex<Real>>& A, Matrix<Real>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::Helper"))
    typedef Complex<Real> C;

    Matrix<C> B( A );
    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.schur )
        {
            Matrix<C> w;
            const bool fullTriangle = true;
            Schur( B, w, fullTriangle, psCtrl.schurCtrl );
            return TriangularPseudospectra
                   ( B, invNormMap, realSize, imagSize, psCtrl );
        }
        else
        {
            Hessenberg( UPPER, B );
            return HessenbergPseudospectra
                   ( B, invNormMap, realSize, imagSize, psCtrl );
        }
    }
    else
    {
        Matrix<C> Q;
        if( psCtrl.schur )
        {
            Matrix<C> w;
            const bool fullTriangle = true;
            Schur( B, w, Q, fullTriangle, psCtrl.schurCtrl );
            return TriangularPseudospectra
                   ( B, Q, invNormMap, realSize, imagSize, psCtrl );
        }
        else
        {
            Matrix<C> t;
            Hessenberg( UPPER, B, t );
            Identity( Q, B.Height(), B.Height() );
            hessenberg::ApplyQ( LEFT, UPPER, NORMAL, B, t, Q );
            return HessenbergPseudospectra
                   ( B, Q, invNormMap, realSize, imagSize, psCtrl );
        }
    }
}

template<typename Real>
DistMatrix<Int> Helper
( const DistMatrix<Complex<Real>>& A, DistMatrix<Real>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::Helper"))
    typedef Complex<Real> C;

    const Grid& g = A.Grid();
    DistMatrix<C> B( A );

    if( psCtrl.norm == PS_TWO_NORM )
    {
        if( psCtrl.schur )
        {
            DistMatrix<C,VR,STAR> w(g);
            const bool fullTriangle = true;
            Schur( B, w, fullTriangle, psCtrl.schurCtrl );
            return TriangularPseudospectra
                   ( B, invNormMap, realSize, imagSize, psCtrl );
        }
        else
        {
            Hessenberg( UPPER, B );
            return HessenbergPseudospectra
                   ( B, invNormMap, realSize, imagSize, psCtrl );
        }
    }
    else
    {
        DistMatrix<C> Q(g);
        if( psCtrl.schur )
        {
            DistMatrix<C,VR,STAR> w(g);
            const bool fullTriangle = true;
            Schur( B, w, Q, fullTriangle, psCtrl.schurCtrl );
            return TriangularPseudospectra
                   ( B, Q, invNormMap, realSize, imagSize, psCtrl );
        }
        else
        {
            DistMatrix<C,STAR,STAR> t(g);
            Hessenberg( UPPER, B, t );
            Identity( Q, B.Height(), B.Height() );
            hessenberg::ApplyQ( LEFT, UPPER, NORMAL, B, t, Q );
            return HessenbergPseudospectra
                   ( B, Q, invNormMap, realSize, imagSize, psCtrl );
        }
    }
}

template<typename Real>
Matrix<Int> Helper
( const Matrix<Real>& A, Matrix<Real>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::Helper"))
    typedef Complex<Real> C;

    if( psCtrl.forceComplexSchur )
    {
        Matrix<C> ACpx;
        Copy( A, ACpx );
        return Pseudospectra( ACpx, invNormMap, realSize, imagSize, psCtrl );
    }

    if( !psCtrl.schur )
        LogicError("Real Hessenberg algorithm not yet supported");
    Matrix<Real> B( A );
    Matrix<C> w;
    const bool fullTriangle = true;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        Schur( B, w, fullTriangle, psCtrl.schurCtrl );
        if( psCtrl.forceComplexPs )
        {
            Matrix<C> BCpx;
            schur::RealToComplex( B, BCpx );
            return TriangularPseudospectra
                   ( BCpx, invNormMap, realSize, imagSize, psCtrl );
        }
        return QuasiTriangularPseudospectra
               ( B, invNormMap, realSize, imagSize, psCtrl );
    }
    else
    {
        Matrix<Real> Q;
        Schur( B, w, Q, fullTriangle, psCtrl.schurCtrl );
        if( psCtrl.forceComplexPs )
        {
            LogicError("Real to complex full Schur not yet supported");
            /*
            Matrix<C> BCpx, QCpx;
            schur::RealToComplex( B, Q, BCpx, QCpx );
            return TriangularPseudospectra
                   ( BCpx, invNormMap, realSize, imagSize, psCtrl );
            */
        }
        return QuasiTriangularPseudospectra
               ( B, Q, invNormMap, realSize, imagSize, psCtrl );
    }
}

template<typename Real>
DistMatrix<Int> Helper
( const DistMatrix<Real>& A, DistMatrix<Real>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("pspec::Helper"))
    typedef Complex<Real> C;
    const Grid& g = A.Grid();

    if( psCtrl.forceComplexSchur )
    {
        DistMatrix<C> ACpx(g);
        Copy( A, ACpx );
        return Pseudospectra( ACpx, invNormMap, realSize, imagSize, psCtrl );
    }

    if( !psCtrl.schur )
        LogicError("Real Hessenberg algorithm not yet supported");
    DistMatrix<Real> B( A );

    DistMatrix<C,VR,STAR> w(g);
    const bool fullTriangle = true;
    if( psCtrl.norm == PS_TWO_NORM )
    {
        Schur( B, w, fullTriangle, psCtrl.schurCtrl );
        if( psCtrl.forceComplexPs ) 
        {
            DistMatrix<C> BCpx(g);
            schur::RealToComplex( B, BCpx );
            return TriangularPseudospectra
                   ( BCpx, invNormMap, realSize, imagSize, psCtrl );
        }
        return QuasiTriangularPseudospectra
               ( B, invNormMap, realSize, imagSize, psCtrl );
    }
    else
    {
        DistMatrix<Real> Q(g); 
        Schur( B, w, Q, fullTriangle, psCtrl.schurCtrl );
        if( psCtrl.forceComplexPs ) 
        {
            LogicError("Real to complex full Schur not yet supported");
            /*
            DistMatrix<C> BCpx(g), QCpx(g);
            schur::RealToComplex( B, Q, BCpx, QCpx );
            return TriangularPseudospectra
                   ( BCpx, QCpx, invNormMap, realSize, imagSize, psCtrl );
            */
        }
        return QuasiTriangularPseudospectra
               ( B, Q, invNormMap, realSize, imagSize, psCtrl );
    }
}

} // namespace pspec

template<typename F>
Matrix<Int> Pseudospectra
( const Matrix<F>& A, Matrix<Base<F>>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("Pseudospectra"))
    return pspec::Helper( A, invNormMap, realSize, imagSize, psCtrl );
}

template<typename F>
DistMatrix<Int> Pseudospectra
( const DistMatrix<F>& A, DistMatrix<Base<F>>& invNormMap, 
  Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl )
{
    DEBUG_ONLY(CallStackEntry cse("Pseudospectra"))
    return pspec::Helper( A, invNormMap, realSize, imagSize, psCtrl );
}

#define PROTO(F) \
  template Matrix<Int> Pseudospectra \
  ( const Matrix<F>& A, const Matrix<Complex<Base<F>>>& shifts, \
    Matrix<Base<F>>& invNorms, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int,VR,STAR> Pseudospectra \
  ( const DistMatrix<F>& A, \
    const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts, \
    DistMatrix<Base<F>,VR,STAR>& invNorms, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> TriangularPseudospectra \
  ( const Matrix<F>& U, const Matrix<Complex<Base<F>>>& shifts, \
    Matrix<Base<F>>& invNorms, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int,VR,STAR> TriangularPseudospectra \
  ( const DistMatrix<F>& U, \
    const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts, \
    DistMatrix<Base<F>,VR,STAR>& invNorms, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> TriangularPseudospectra \
  ( const Matrix<F>& U, const Matrix<F>& Q, \
    const Matrix<Complex<Base<F>>>& shifts, \
    Matrix<Base<F>>& invNorms, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int,VR,STAR> TriangularPseudospectra \
  ( const DistMatrix<F>& U, const DistMatrix<F>& Q, \
    const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts, \
    DistMatrix<Base<F>,VR,STAR>& invNorms, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> HessenbergPseudospectra \
  ( const Matrix<F>& H, const Matrix<Complex<Base<F>>>& shifts, \
    Matrix<Base<F>>& invNorms, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int,VR,STAR> HessenbergPseudospectra \
  ( const DistMatrix<F>& H, \
    const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts, \
    DistMatrix<Base<F>,VR,STAR>& invNorms, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> HessenbergPseudospectra \
  ( const Matrix<F>& H, const Matrix<F>& Q, \
    const Matrix<Complex<Base<F>>>& shifts, Matrix<Base<F>>& invNorms, \
    PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int,VR,STAR> HessenbergPseudospectra \
  ( const DistMatrix<F>& H, const DistMatrix<F>& Q, \
    const DistMatrix<Complex<Base<F>>,VR,STAR>& shifts, \
    DistMatrix<Base<F>,VR,STAR>& invNorms, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> TriangularPseudospectra \
  ( const Matrix<F>& U, Matrix<Base<F>>& invNormMap, \
    Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int> TriangularPseudospectra \
  ( const DistMatrix<F>& U, DistMatrix<Base<F>>& invNormMap, \
    Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> TriangularPseudospectra \
  ( const Matrix<F>& U, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap, \
    Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int> TriangularPseudospectra \
  ( const DistMatrix<F>& U, const DistMatrix<F>& Q, \
    DistMatrix<Base<F>>& invNormMap, \
    Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> HessenbergPseudospectra \
  ( const Matrix<F>& H, Matrix<Base<F>>& invNormMap, \
    Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int> HessenbergPseudospectra \
  ( const DistMatrix<F>& H, DistMatrix<Base<F>>& invNormMap, \
    Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> HessenbergPseudospectra \
  ( const Matrix<F>& H, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap, \
    Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int> HessenbergPseudospectra \
  ( const DistMatrix<F>& H, const DistMatrix<F>& Q, \
    DistMatrix<Base<F>>& invNormMap, \
    Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> Pseudospectra \
  ( const Matrix<F>& A, Matrix<Base<F>>& invNormMap, \
    Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int> Pseudospectra \
  ( const DistMatrix<F>& A, DistMatrix<Base<F>>& invNormMap, \
    Complex<Base<F>> center, Base<F> realWidth, Base<F> imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> Pseudospectra \
  ( const Matrix<F>& A, Matrix<Base<F>>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int> Pseudospectra \
  ( const DistMatrix<F>& A, DistMatrix<Base<F>>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> TriangularPseudospectra \
  ( const Matrix<F>& U, Matrix<Base<F>>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int> TriangularPseudospectra \
  ( const DistMatrix<F>& U, DistMatrix<Base<F>>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> TriangularPseudospectra \
  ( const Matrix<F>& U, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int> TriangularPseudospectra \
  ( const DistMatrix<F>& U, const DistMatrix<F>& Q, \
    DistMatrix<Base<F>>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> HessenbergPseudospectra \
  ( const Matrix<F>& H, Matrix<Base<F>>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int> HessenbergPseudospectra \
  ( const DistMatrix<F>& H, DistMatrix<Base<F>>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template Matrix<Int> HessenbergPseudospectra \
  ( const Matrix<F>& H, const Matrix<F>& Q, Matrix<Base<F>>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl ); \
  template DistMatrix<Int> HessenbergPseudospectra \
  ( const DistMatrix<F>& H, const DistMatrix<F>& Q, \
    DistMatrix<Base<F>>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Base<F>> psCtrl );

#define PROTO_REAL(Real) \
  PROTO(Real) \
  template Matrix<Int> QuasiTriangularPseudospectra \
  ( const Matrix<Real>& U, const Matrix<Complex<Real>>& shifts, \
    Matrix<Real>& invNorms, PseudospecCtrl<Real> psCtrl ); \
  template DistMatrix<Int,VR,STAR> QuasiTriangularPseudospectra \
  ( const DistMatrix<Real>& U, \
    const DistMatrix<Complex<Real>,VR,STAR>& shifts, \
    DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl ); \
  template Matrix<Int> QuasiTriangularPseudospectra \
  ( const Matrix<Real>& U, const Matrix<Real>& Q, \
    const Matrix<Complex<Real>>& shifts, Matrix<Real>& invNorms, \
    PseudospecCtrl<Real> psCtrl ); \
  template DistMatrix<Int,VR,STAR> QuasiTriangularPseudospectra \
  ( const DistMatrix<Real>& U, const DistMatrix<Real>& Q, \
    const DistMatrix<Complex<Real>,VR,STAR>& shifts, \
    DistMatrix<Real,VR,STAR>& invNorms, PseudospecCtrl<Real> psCtrl ); \
  template Matrix<Int> QuasiTriangularPseudospectra \
  ( const Matrix<Real>& U, Matrix<Real>& invNormMap, \
    Complex<Real> center, Real realWidth, Real imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl ); \
  template DistMatrix<Int> QuasiTriangularPseudospectra \
  ( const DistMatrix<Real>& U, DistMatrix<Real>& invNormMap, \
    Complex<Real> center, Real realWidth, Real imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl ); \
  template Matrix<Int> QuasiTriangularPseudospectra \
  ( const Matrix<Real>& U, const Matrix<Real>& Q, Matrix<Real>& invNormMap, \
    Complex<Real> center, Real realWidth, Real imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl ); \
  template DistMatrix<Int> QuasiTriangularPseudospectra \
  ( const DistMatrix<Real>& U, const DistMatrix<Real>& Q, \
    DistMatrix<Real>& invNormMap, \
    Complex<Real> center, Real realWidth, Real imagWidth, \
    Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl ); \
  template Matrix<Int> QuasiTriangularPseudospectra \
  ( const Matrix<Real>& U, Matrix<Real>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl ); \
  template DistMatrix<Int> QuasiTriangularPseudospectra \
  ( const DistMatrix<Real>& U, DistMatrix<Real>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl ); \
  template Matrix<Int> QuasiTriangularPseudospectra \
  ( const Matrix<Real>& U, const Matrix<Real>& Q, Matrix<Real>& invNormMap, \
    Int realSize, Int imagSize, PseudospecCtrl<Real> psCtrl ); \
  template DistMatrix<Int> QuasiTriangularPseudospectra \
  ( const DistMatrix<Real>& U, const DistMatrix<Real>& Q, \
    DistMatrix<Real>& invNormMap, Int realSize, Int imagSize, \
    PseudospecCtrl<Real> psCtrl );

#define EL_NO_INT_PROTO
#include "El/macros/Instantiate.h"

} // namespace El
