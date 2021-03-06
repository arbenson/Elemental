/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_LAPACK_SPECTRAL_C_H
#define EL_LAPACK_SPECTRAL_C_H

#include "El/lapack-like/funcs-C.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Pencil */
typedef enum {
  EL_AXBX=1,
  EL_ABX=2, 
  EL_BAX=3
} ElPencil;

/* HermitianSdcCtrl */
typedef struct {
  ElInt cutoff;
  ElInt maxInnerIts, maxOuterIts;
  float tol;
  float spreadFactor;
  bool progress;
} ElHermitianSdcCtrl_s;
ElError ElHermitianSdcCtrlDefault_s( ElHermitianSdcCtrl_s* ctrl );

typedef struct {
  ElInt cutoff;
  ElInt maxInnerIts, maxOuterIts;
  double tol;
  double spreadFactor;
  bool progress;
} ElHermitianSdcCtrl_d;
ElError ElHermitianSdcCtrlDefault_d( ElHermitianSdcCtrl_d* ctrl );

/* HermitianEigSubset */
typedef struct {
  bool indexSubset;
  ElInt lowerIndex, upperIndex;

  bool rangeSubset;
  float lowerBound, upperBound;
} ElHermitianEigSubset_s;
ElError ElHermitianEigSubsetDefault_s( ElHermitianEigSubset_s* subset );

typedef struct {
  bool indexSubset;
  ElInt lowerIndex, upperIndex;

  bool rangeSubset;
  double lowerBound, upperBound;
} ElHermitianEigSubset_d;
ElError ElHermitianEigSubsetDefault_d( ElHermitianEigSubset_d* subset );

/* HermitianEigCtrl */
typedef struct {
  ElHermitianTridiagCtrl tridiagCtrl;
  ElHermitianSdcCtrl_s sdcCtrl;
  bool useSdc;
} ElHermitianEigCtrl_s;
ElError ElHermitianEigCtrlDefault_s( ElHermitianEigCtrl_s* ctrl );

typedef struct {
  ElHermitianTridiagCtrl tridiagCtrl;
  ElHermitianSdcCtrl_d sdcCtrl;
  bool useSdc;
} ElHermitianEigCtrl_d;
ElError ElHermitianEigCtrlDefault_d( ElHermitianEigCtrl_d* ctrl );

/* PolarCtrl */
typedef struct {
  bool qdwh;
  bool colPiv;
  ElInt maxIts;
  ElInt numIts;
} ElPolarCtrl;
ElError ElPolarCtrlDefault( ElPolarCtrl* ctrl );

/* HessQrCtrl */
typedef struct {
  bool distAed;
  ElInt blockHeight, blockWidth;
} ElHessQrCtrl;
ElError ElHessQrCtrlDefault( ElHessQrCtrl* ctrl );

/* SdcCtrl */
typedef struct {
  ElInt cutoff;
  ElInt maxInnerIts, maxOuterIts;
  float tol;
  float spreadFactor;
  bool random;
  bool progress;
  ElSignCtrl_s signCtrl;
} ElSdcCtrl_s;
ElError ElSdcCtrlDefault_s( ElSdcCtrl_s* ctrl );

typedef struct {
  ElInt cutoff;
  ElInt maxInnerIts, maxOuterIts;
  double tol;
  double spreadFactor;
  bool random;
  bool progress;
  ElSignCtrl_d signCtrl;
} ElSdcCtrl_d;
ElError ElSdcCtrlDefault_d( ElSdcCtrl_d* ctrl );

/* SchurCtrl */
typedef struct {
  bool useSdc;
  ElHessQrCtrl qrCtrl;
  ElSdcCtrl_s sdcCtrl;
} ElSchurCtrl_s;
ElError ElSchurCtrlDefault_s( ElSchurCtrl_s* ctrl );

typedef struct {
  bool useSdc;
  ElHessQrCtrl qrCtrl;
  ElSdcCtrl_d sdcCtrl;
} ElSchurCtrl_d;
ElError ElSchurCtrlDefault_d( ElSchurCtrl_d* ctrl );

/* Hermitian eigensolvers
   ====================== */

