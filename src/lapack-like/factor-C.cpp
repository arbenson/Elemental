/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"
#include "El-C.h"
using namespace El;

#define DM_CAST(T,A) dynamic_cast<DistMatrix<T>&>(*CReflect(A))
#define DM_CAST_CONST(T,A) dynamic_cast<const DistMatrix<T>&>(*CReflect(A))

#define DM_MD_STAR_CAST(T,A) \
  dynamic_cast<DistMatrix<T,MD,STAR>&>(*CReflect(A))
#define DM_MD_STAR_CAST_CONST(T,A) \
  dynamic_cast<const DistMatrix<T,MD,STAR>&>(*CReflect(A))

#define DM_STAR_VR_CAST(T,A) \
  dynamic_cast<DistMatrix<T,STAR,VR>&>(*CReflect(A))
#define DM_STAR_VR_CAST_CONST(T,A) \
  dynamic_cast<const DistMatrix<T,STAR,VR>&>(*CReflect(A))

#define DM_STAR_STAR_CAST(T,A) \
  dynamic_cast<DistMatrix<T,STAR,STAR>&>(*CReflect(A))
#define DM_STAR_STAR_CAST_CONST(T,A) \
  dynamic_cast<const DistMatrix<T,STAR,STAR>&>(*CReflect(A))

#define DM_VC_STAR_CAST(T,A) \
  dynamic_cast<DistMatrix<T,VC,STAR>&>(*CReflect(A))
#define DM_VC_STAR_CAST_CONST(T,A) \
  dynamic_cast<const DistMatrix<T,VC,STAR>&>(*CReflect(A))

#define DM_VR_STAR_CAST(T,A) \
  dynamic_cast<DistMatrix<T,VR,STAR>&>(*CReflect(A))
#define DM_VR_STAR_CAST_CONST(T,A) \
  dynamic_cast<const DistMatrix<T,VR,STAR>&>(*CReflect(A))