/* Compute all eigenvalues
   ----------------------- */
ElError ElHermitianEig_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElSortType sort );
ElError ElHermitianEig_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElSortType sort );
ElError ElHermitianEig_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElSortType sort );
ElError ElHermitianEig_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElSortType sort );

/* NOTE: 'A' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianEigDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, 
  ElSortType sort );
ElError ElHermitianEigDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w,
  ElSortType sort );
ElError ElHermitianEigDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w,
  ElSortType sort );
ElError ElHermitianEigDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w,
  ElSortType sort );

/* TODO: Expert version */

/* Compute the entire eigenvalue decomposition 
   ------------------------------------------- */
ElError ElHermitianEigPair_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElMatrix_s Z,
  ElSortType sort );
ElError ElHermitianEigPair_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElMatrix_d Z,
  ElSortType sort );
ElError ElHermitianEigPair_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElMatrix_c Z,
  ElSortType sort );
ElError ElHermitianEigPair_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElMatrix_z Z,
  ElSortType sort );

/* NOTE: 'A' and 'Z' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianEigPairDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, ElDistMatrix_s Z,
  ElSortType sort );
ElError ElHermitianEigPairDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w, ElDistMatrix_d Z,
  ElSortType sort );
ElError ElHermitianEigPairDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort );
ElError ElHermitianEigPairDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort );

/* TODO: Expert version */

/* Compute a partial set of eigenvalues
   ------------------------------------ */
ElError ElHermitianEigPartial_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianEigPartial_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElHermitianEigPartial_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianEigPartial_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* NOTE: 'A' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianEigPartialDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianEigPartialDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElHermitianEigPartialDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianEigPartialDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Compute a partial set of eigenpairs
   ----------------------------------- */
ElError ElHermitianEigPairPartial_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElMatrix_s Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianEigPairPartial_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElMatrix_d Z,
  ElSortType sort, ElHermitianEigSubset_d subset );
ElError ElHermitianEigPairPartial_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElMatrix_c Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianEigPairPartial_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );

/* NOTE: 'A' and 'Z' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianEigPairPartialDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, ElDistMatrix_s Z, 
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianEigPairPartialDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w, ElDistMatrix_d Z,
  ElSortType sort, ElHermitianEigSubset_d subset );
ElError ElHermitianEigPairPartialDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianEigPairPartialDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Skew-Hermitian eigensolvers
   =========================== */

/* Compute all eigenvalues
   ----------------------- */
ElError ElSkewHermitianEig_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElSortType sort );
ElError ElSkewHermitianEig_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElSortType sort );
ElError ElSkewHermitianEig_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElSortType sort );
ElError ElSkewHermitianEig_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElSortType sort );

/* NOTE: 'A' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElSkewHermitianEigDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, 
  ElSortType sort );
ElError ElSkewHermitianEigDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w,
  ElSortType sort );
ElError ElSkewHermitianEigDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w,
  ElSortType sort );
ElError ElSkewHermitianEigDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w,
  ElSortType sort );

/* TODO: Expert version */

/* Compute the entire eigenvalue decomposition 
   ------------------------------------------- */
ElError ElSkewHermitianEigPair_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElMatrix_c Z,
  ElSortType sort );
ElError ElSkewHermitianEigPair_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElMatrix_z Z,
  ElSortType sort );
ElError ElSkewHermitianEigPair_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElMatrix_c Z,
  ElSortType sort );
ElError ElSkewHermitianEigPair_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElMatrix_z Z,
  ElSortType sort );

/* NOTE: 'A' and 'Z' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElSkewHermitianEigPairDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort );
ElError ElSkewHermitianEigPairDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort );
ElError ElSkewHermitianEigPairDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort );
ElError ElSkewHermitianEigPairDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort );

/* TODO: Expert version */

/* Compute a partial set of eigenvalues
   ------------------------------------ */
ElError ElSkewHermitianEigPartial_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElSkewHermitianEigPartial_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElSkewHermitianEigPartial_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElSkewHermitianEigPartial_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* NOTE: 'A' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElSkewHermitianEigPartialDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElSkewHermitianEigPartialDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElSkewHermitianEigPartialDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElSkewHermitianEigPartialDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Compute a partial set of eigenpairs
   ----------------------------------- */
ElError ElSkewHermitianEigPairPartial_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElMatrix_c Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElSkewHermitianEigPairPartial_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );
ElError ElSkewHermitianEigPairPartial_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElMatrix_c Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElSkewHermitianEigPairPartial_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );

/* NOTE: 'A' and 'Z' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElSkewHermitianEigPairPartialDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, ElDistMatrix_c Z, 
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElSkewHermitianEigPairPartialDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );
ElError ElSkewHermitianEigPairPartialDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElSkewHermitianEigPairPartialDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Hermitian generalized-definite eigensolvers
   =========================================== */

/* Compute all of the eigenvalues
   ------------------------------ */
ElError ElHermitianGenDefEig_s
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_s A, ElMatrix_s B, ElMatrix_s w, ElSortType sort );
ElError ElHermitianGenDefEig_d
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_d A, ElMatrix_d B, ElMatrix_d w, ElSortType sort );
ElError ElHermitianGenDefEig_c
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_c A, ElMatrix_c B, ElMatrix_s w, ElSortType sort );
ElError ElHermitianGenDefEig_z
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_z A, ElMatrix_z B, ElMatrix_d w, ElSortType sort );

/* NOTE: 'A' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianGenDefEigDist_s
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_s A, ElDistMatrix_s B, ElDistMatrix_s w, 
  ElSortType sort );
ElError ElHermitianGenDefEigDist_d
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_d A, ElDistMatrix_d B, ElDistMatrix_d w,
  ElSortType sort );
ElError ElHermitianGenDefEigDist_c
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_c A, ElDistMatrix_c B, ElDistMatrix_s w,
  ElSortType sort );
ElError ElHermitianGenDefEigDist_z
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_z A, ElDistMatrix_z B, ElDistMatrix_d w,
  ElSortType sort );

/* TODO: Expert version */

/* Compute the entire eigenvalue decomposition 
   ------------------------------------------- */
ElError ElHermitianGenDefEigPair_s
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_s A, ElMatrix_s B, ElMatrix_s w, ElMatrix_s Z,
  ElSortType sort );
ElError ElHermitianGenDefEigPair_d
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_d A, ElMatrix_d B, ElMatrix_d w, ElMatrix_d Z,
  ElSortType sort );
ElError ElHermitianGenDefEigPair_c
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_c A, ElMatrix_c B, ElMatrix_s w, ElMatrix_c Z,
  ElSortType sort );
ElError ElHermitianGenDefEigPair_z
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_z A, ElMatrix_z B, ElMatrix_d w, ElMatrix_z Z,
  ElSortType sort );

/* NOTE: 'A' and 'Z' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianGenDefEigPairDist_s
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_s A, ElDistMatrix_s B, ElDistMatrix_s w, ElDistMatrix_s Z,
  ElSortType sort );
ElError ElHermitianGenDefEigPairDist_d
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_d A, ElDistMatrix_d B, ElDistMatrix_d w, ElDistMatrix_d Z,
  ElSortType sort );
ElError ElHermitianGenDefEigPairDist_c
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_c A, ElDistMatrix_c B, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort );
ElError ElHermitianGenDefEigPairDist_z
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_z A, ElDistMatrix_z B, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort );

/* TODO: Expert version */

/* Compute a partial set of eigenvalues
   ------------------------------------ */
ElError ElHermitianGenDefEigPartial_s
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_s A, ElMatrix_s B, ElMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefEigPartial_d
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_d A, ElMatrix_d B, ElMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElHermitianGenDefEigPartial_c
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_c A, ElMatrix_c B, ElMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefEigPartial_z
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_z A, ElMatrix_z B, ElMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* NOTE: 'A' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianGenDefEigPartialDist_s
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_s A, ElDistMatrix_s B, ElDistMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefEigPartialDist_d
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_d A, ElDistMatrix_d B, ElDistMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElHermitianGenDefEigPartialDist_c
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_c A, ElDistMatrix_c B, ElDistMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefEigPartialDist_z
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_z A, ElDistMatrix_z B, ElDistMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Compute a partial set of eigenpairs
   ----------------------------------- */