extern "C" {

ElError ElQRCtrlFillDefault_s( ElQRCtrl_s* ctrl )
{
    ctrl->boundRank = false;
    ctrl->maxRank = 0;
    ctrl->adaptive = false;
    ctrl->tol = 0;
    ctrl->alwaysRecomputeNorms = false;
    return EL_SUCCESS;
}

ElError ElQRCtrlFillDefault_d( ElQRCtrl_d* ctrl )
{
    ctrl->boundRank = false;
    ctrl->maxRank = 0;
    ctrl->adaptive = false;
    ctrl->tol = 0;
    ctrl->alwaysRecomputeNorms = false;
    return EL_SUCCESS;
}

#define C_PROTO_FIELD(SIG,SIGBASE,F) \
  /* Cholesky
     ======== */ \
  /* Cholesky (no pivoting) */ \
  ElError ElCholesky_ ## SIG \
  ( ElUpperOrLower uplo, ElMatrix_ ## SIG A ) \
  { EL_TRY( Cholesky( CReflect(uplo), *CReflect(A) ) ) } \
  ElError ElCholeskyDist_ ## SIG \
  ( ElUpperOrLower uplo, ElDistMatrix_ ## SIG A ) \
  { EL_TRY( Cholesky( CReflect(uplo), DM_CAST(F,A) ) ) } \
  /* Reverse Cholesky (no pivoting) */ \
  ElError ElReverseCholesky_ ## SIG \
  ( ElUpperOrLower uplo, ElMatrix_ ## SIG A ) \
  { EL_TRY( ReverseCholesky( CReflect(uplo), *CReflect(A) ) ) } \
  ElError ElReverseCholeskyDist_ ## SIG \
  ( ElUpperOrLower uplo, ElDistMatrix_ ## SIG A ) \
  { EL_TRY( ReverseCholesky( CReflect(uplo), DM_CAST(F,A) ) ) } \
  /* Cholesky (full pivoting) */ \
  ElError ElCholeskyPiv_ ## SIG \
  ( ElUpperOrLower uplo, ElMatrix_ ## SIG A, ElMatrix_i p ) \
  { EL_TRY( \
      Cholesky( CReflect(uplo), *CReflect(A), *CReflect(p) ) ) } \
  ElError ElCholeskyPivDist_ ## SIG \
  ( ElUpperOrLower uplo, ElDistMatrix_ ## SIG A, ElDistMatrix_i p ) \
  { EL_TRY( \
      Cholesky( CReflect(uplo), DM_CAST(F,A), DM_VC_STAR_CAST(Int,p) ) ) } \
  /* Cholesky low-rank modification */ \
  ElError ElCholeskyMod_ ## SIG \
  ( ElUpperOrLower uplo, ElMatrix_ ## SIG T, \
    Base<F> alpha, ElMatrix_ ## SIG V ) \
  { EL_TRY( \
      CholeskyMod( \
        CReflect(uplo), *CReflect(T), alpha, *CReflect(V) ) ) } \
  ElError ElCholeskyModDist_ ## SIG \
  ( ElUpperOrLower uplo, ElDistMatrix_ ## SIG T, \
    Base<F> alpha, ElDistMatrix_ ## SIG V ) \
  { EL_TRY( \
      CholeskyMod( \
        CReflect(uplo), DM_CAST(F,T), alpha, DM_CAST(F,V) ) ) } \
  /* Hermitian Positive Semi-Definite Cholesky */ \
  ElError ElHPSDCholesky_ ## SIG \
  ( ElUpperOrLower uplo, ElMatrix_ ## SIG A ) \
  { EL_TRY( HPSDCholesky( CReflect(uplo), *CReflect(A) ) ) } \
  ElError ElHPSDCholeskyDist_ ## SIG \
  ( ElUpperOrLower uplo, ElDistMatrix_ ## SIG A ) \
  { EL_TRY( HPSDCholesky( CReflect(uplo), DM_CAST(F,A) ) ) } \
  /* Solve after a Cholesky factorization (without pivoting) */ \
  ElError ElSolveAfterCholesky_ ## SIG \
  ( ElUpperOrLower uplo, ElOrientation orientation, \
    ElConstMatrix_ ## SIG A, ElMatrix_ ## SIG B ) \
  { EL_TRY( \
      cholesky::SolveAfter( \
        CReflect(uplo), CReflect(orientation), \
        *CReflect(A), *CReflect(B) ) ) } \
  ElError ElSolveAfterCholeskyDist_ ## SIG \
  ( ElUpperOrLower uplo, ElOrientation orientation, \
    ElConstDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( \
      cholesky::SolveAfter( \
        CReflect(uplo), CReflect(orientation), \
        DM_CAST_CONST(F,A), DM_CAST(F,B) ) ) } \
  /* Solve after a Cholesky factorization (full pivoting) */ \
  ElError ElSolveAfterCholeskyFullPiv_ ## SIG \
  ( ElUpperOrLower uplo, ElOrientation orientation, \
    ElConstMatrix_ ## SIG A, ElConstMatrix_i p, ElMatrix_ ## SIG B ) \
  { EL_TRY( \
      cholesky::SolveAfter( \
        CReflect(uplo), CReflect(orientation), \
        *CReflect(A), *CReflect(p), *CReflect(B) ) ) } \
  ElError ElSolveAfterCholeskyFullPivDist_ ## SIG \
  ( ElUpperOrLower uplo, ElOrientation orientation, \
    ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_i p, \
    ElDistMatrix_ ## SIG B ) \
  { EL_TRY( \
      cholesky::SolveAfter( \
        CReflect(uplo), CReflect(orientation), \
        DM_CAST_CONST(F,A), DM_VC_STAR_CAST_CONST(Int,p), DM_CAST(F,B) ) ) } \
  /* Generalized QR
     ============== */ \
  ElError ElGQR_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG tA, ElMatrix_ ## SIGBASE dA, \
    ElMatrix_ ## SIG B, ElMatrix_ ## SIG tB, ElMatrix_ ## SIGBASE dB ) \
  { EL_TRY( GQR( *CReflect(A), *CReflect(tA), *CReflect(dA), \
                 *CReflect(B), *CReflect(tB), *CReflect(dB) ) ) } \
  ElError ElGQRDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    ElDistMatrix_ ## SIG tA, ElDistMatrix_ ## SIGBASE dA, \
    ElDistMatrix_ ## SIG B, \
    ElDistMatrix_ ## SIG tB, ElDistMatrix_ ## SIGBASE dB ) \
  { EL_TRY( GQR( \
      DM_CAST(F,A), DM_MD_STAR_CAST(F,tA), DM_MD_STAR_CAST(Base<F>,dA), \
      DM_CAST(F,B), DM_MD_STAR_CAST(F,tB), DM_MD_STAR_CAST(Base<F>,dB) ) ) } \
  ElError ElGQRTriang_ ## SIG ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG B ) \
  { EL_TRY( GQR( *CReflect(A), *CReflect(B) ) ) } \
  ElError ElGQRTriangDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( GQR( DM_CAST(F,A), DM_CAST(F,B) ) ) } \
  /* Generalized RQ
     ============== */ \
  ElError ElGRQ_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG tA, ElMatrix_ ## SIGBASE dA, \
    ElMatrix_ ## SIG B, ElMatrix_ ## SIG tB, ElMatrix_ ## SIGBASE dB ) \
  { EL_TRY( GRQ( *CReflect(A), *CReflect(tA), *CReflect(dA), \
                 *CReflect(B), *CReflect(tB), *CReflect(dB) ) ) } \
  ElError ElGRQDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    ElDistMatrix_ ## SIG tA, ElDistMatrix_ ## SIGBASE dA, \
    ElDistMatrix_ ## SIG B, \
    ElDistMatrix_ ## SIG tB, ElDistMatrix_ ## SIGBASE dB ) \
  { EL_TRY( GRQ( \
      DM_CAST(F,A), DM_MD_STAR_CAST(F,tA), DM_MD_STAR_CAST(Base<F>,dA), \
      DM_CAST(F,B), DM_MD_STAR_CAST(F,tB), DM_MD_STAR_CAST(Base<F>,dB) ) ) } \
  ElError ElGRQTriang_ ## SIG ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG B ) \
  { EL_TRY( GRQ( *CReflect(A), *CReflect(B) ) ) } \
  ElError ElGRQTriangDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( GRQ( DM_CAST(F,A), DM_CAST(F,B) ) ) } \
  /* Interpolative Decomposition 
     =========================== */ \
  ElError ElID_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_i p, ElMatrix_ ## SIG Z, \
    ElQRCtrl_ ## SIGBASE ctrl, bool canOverwrite ) \
  { EL_TRY( \
      ID( *CReflect(A), *CReflect(p), *CReflect(Z), \
          CReflect(ctrl), canOverwrite ) ) } \
  ElError ElIDDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_i p, ElDistMatrix_ ## SIG Z, \
    ElQRCtrl_ ## SIGBASE ctrl, bool canOverwrite ) \
  { EL_TRY( \
      ID( DM_CAST(F,A), DM_VR_STAR_CAST(Int,p), DM_STAR_VR_CAST(F,Z), \
          CReflect(ctrl), canOverwrite ) ) } \
  /* LDL factorization
     ================= */ \
  /* Return the inertia given diagonal and subdiagonal from an LDL^H fact */ \
  ElError ElInertiaAfterLDL_ ## SIG \
  ( ElConstMatrix_ ## SIGBASE d, ElConstMatrix_ ## SIG dSub, \
    ElInertiaType* inertia ) \
  { EL_TRY( *inertia = \
      CReflect(ldl::Inertia(*CReflect(d),*CReflect(dSub))) ) } \
  ElError ElInertiaAfterLDLDist_ ## SIG \
  ( ElConstDistMatrix_ ## SIGBASE d, ElConstDistMatrix_ ## SIG dSub, \
    ElInertiaType* inertia ) \
  { EL_TRY( *inertia = CReflect(ldl::Inertia( \
      DM_MD_STAR_CAST_CONST(Base<F>,d),DM_MD_STAR_CAST_CONST(F,dSub))) ) } \
  /* LQ factorization 
     ================ */ \
  /* Return the packed LQ factorization */ \
  ElError ElLQ_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG t, ElMatrix_ ## SIGBASE d ) \
  { EL_TRY( LQ( *CReflect(A), *CReflect(t), *CReflect(d) ) ) } \
  ElError ElLQDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG t, \
    ElDistMatrix_ ## SIGBASE d ) \
  { EL_TRY( \
      LQ( DM_CAST(F,A), DM_MD_STAR_CAST(F,t), DM_MD_STAR_CAST(Base<F>,d) ) ) } \
  /* Explicitly return both factors */ \
  ElError ElExplicitLQ_ ## SIG \
  ( ElMatrix_ ## SIG L, ElMatrix_ ## SIG A ) \
  { EL_TRY( lq::Explicit( *CReflect(L), *CReflect(A) ) ) } \
  ElError ElExplicitLQDist_ ## SIG \
  ( ElDistMatrix_ ## SIG L, ElDistMatrix_ ## SIG A ) \
  { EL_TRY( lq::Explicit( DM_CAST(F,L), DM_CAST(F,A) ) ) } \
  /* Only return the triangular factor */ \
  ElError ElLQTriang_ ## SIG ( ElMatrix_ ## SIG A ) \
  { EL_TRY( LQ( *CReflect(A) ) ) } \
  ElError ElLQTriangDist_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( LQ( DM_CAST(F,A) ) ) } \
  /* Only return the unitary factor */ \
  ElError ElLQUnitary_ ## SIG ( ElMatrix_ ## SIG A ) \
  { EL_TRY( lq::Explicit( *CReflect(A) ) ) } \
  ElError ElLQUnitaryDist_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( lq::Explicit( DM_CAST(F,A) ) ) } \
  /* Apply Q after an LQ factorization */ \
  ElError ElApplyQAfterLQ_ ## SIG \
  ( ElLeftOrRight side, ElOrientation orientation, \
    ElConstMatrix_ ## SIG A, ElConstMatrix_ ## SIG t, \
    ElConstMatrix_ ## SIGBASE d, ElMatrix_ ## SIG B ) \
  { EL_TRY( lq::ApplyQ( \
      CReflect(side), CReflect(orientation), \
      *CReflect(A), *CReflect(t), \
      *CReflect(d), *CReflect(B) ) ) } \
  ElError ElApplyQAfterLQDist_ ## SIG \
  ( ElLeftOrRight side, ElOrientation orientation, \
    ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_ ## SIG t, \
    ElConstDistMatrix_ ## SIGBASE d, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( lq::ApplyQ( \
      CReflect(side), CReflect(orientation), \
      DM_CAST_CONST(F,A), DM_MD_STAR_CAST_CONST(F,t), \
      DM_MD_STAR_CAST_CONST(Base<F>,d), DM_CAST(F,B) ) ) } \
  /* Solve against vectors after LQ factorization */ \
  ElError ElSolveAfterLQ_ ## SIG \
  ( ElOrientation orientation, \
    ElConstMatrix_ ## SIG A, ElConstMatrix_ ## SIG t, \
    ElConstMatrix_ ## SIGBASE d, ElConstMatrix_ ## SIG B, ElMatrix_ ## SIG X ) \
  { EL_TRY( lq::SolveAfter( \
      CReflect(orientation), *CReflect(A), *CReflect(t), \
      *CReflect(d), *CReflect(B), *CReflect(X) ) ) } \
  ElError ElSolveAfterLQDist_ ## SIG \
  ( ElOrientation orientation, \
    ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_ ## SIG t, \
    ElConstDistMatrix_ ## SIGBASE d, ElConstDistMatrix_ ## SIG B, \
    ElDistMatrix_ ## SIG X ) \
  { EL_TRY( lq::SolveAfter( \
      CReflect(orientation), \
      DM_CAST_CONST(F,A), DM_MD_STAR_CAST_CONST(F,t), \
      DM_MD_STAR_CAST_CONST(Base<F>,d), DM_CAST_CONST(F,B), DM_CAST(F,X) ) ) } \
  /* LU factorization 
     ================ */ \
  /* LU without pivoting */ \
  ElError ElLU_ ## SIG ( ElMatrix_ ## SIG A ) \
  { EL_TRY( LU( *CReflect(A) ) ) } \
  ElError ElLUDist_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( LU( DM_CAST(F,A) ) ) } \
  /* LU with partial pivoting */ \
  ElError ElLUPartialPiv_ ## SIG ( ElMatrix_ ## SIG A, ElMatrix_i p ) \
  { EL_TRY( LU( *CReflect(A), *CReflect(p) ) ) } \
  ElError ElLUPartialPivDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_i p ) \
  { EL_TRY( LU( DM_CAST(F,A), DM_VC_STAR_CAST(Int,p) ) ) } \
  /* LU with full pivoting */ \
  ElError ElLUFullPiv_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_i p, ElMatrix_i q ) \
  { EL_TRY( LU( *CReflect(A), *CReflect(p), *CReflect(q) ) ) } \
  ElError ElLUFullPivDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_i p, ElDistMatrix_i q ) \
  { EL_TRY( LU( \
      DM_CAST(F,A), DM_VC_STAR_CAST(Int,p), DM_VC_STAR_CAST(Int,q) ) ) } \
  /* Solve against vectors after LU with no pivoting */ \
  ElError ElSolveAfterLU_ ## SIG \
  ( ElOrientation orientation, ElConstMatrix_ ## SIG A, ElMatrix_ ## SIG B ) \
  { EL_TRY( lu::SolveAfter( \
      CReflect(orientation), *CReflect(A), *CReflect(B) ) ) } \
  ElError ElSolveAfterLUDist_ ## SIG \
  ( ElOrientation orientation, \
    ElConstDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( lu::SolveAfter( \
      CReflect(orientation), DM_CAST_CONST(F,A), DM_CAST(F,B) ) ) } \
  /* Solve against vectors after LU with partial pivoting */ \
  ElError ElSolveAfterLUPartialPiv_ ## SIG \
  ( ElOrientation orientation, ElConstMatrix_ ## SIG A, ElConstMatrix_i p, \
    ElMatrix_ ## SIG B ) \
  { EL_TRY( lu::SolveAfter( \
      CReflect(orientation), \
      *CReflect(A), *CReflect(p), *CReflect(B) ) ) } \
  ElError ElSolveAfterLUPartialPivDist_ ## SIG \
  ( ElOrientation orientation, \
    ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_i p, \
    ElDistMatrix_ ## SIG B ) \
  { EL_TRY( lu::SolveAfter( \
      CReflect(orientation), \
      DM_CAST_CONST(F,A), DM_VC_STAR_CAST_CONST(Int,p), DM_CAST(F,B) ) ) } \
  /* Solve against vectors after LU with full pivoting */ \
  ElError ElSolveAfterLUFullPiv_ ## SIG \
  ( ElOrientation orientation, ElConstMatrix_ ## SIG A, \
    ElConstMatrix_i p, ElConstMatrix_i q, ElMatrix_ ## SIG B ) \
  { EL_TRY( lu::SolveAfter( \
      CReflect(orientation), *CReflect(A), \
      *CReflect(p), *CReflect(q), *CReflect(B) ) ) } \
  ElError ElSolveAfterLUFullPivDist_ ## SIG \
  ( ElOrientation orientation, \
    ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_i p, ElConstDistMatrix_i q, \
    ElDistMatrix_ ## SIG B ) \
  { EL_TRY( lu::SolveAfter( \
      CReflect(orientation), DM_CAST_CONST(F,A), \
      DM_VC_STAR_CAST_CONST(Int,p), DM_VC_STAR_CAST_CONST(Int,q), \
      DM_CAST(F,B) ) ) } \
  /* QR factorization 
     ================ */ \
  /* Return the packed QR factorization (with no pivoting) */ \
  ElError ElQR_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG t, ElMatrix_ ## SIGBASE d ) \
  { EL_TRY( QR( *CReflect(A), *CReflect(t), *CReflect(d) ) ) } \
  ElError ElQRDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    ElDistMatrix_ ## SIG t, ElDistMatrix_ ## SIGBASE d ) \
  { EL_TRY( QR( \
      DM_CAST(F,A), DM_MD_STAR_CAST(F,t), DM_MD_STAR_CAST(Base<F>,d) ) ) } \
  /* Return the packed QR factorization (with column pivoting) */ \
  ElError ElQRColPiv_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG t, ElMatrix_ ## SIGBASE d, \
    ElMatrix_i p ) \
  { EL_TRY( QR( \
      *CReflect(A), *CReflect(t), *CReflect(d), *CReflect(p) ) ) } \
  ElError ElQRColPivDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    ElDistMatrix_ ## SIG t, ElDistMatrix_ ## SIGBASE d, ElDistMatrix_i p ) \
  { EL_TRY( QR( \
      DM_CAST(F,A), DM_MD_STAR_CAST(F,t), DM_MD_STAR_CAST(Base<F>,d), \
      DM_VR_STAR_CAST(Int,p) ) ) } \
  /* Return the packed QR factorization (with column pivoting, eXpert) */ \
  ElError ElQRColPivX_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG t, ElMatrix_ ## SIGBASE d, \
    ElMatrix_i p, ElQRCtrl_ ## SIGBASE ctrl ) \
  { EL_TRY( QR( \
      *CReflect(A), *CReflect(t), *CReflect(d), *CReflect(p),\
      CReflect(ctrl) ) ) } \
  ElError ElQRColPivXDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    ElDistMatrix_ ## SIG t, ElDistMatrix_ ## SIGBASE d, ElDistMatrix_i p, \
    ElQRCtrl_ ## SIGBASE ctrl ) \
  { EL_TRY( QR( \
      DM_CAST(F,A), DM_MD_STAR_CAST(F,t), DM_MD_STAR_CAST(Base<F>,d), \
      DM_VR_STAR_CAST(Int,p), CReflect(ctrl) ) ) } \
  /* Explicitly return Q and R (with no pivoting) */ \
  ElError ElExplicitQR_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG R ) \
  { EL_TRY( qr::Explicit( *CReflect(A), *CReflect(R) ) ) } \
  ElError ElExplicitQRDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG R ) \
  { EL_TRY( qr::Explicit( DM_CAST(F,A), DM_CAST(F,R) ) ) } \
  /* Explicitly return Q, R, and P (with column pivoting) */ \
  ElError ElExplicitQRColPiv_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG R, ElMatrix_i p ) \
  { EL_TRY( \
      qr::Explicit( *CReflect(A), *CReflect(R), *CReflect(p) ) ) } \
  ElError ElExplicitQRColPivDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG R, ElDistMatrix_i p ) \
  { EL_TRY( qr::Explicit( \
      DM_CAST(F,A), DM_CAST(F,R), DM_VR_STAR_CAST(Int,p) ) ) } \
  /* Return the triangular factor from QR (with no pivoting) */ \
  ElError ElQRTriang_ ## SIG ( ElMatrix_ ## SIG A ) \
  { EL_TRY( QR( *CReflect(A) ) ) } \
  ElError ElQRTriangDist_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( QR( DM_CAST(F,A) ) ) } \
  /* Return the triangular factor and P from QR (with column pivoting) */ \
  ElError ElQRColPivTriang_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_i p ) \
  { EL_TRY( QR( *CReflect(A), *CReflect(p) ) ) } \
  ElError ElQRColPivTriangDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_i p ) \
  { EL_TRY( QR( DM_CAST(F,A), DM_VR_STAR_CAST(Int,p) ) ) } \
  /* Return the unitary factor from QR with no pivoting */ \
  ElError ElQRUnitary_ ## SIG ( ElMatrix_ ## SIG A ) \
  { EL_TRY( qr::Explicit( *CReflect(A) ) ) } \
  ElError ElQRUnitaryDist_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( qr::Explicit( DM_CAST(F,A) ) ) } \
  /* Return the unitary factor from QR with column pivoting */ \
  ElError ElQRColPivUnitary_ ## SIG ( ElMatrix_ ## SIG A ) \
  { EL_TRY( qr::Explicit( *CReflect(A), true ) ) } \
  ElError ElQRColPivUnitaryDist_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( qr::Explicit( DM_CAST(F,A), true ) ) } \
  /* Cholesky-based QR factorization */ \
  ElError ElCholeskyQR_ ## SIG ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG R ) \
  { EL_TRY( qr::Cholesky( *CReflect(A), *CReflect(R) ) ) } \
  ElError ElCholeskyQRDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG R ) \
  { EL_TRY( qr::Cholesky( DM_VC_STAR_CAST(F,A), DM_STAR_STAR_CAST(F,R) ) ) } \
  /* Apply Q after a QR factorization */ \
  ElError ElApplyQAfterQR_ ## SIG \
  ( ElLeftOrRight side, ElOrientation orientation, \
    ElConstMatrix_ ## SIG A, ElConstMatrix_ ## SIG t, \
    ElConstMatrix_ ## SIGBASE d, ElMatrix_ ## SIG B ) \
  { EL_TRY( qr::ApplyQ( \
      CReflect(side), CReflect(orientation), \
      *CReflect(A), *CReflect(t), \
      *CReflect(d), *CReflect(B) ) ) } \
  ElError ElApplyQAfterQRDist_ ## SIG \
  ( ElLeftOrRight side, ElOrientation orientation, \
    ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_ ## SIG t, \
    ElConstDistMatrix_ ## SIGBASE d, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( qr::ApplyQ( \
      CReflect(side), CReflect(orientation), \
      DM_CAST_CONST(F,A), DM_MD_STAR_CAST_CONST(F,t), \
      DM_MD_STAR_CAST_CONST(Base<F>,d), DM_CAST(F,B) ) ) } \
  /* Solve against vectors after QR factorization */ \
  ElError ElSolveAfterQR_ ## SIG \
  ( ElOrientation orientation, ElConstMatrix_ ## SIG A, \
    ElConstMatrix_ ## SIG t, ElConstMatrix_ ## SIGBASE d, \
    ElConstMatrix_ ## SIG B, ElMatrix_ ## SIG X ) \
  { EL_TRY( qr::SolveAfter( \
      CReflect(orientation), *CReflect(A), \
      *CReflect(t), *CReflect(d), *CReflect(B), *CReflect(X) ) ) } \
  ElError ElSolveAfterQRDist_ ## SIG \
  ( ElOrientation orientation, ElConstDistMatrix_ ## SIG A, \
    ElConstDistMatrix_ ## SIG t, ElConstDistMatrix_ ## SIGBASE d, \
    ElConstDistMatrix_ ## SIG B, ElDistMatrix_ ## SIG X ) \
  { EL_TRY( qr::SolveAfter( \
      CReflect(orientation), DM_CAST_CONST(F,A), \
      DM_MD_STAR_CAST_CONST(F,t), DM_MD_STAR_CAST_CONST(Base<F>,d), \
      DM_CAST_CONST(F,B), DM_CAST(F,X) ) ) } \
  /* RQ factorization 
     ================ */ \
  /* Return the packed RQ factorization */ \
  ElError ElRQ_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG t, ElMatrix_ ## SIGBASE d ) \
  { EL_TRY( RQ( *CReflect(A), *CReflect(t), *CReflect(d) ) ) } \
  ElError ElRQDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG t, \
    ElDistMatrix_ ## SIGBASE d ) \
  { EL_TRY( \
      RQ( DM_CAST(F,A), DM_MD_STAR_CAST(F,t), DM_MD_STAR_CAST(Base<F>,d) ) ) } \
  /* TODO: Explicitly return both factors */ \
  /* Only return the triangular factor */ \
  ElError ElRQTriang_ ## SIG ( ElMatrix_ ## SIG A ) \
  { EL_TRY( RQ( *CReflect(A) ) ) } \
  ElError ElRQTriangDist_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( RQ( DM_CAST(F,A) ) ) } \
  /* TODO: Only return the unitary factor */ \
  /* Apply Q after an RQ factorization */ \
  ElError ElApplyQAfterRQ_ ## SIG \
  ( ElLeftOrRight side, ElOrientation orientation, \
    ElConstMatrix_ ## SIG A, ElConstMatrix_ ## SIG t, \
    ElConstMatrix_ ## SIGBASE d, ElMatrix_ ## SIG B ) \
  { EL_TRY( rq::ApplyQ( \
      CReflect(side), CReflect(orientation), \
      *CReflect(A), *CReflect(t), \
      *CReflect(d), *CReflect(B) ) ) } \
  ElError ElApplyQAfterRQDist_ ## SIG \
  ( ElLeftOrRight side, ElOrientation orientation, \
    ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_ ## SIG t, \
    ElConstDistMatrix_ ## SIGBASE d, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( rq::ApplyQ( \
      CReflect(side), CReflect(orientation), \
      DM_CAST_CONST(F,A), DM_MD_STAR_CAST_CONST(F,t), \
      DM_MD_STAR_CAST_CONST(Base<F>,d), DM_CAST(F,B) ) ) } \
  /* Solve against vectors after RQ factorization */ \
  ElError ElSolveAfterRQ_ ## SIG \
  ( ElOrientation orientation, \
    ElConstMatrix_ ## SIG A, ElConstMatrix_ ## SIG t, \
    ElConstMatrix_ ## SIGBASE d, ElConstMatrix_ ## SIG B, ElMatrix_ ## SIG X ) \
  { EL_TRY( rq::SolveAfter( \
      CReflect(orientation), *CReflect(A), *CReflect(t), \
      *CReflect(d), *CReflect(B), *CReflect(X) ) ) } \
  ElError ElSolveAfterRQDist_ ## SIG \
  ( ElOrientation orientation, \
    ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_ ## SIG t, \
    ElConstDistMatrix_ ## SIGBASE d, ElConstDistMatrix_ ## SIG B, \
    ElDistMatrix_ ## SIG X ) \
  { EL_TRY( rq::SolveAfter( \
      CReflect(orientation), \
      DM_CAST_CONST(F,A), DM_MD_STAR_CAST_CONST(F,t), \
      DM_MD_STAR_CAST_CONST(Base<F>,d), DM_CAST_CONST(F,B), DM_CAST(F,X) ) ) } \
  /* Skeleton factorization
     ====================== */ \
  ElError ElSkeleton_ ## SIG \
  ( ElConstMatrix_ ## SIG A, ElMatrix_i pR, ElMatrix_i pC, \
    ElMatrix_ ## SIG Z, ElQRCtrl_ ## SIGBASE ctrl ) \
  { EL_TRY( Skeleton( *CReflect(A), *CReflect(pR), *CReflect(pC), \
                      *CReflect(Z), CReflect(ctrl) ) ) } \
  ElError ElSkeletonDist_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElDistMatrix_i pR, ElDistMatrix_i pC, \
    ElDistMatrix_ ## SIG Z, ElQRCtrl_ ## SIGBASE ctrl ) \
  { EL_TRY( Skeleton( \
      DM_CAST_CONST(F,A), DM_VR_STAR_CAST(Int,pR), DM_VR_STAR_CAST(Int,pC), \
      DM_CAST(F,Z), CReflect(ctrl) ) ) }

#define C_PROTO_REAL(SIG,F) \
  C_PROTO_FIELD(SIG,SIG,F) \
  /* LDL factorization 
     ================= */ \
  /* Return the packed LDL factorization (without pivoting) */ \
  ElError ElLDL_ ## SIG ( ElMatrix_ ## SIG A ) \
  { EL_TRY( LDL( *CReflect(A), false ) ) } \
  ElError ElLDLDist_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( LDL( DM_CAST(F,A), false ) ) } \
  /* Return the packed LDL factorization with pivoting */ \
  ElError ElLDLPiv_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG dSub, ElMatrix_i p, \
    ElLDLPivotType pivotType ) \
  { EL_TRY( LDL( *CReflect(A), *CReflect(dSub), *CReflect(p), false, \
                 CReflect(pivotType) ) ) } \
  ElError ElLDLPivDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG dSub, ElDistMatrix_i p, \
    ElLDLPivotType pivotType ) \
  { EL_TRY( LDL( DM_CAST(F,A), DM_MD_STAR_CAST(F,dSub), \
                 DM_VC_STAR_CAST(Int,p), false, CReflect(pivotType) ) ) } \
  /* Multiply vectors after an unpivoted LDL factorization */ \
  ElError ElMultiplyAfterLDL_ ## SIG \
  ( ElConstMatrix_ ## SIG A, ElMatrix_ ## SIG B ) \
  { EL_TRY( ldl::MultiplyAfter( *CReflect(A), *CReflect(B), false ) ) } \
  ElError ElMultiplyAfterLDLDist_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( ldl::MultiplyAfter( DM_CAST_CONST(F,A), DM_CAST(F,B), false ) ) } \
  /* Multiply vectors after a pivoted LDL factorization */ \
  ElError ElMultiplyAfterLDLPiv_ ## SIG \
  ( ElConstMatrix_ ## SIG A, ElConstMatrix_ ## SIG dSub, ElConstMatrix_i p, \
    ElMatrix_ ## SIG B ) \
  { EL_TRY( ldl::MultiplyAfter( \
      *CReflect(A), *CReflect(dSub), *CReflect(p), *CReflect(B), \
      false ) ) } \
  ElError ElMultiplyAfterLDLPivDist_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_ ## SIG dSub, \
    ElConstDistMatrix_i p, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( ldl::MultiplyAfter( \
      DM_CAST_CONST(F,A), DM_MD_STAR_CAST_CONST(F,dSub), \
      DM_VC_STAR_CAST_CONST(Int,p), DM_CAST(F,B), false ) ) } \
  /* Solve against vectors after an unpivoted LDL factorization */ \
  ElError ElSolveAfterLDL_ ## SIG \
  ( ElConstMatrix_ ## SIG A, ElMatrix_ ## SIG B ) \
  { EL_TRY( ldl::SolveAfter( *CReflect(A), *CReflect(B), false ) ) } \
  ElError ElSolveAfterLDLDist_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( ldl::SolveAfter( DM_CAST_CONST(F,A), DM_CAST(F,B), false ) ) } \
  /* Solve against vectors after a pivoted LDL factorization */ \
  ElError ElSolveAfterLDLPiv_ ## SIG \
  ( ElConstMatrix_ ## SIG A, ElConstMatrix_ ## SIG dSub, ElConstMatrix_i p, \
    ElMatrix_ ## SIG B ) \
  { EL_TRY( ldl::SolveAfter( \
      *CReflect(A), *CReflect(dSub), *CReflect(p), *CReflect(B), \
      false ) ) } \
  ElError ElSolveAfterLDLPivDist_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_ ## SIG dSub, \
    ElConstDistMatrix_i p, ElDistMatrix_ ## SIG B ) \
  { EL_TRY( ldl::SolveAfter( \
      DM_CAST_CONST(F,A), DM_MD_STAR_CAST_CONST(F,dSub), \
      DM_VC_STAR_CAST_CONST(Int,p), DM_CAST(F,B), false ) ) } \
  /* LU factorization
     ================ */ \
  /* Rank-one LU factorization modification */ \
  ElError ElLUMod_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_i p, \
    ElConstMatrix_ ## SIG u, ElConstMatrix_ ## SIG v, \
    Base<F> tau ) \
  { EL_TRY( LUMod( \
      *CReflect(A), *CReflect(p), \
      *CReflect(u), *CReflect(v), false, tau ) ) } \
  ElError ElLUModDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_i p, \
    ElConstDistMatrix_ ## SIG u, ElConstDistMatrix_ ## SIG v, \
    Base<F> tau ) \
  { EL_TRY( LUMod( \
      DM_CAST(F,A), DM_VC_STAR_CAST(Int,p), \
      DM_CAST_CONST(F,u), DM_CAST_CONST(F,v), false, tau ) ) }

#define C_PROTO_COMPLEX(SIG,SIGBASE,F) \
  C_PROTO_FIELD(SIG,SIGBASE,F) \
  /* LDL factorization 
     ================= */ \
  /* Return the packed LDL factorization (without pivoting) */ \
  ElError ElLDL_ ## SIG ( ElMatrix_ ## SIG A, bool conjugate ) \
  { EL_TRY( LDL( *CReflect(A), conjugate ) ) } \
  ElError ElLDLDist_ ## SIG ( ElDistMatrix_ ## SIG A, bool conjugate ) \
  { EL_TRY( LDL( DM_CAST(F,A), conjugate ) ) } \
  /* Return the packed LDL factorization with pivoting */ \
  ElError ElLDLPiv_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_ ## SIG dSub, ElMatrix_i p, bool conjugate, \
    ElLDLPivotType pivotType ) \
  { EL_TRY( LDL( *CReflect(A), *CReflect(dSub), *CReflect(p), \
                 conjugate, CReflect(pivotType) ) ) } \
  ElError ElLDLPivDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG dSub, ElDistMatrix_i p, \
    bool conjugate, ElLDLPivotType pivotType ) \
  { EL_TRY( LDL( DM_CAST(F,A), DM_MD_STAR_CAST(F,dSub), \
                 DM_VC_STAR_CAST(Int,p), conjugate, \
                 CReflect(pivotType) ) ) } \
  /* Multiply vectors after an unpivoted LDL factorization */ \
  ElError ElMultiplyAfterLDL_ ## SIG \
  ( ElConstMatrix_ ## SIG A, ElMatrix_ ## SIG B, bool conjugate ) \
  { EL_TRY( \
      ldl::MultiplyAfter( *CReflect(A), *CReflect(B), conjugate ) ) } \
  ElError ElMultiplyAfterLDLDist_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG B, bool conjugate ) \
  { EL_TRY( \
      ldl::MultiplyAfter( DM_CAST_CONST(F,A), DM_CAST(F,B), conjugate ) ) } \
  /* Multiply vectors after a pivoted LDL factorization */ \
  ElError ElMultiplyAfterLDLPiv_ ## SIG \
  ( ElConstMatrix_ ## SIG A, ElConstMatrix_ ## SIG dSub, ElConstMatrix_i p, \
    ElMatrix_ ## SIG B, bool conjugate ) \
  { EL_TRY( ldl::MultiplyAfter( \
      *CReflect(A), *CReflect(dSub), *CReflect(p), *CReflect(B), \
      conjugate ) ) } \
  ElError ElMultiplyAfterLDLPivDist_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_ ## SIG dSub, \
    ElConstDistMatrix_i p, ElDistMatrix_ ## SIG B, bool conjugate ) \
  { EL_TRY( ldl::MultiplyAfter( \
      DM_CAST_CONST(F,A), DM_MD_STAR_CAST_CONST(F,dSub), \
      DM_VC_STAR_CAST_CONST(Int,p), DM_CAST(F,B), conjugate ) ) } \
  /* Solve against vectors after an unpivoted LDL factorization */ \
  ElError ElSolveAfterLDL_ ## SIG \
  ( ElConstMatrix_ ## SIG A, ElMatrix_ ## SIG B, bool conjugate ) \
  { EL_TRY( ldl::SolveAfter( *CReflect(A), *CReflect(B), conjugate ) ) } \
  ElError ElSolveAfterLDLDist_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElDistMatrix_ ## SIG B, bool conjugate ) \
  { EL_TRY( ldl::SolveAfter( DM_CAST_CONST(F,A), DM_CAST(F,B), conjugate ) ) } \
  /* Solve against vectors after a pivoted LDL factorization */ \
  ElError ElSolveAfterLDLPiv_ ## SIG \
  ( ElConstMatrix_ ## SIG A, ElConstMatrix_ ## SIG dSub, ElConstMatrix_i p, \
    ElMatrix_ ## SIG B, bool conjugate ) \
  { EL_TRY( ldl::SolveAfter( \
      *CReflect(A), *CReflect(dSub), *CReflect(p), *CReflect(B), \
      conjugate ) ) } \
  ElError ElSolveAfterLDLPivDist_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElConstDistMatrix_ ## SIG dSub, \
    ElConstDistMatrix_i p, ElDistMatrix_ ## SIG B, bool conjugate ) \
  { EL_TRY( ldl::SolveAfter( \
      DM_CAST_CONST(F,A), \
      DM_MD_STAR_CAST_CONST(F,dSub), DM_VC_STAR_CAST_CONST(Int,p), \
      DM_CAST(F,B), conjugate ) ) } \
  /* LU factorization
     ================ */ \
  /* Rank-one LU factorization modification */ \
  ElError ElLUMod_ ## SIG \
  ( ElMatrix_ ## SIG A, ElMatrix_i p, \
    ElConstMatrix_ ## SIG u, ElConstMatrix_ ## SIG v, \
    bool conjugate, Base<F> tau ) \
  { EL_TRY( LUMod( \
      *CReflect(A), *CReflect(p), \
      *CReflect(u), *CReflect(v), conjugate, tau ) ) } \
  ElError ElLUModDist_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistMatrix_i p, \
    ElConstDistMatrix_ ## SIG u, ElConstDistMatrix_ ## SIG v, \
    bool conjugate, Base<F> tau ) \
  { EL_TRY( LUMod( \
      DM_CAST(F,A), DM_VC_STAR_CAST(Int,p), \
      DM_CAST_CONST(F,u), DM_CAST_CONST(F,v), conjugate, tau ) ) }

#define EL_NO_INT_PROTO
#include "El/macros/CInstantiate.h"

} // extern "C"