ElError ElHermitianGenDefEigPairPartial_s
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_s A, ElMatrix_s B, ElMatrix_s w, ElMatrix_s Z, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefEigPairPartial_d
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_d A, ElMatrix_d B, ElMatrix_d w, ElMatrix_d Z, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElHermitianGenDefEigPairPartial_c
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_c A, ElMatrix_c B, ElMatrix_s w, ElMatrix_c Z, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefEigPairPartial_z
( ElPencil pencil, ElUpperOrLower uplo, 
  ElMatrix_z A, ElMatrix_z B, ElMatrix_d w, ElMatrix_z Z, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* NOTE: 'A' and 'Z' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianGenDefEigPairPartialDist_s
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_s A, ElDistMatrix_s B, ElDistMatrix_s w, ElDistMatrix_s Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefEigPairPartialDist_d
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_d A, ElDistMatrix_d B, ElDistMatrix_d w, ElDistMatrix_d Z,
  ElSortType sort, ElHermitianEigSubset_d subset );
ElError ElHermitianGenDefEigPairPartialDist_c
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_c A, ElDistMatrix_c B, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefEigPairPartialDist_z
( ElPencil pencil, ElUpperOrLower uplo, 
  ElDistMatrix_z A, ElDistMatrix_z B, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Hermitian tridiagonal eigensolvers
   ================================== */

/* Compute all eigenvalues
   ----------------------- */
ElError ElHermitianTridiagEig_s
( ElMatrix_s d, ElMatrix_s e, ElMatrix_s w, ElSortType sort );
ElError ElHermitianTridiagEig_d
( ElMatrix_d d, ElMatrix_d e, ElMatrix_d w, ElSortType sort );
ElError ElHermitianTridiagEig_c
( ElMatrix_s d, ElMatrix_c e, ElMatrix_s w, ElSortType sort );
ElError ElHermitianTridiagEig_z
( ElMatrix_d d, ElMatrix_z e, ElMatrix_d w, ElSortType sort );

/* NOTE: 'd', 'e', and 'w' must be in [VR,STAR] distributions */
/*
ElError ElHermitianTridiagEigDist_s
( ElConstDistMatrix_s d, ElConstDistMatrix_s e, 
  ElDistMatrix_s w, ElSortType sort );
*/
ElError ElHermitianTridiagEigDist_d
( ElConstDistMatrix_d d, ElConstDistMatrix_d e, 
  ElDistMatrix_d w, ElSortType sort );
/*
ElError ElHermitianTridiagEigDist_c
( ElConstDistMatrix_s d, ElConstDistMatrix_c e, 
  ElDistMatrix_s w, ElSortType sort );
*/
ElError ElHermitianTridiagEigDist_z
( ElConstDistMatrix_d d, ElConstDistMatrix_z e, 
  ElDistMatrix_d w, ElSortType sort );

/* TODO: Expert version */

/* Compute all eigenpairs
   ---------------------- */
ElError ElHermitianTridiagEigPair_s
( ElMatrix_s d, ElMatrix_s e, ElMatrix_s w, ElMatrix_s Z, ElSortType sort );
ElError ElHermitianTridiagEigPair_d
( ElMatrix_d d, ElMatrix_d e, ElMatrix_d w, ElMatrix_d Z, ElSortType sort );
ElError ElHermitianTridiagEigPair_c
( ElMatrix_s d, ElMatrix_c e, ElMatrix_s w, ElMatrix_c Z, ElSortType sort );
ElError ElHermitianTridiagEigPair_z
( ElMatrix_d d, ElMatrix_z e, ElMatrix_d w, ElMatrix_z Z, ElSortType sort );

/* NOTE: 'd', 'e', and 'w' must be in [VR,STAR] distributions, while
         'Z' must be in a [STAR,VR] distribution */ 
/*
ElError ElHermitianTridiagEigPairDist_s
( ElConstDistMatrix_s d, ElConstDistMatrix_s e, 
  ElDistMatrix_s w, ElDistMatrix_s Z, ElSortType sort );
*/
ElError ElHermitianTridiagEigPairDist_d
( ElConstDistMatrix_d d, ElConstDistMatrix_d e, 
  ElDistMatrix_d w, ElDistMatrix_d Z, ElSortType sort );
/*
ElError ElHermitianTridiagEigPairDist_c
( ElConstDistMatrix_s d, ElConstDistMatrix_c e, 
  ElDistMatrix_s w, ElDistMatrix_c Z, ElSortType sort );
*/
ElError ElHermitianTridiagEigPairDist_z
( ElConstDistMatrix_d d, ElConstDistMatrix_z e, 
  ElDistMatrix_d w, ElDistMatrix_z Z, ElSortType sort );

/* TODO: Expert version */

/* Compute a subset of eigenvalues
   ------------------------------- */
ElError ElHermitianTridiagEigPartial_s
( ElMatrix_s d, ElMatrix_s e, 
  ElMatrix_s w, ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianTridiagEigPartial_d
( ElMatrix_d d, ElMatrix_d e, 
  ElMatrix_d w, ElSortType sort, ElHermitianEigSubset_d subset );
ElError ElHermitianTridiagEigPartial_c
( ElMatrix_s d, ElMatrix_c e, 
  ElMatrix_s w, ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianTridiagEigPartial_z
( ElMatrix_d d, ElMatrix_z e, 
  ElMatrix_d w, ElSortType sort, ElHermitianEigSubset_d subset );

/* NOTE: 'd', 'e', and 'w' must be in [VR,STAR] distributions */
/*
ElError ElHermitianTridiagEigPartialDist_s
( ElConstDistMatrix_s d, ElConstDistMatrix_s e, 
  ElDistMatrix_s w, ElSortType sort, ElHermitianEigSubset_s subset );
*/
ElError ElHermitianTridiagEigPartialDist_d
( ElConstDistMatrix_d d, ElConstDistMatrix_d e, 
  ElDistMatrix_d w, ElSortType sort, ElHermitianEigSubset_d subset );
/*
ElError ElHermitianTridiagEigPartialDist_c
( ElConstDistMatrix_s d, ElConstDistMatrix_c e, 
  ElDistMatrix_s w, ElSortType sort, ElHermitianEigSubset_s subset );
*/
ElError ElHermitianTridiagEigPartialDist_z
( ElConstDistMatrix_d d, ElConstDistMatrix_z e, 
  ElDistMatrix_d w, ElSortType sort, ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Compute a subset of eigenpairs
   ------------------------------ */
ElError ElHermitianTridiagEigPairPartial_s
( ElMatrix_s d, ElMatrix_s e, 
  ElMatrix_s w,  ElMatrix_s Z, ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianTridiagEigPairPartial_d
( ElMatrix_d d, ElMatrix_d e, 
  ElMatrix_d w, ElMatrix_d Z, ElSortType sort, ElHermitianEigSubset_d subset );
ElError ElHermitianTridiagEigPairPartial_c
( ElMatrix_s d, ElMatrix_c e, 
  ElMatrix_s w, ElMatrix_c Z, ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianTridiagEigPairPartial_z
( ElMatrix_d d, ElMatrix_z e, 
  ElMatrix_d w, ElMatrix_z Z, ElSortType sort, ElHermitianEigSubset_d subset );

/* NOTE: 'd', 'e', and 'w' must be in [VR,STAR] distributions, while
         'Z' must be in a [STAR,VR] distribution */ 
/*
ElError ElHermitianTridiagEigPairPartialDist_s
( ElConstDistMatrix_s d, ElConstDistMatrix_s e, 
  ElDistMatrix_s w, ElDistMatrix_s Z, 
  ElSortType sort, ElHermitianEigSubset_s subset );
*/
ElError ElHermitianTridiagEigPairPartialDist_d
( ElConstDistMatrix_d d, ElConstDistMatrix_d e, 
  ElDistMatrix_d w, ElDistMatrix_d Z,
  ElSortType sort, ElHermitianEigSubset_d subset );
/*
ElError ElHermitianTridiagEigPairPartialDist_c
( ElConstDistMatrix_s d, ElConstDistMatrix_c e, 
  ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
*/
ElError ElHermitianTridiagEigPairPartialDist_z
( ElConstDistMatrix_d d, ElConstDistMatrix_z e, 
  ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Polar decomposition
   =================== */

/* Compute just the polar factor
   ----------------------------- */
ElError ElPolar_s( ElMatrix_s A );
ElError ElPolar_d( ElMatrix_d A );
ElError ElPolar_c( ElMatrix_c A );
ElError ElPolar_z( ElMatrix_z A );

/* NOTE: 'A' and 'P' must be in [MC,MR] distributions */
ElError ElPolarDist_s( ElDistMatrix_s A );
ElError ElPolarDist_d( ElDistMatrix_d A );
ElError ElPolarDist_c( ElDistMatrix_c A );
ElError ElPolarDist_z( ElDistMatrix_z A );

ElError ElHermitianPolar_s( ElUpperOrLower uplo, ElMatrix_s A );
ElError ElHermitianPolar_d( ElUpperOrLower uplo, ElMatrix_d A );
ElError ElHermitianPolar_c( ElUpperOrLower uplo, ElMatrix_c A );
ElError ElHermitianPolar_z( ElUpperOrLower uplo, ElMatrix_z A );

/* NOTE: 'A' and 'P' must be in [MC,MR] distributions */
ElError ElHermitianPolarDist_s( ElUpperOrLower uplo, ElDistMatrix_s A );
ElError ElHermitianPolarDist_d( ElUpperOrLower uplo, ElDistMatrix_d A );
ElError ElHermitianPolarDist_c( ElUpperOrLower uplo, ElDistMatrix_c A );
ElError ElHermitianPolarDist_z( ElUpperOrLower uplo, ElDistMatrix_z A );

/* Compute the entire polar decomposition
   -------------------------------------- */
ElError ElPolarDecomp_s( ElMatrix_s A, ElMatrix_s P );
ElError ElPolarDecomp_d( ElMatrix_d A, ElMatrix_d P );
ElError ElPolarDecomp_c( ElMatrix_c A, ElMatrix_c P );
ElError ElPolarDecomp_z( ElMatrix_z A, ElMatrix_z P );

/* NOTE: 'A' and 'P' must be in [MC,MR] distributions */
ElError ElPolarDecompDist_s( ElDistMatrix_s A, ElDistMatrix_s P );
ElError ElPolarDecompDist_d( ElDistMatrix_d A, ElDistMatrix_d P );
ElError ElPolarDecompDist_c( ElDistMatrix_c A, ElDistMatrix_c P );
ElError ElPolarDecompDist_z( ElDistMatrix_z A, ElDistMatrix_z P );

ElError ElHermitianPolarDecomp_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s P );
ElError ElHermitianPolarDecomp_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d P );
ElError ElHermitianPolarDecomp_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_c P );
ElError ElHermitianPolarDecomp_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_z P );

/* NOTE: 'A' and 'P' must be in [MC,MR] distributions */
ElError ElHermitianPolarDecompDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s P );
ElError ElHermitianPolarDecompDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d P );
ElError ElHermitianPolarDecompDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_c P );
ElError ElHermitianPolarDecompDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_z P );

/* TODO: Expert versions */

/* Schur decomposition
   =================== */

/* Compute just the eigenvalues (and perhaps the Schur factor)
   ----------------------------------------------------------- */
ElError ElSchur_s( ElMatrix_s A, ElMatrix_c w, bool fullTriangle );
ElError ElSchur_d( ElMatrix_d A, ElMatrix_z w, bool fullTriangle );
ElError ElSchur_c( ElMatrix_c A, ElMatrix_c w, bool fullTriangle );
ElError ElSchur_z( ElMatrix_z A, ElMatrix_z w, bool fullTriangle );

/* NOTE: 'A' must be in a [MC,MR] distribution, while
         'w' must be in a [VR,STAR] distribution  */
ElError ElSchurDist_s( ElDistMatrix_s A, ElDistMatrix_c w, bool fullTriangle );
ElError ElSchurDist_d( ElDistMatrix_d A, ElDistMatrix_z w, bool fullTriangle );
ElError ElSchurDist_c( ElDistMatrix_c A, ElDistMatrix_c w, bool fullTriangle );
ElError ElSchurDist_z( ElDistMatrix_z A, ElDistMatrix_z w, bool fullTriangle );

/* Compute the eigvalues and Schur vectors (and possibly Schur factor)
   ------------------------------------------------------------------- */
ElError ElSchurDecomp_s
( ElMatrix_s A, ElMatrix_c w, ElMatrix_s Q, bool fullTriangle );
ElError ElSchurDecomp_d
( ElMatrix_d A, ElMatrix_z w, ElMatrix_d Q, bool fullTriangle );
ElError ElSchurDecomp_c
( ElMatrix_c A, ElMatrix_c w, ElMatrix_c Q, bool fullTriangle );
ElError ElSchurDecomp_z
( ElMatrix_z A, ElMatrix_z w, ElMatrix_z Q, bool fullTriangle );

/* NOTE: 'A' and 'Q' must be in [MC,MR] distributions, while
         'w' must be in a [VR,STAR] distribution  */
ElError ElSchurDecompDist_s
( ElDistMatrix_s A, ElDistMatrix_c w, ElDistMatrix_s Q, bool fullTriangle );
ElError ElSchurDecompDist_d
( ElDistMatrix_d A, ElDistMatrix_z w, ElDistMatrix_d Q, bool fullTriangle );
ElError ElSchurDecompDist_c
( ElDistMatrix_c A, ElDistMatrix_c w, ElDistMatrix_c Q, bool fullTriangle );
ElError ElSchurDecompDist_z
( ElDistMatrix_z A, ElDistMatrix_z w, ElDistMatrix_z Q, bool fullTriangle );

/* TODO: Expert versions */
/* TODO: QuasiTriangEig, CheckRealSchur, and RealToComplex */

/* Singular Value Decomposition (SVD)
   ================================== */

/* Compute the singular values
   --------------------------- */
ElError ElSingularValues_s( ElMatrix_s A, ElMatrix_s s );
ElError ElSingularValues_d( ElMatrix_d A, ElMatrix_d s );
ElError ElSingularValues_c( ElMatrix_c A, ElMatrix_s s );
ElError ElSingularValues_z( ElMatrix_z A, ElMatrix_d s );

/* NOTE: 'A' must be in a [MC,MR] distribution, while
         's' must be in a [VR,STAR] distribution */
ElError ElSingularValuesDist_s( ElDistMatrix_s A, ElDistMatrix_s s );
ElError ElSingularValuesDist_d( ElDistMatrix_d A, ElDistMatrix_d s );
ElError ElSingularValuesDist_c( ElDistMatrix_c A, ElDistMatrix_s s );
ElError ElSingularValuesDist_z( ElDistMatrix_z A, ElDistMatrix_d s );

/* Compute the full SVD
   -------------------- */
ElError ElSVD_s( ElMatrix_s A, ElMatrix_s s, ElMatrix_s V );
ElError ElSVD_d( ElMatrix_d A, ElMatrix_d s, ElMatrix_d V );
ElError ElSVD_c( ElMatrix_c A, ElMatrix_s s, ElMatrix_c V );
ElError ElSVD_z( ElMatrix_z A, ElMatrix_d s, ElMatrix_z V );

/* NOTE: 'A' and 'V' must be in [MC,MR] distributions, while
         's' must be in a [VR,STAR] distribution */
ElError ElSVDDist_s( ElDistMatrix_s A, ElDistMatrix_s s, ElDistMatrix_s V );
ElError ElSVDDist_d( ElDistMatrix_d A, ElDistMatrix_d s, ElDistMatrix_d V );
ElError ElSVDDist_c( ElDistMatrix_c A, ElDistMatrix_s s, ElDistMatrix_c V );
ElError ElSVDDist_z( ElDistMatrix_z A, ElDistMatrix_d s, ElDistMatrix_z V );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* ifndef EL_LAPACK_SPECTRAL_C_H */
