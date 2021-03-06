/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.
   Copyright (c) 2014, Jed Brown
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"
#include "El-C.h"
using namespace El;

template<typename T>
ElError ElDistMatrixCreateSpecific
( ElDist U_C, ElDist V_C, ElConstGrid grid, AbstractDistMatrix<T>** A )
{
    try 
    {
        Dist U = CReflect(U_C);
        Dist V = CReflect(V_C);
        auto gridPtr = CReflect(grid);

        #define GUARD(CDIST,RDIST) U == CDIST && V == RDIST
        #define PAYLOAD(CDIST,RDIST) \
          *A = new DistMatrix<T,CDIST,RDIST>(*gridPtr);
        #include "El/macros/GuardAndPayload.h"
        #undef GUARD
        #undef PAYLOAD
    }
    EL_CATCH
    return EL_SUCCESS;
}

// DistMatrix<T,UDiag,VDiag> DistMatrix<T,U,V>::GetDiagonal( Int offset ) const
// ----------------------------------------------------------------------------
template<typename T>
ElError ElDistMatrixGetDiagonal
( const AbstractDistMatrix<T>* AAbs, Int offset, 
        AbstractDistMatrix<T>** dAbs )
{
    const Grid& g = AAbs->Grid();
    Dist U = AAbs->DistData().colDist;
    Dist V = AAbs->DistData().rowDist;
    try 
    {
        #define GUARD(CDIST,RDIST) U == CDIST && V == RDIST
        #define PAYLOAD(CDIST,RDIST) \
            auto A = dynamic_cast<const DistMatrix<T,CDIST,RDIST>*>(AAbs); \
            DynamicCastCheck(A); \
            auto* d = new DistMatrix<T,DiagColDist<CDIST,RDIST>(), \
                                       DiagRowDist<CDIST,RDIST>()>(g); \
            A->GetDiagonal( *d, offset ); \
            *dAbs = d;
        #include "El/macros/GuardAndPayload.h"
        #undef GUARD
        #undef PAYLOAD
    }
    EL_CATCH
    return EL_SUCCESS;
}

// DistMatrix<Base<T>,UDiag,VDiag> 
// DistMatrix<T,U,V>::GetRealPartOfDiagonal( Int offset ) const
// ------------------------------------------------------------
template<typename T>
ElError ElDistMatrixGetRealPartOfDiagonal
( const AbstractDistMatrix<Complex<T>>* AAbs, Int offset, 
        AbstractDistMatrix<T>** dAbs )
{
    const Grid& g = AAbs->Grid();
    Dist U = AAbs->DistData().colDist;
    Dist V = AAbs->DistData().rowDist;
    try 
    {
        #define GUARD(CDIST,RDIST) U == CDIST && V == RDIST
        #define PAYLOAD(CDIST,RDIST) \
            auto A = \
              dynamic_cast<const DistMatrix<Complex<T>,CDIST,RDIST>*>(AAbs); \
            DynamicCastCheck(A); \
            auto* d = new DistMatrix<T,DiagColDist<CDIST,RDIST>(), \
                                       DiagRowDist<CDIST,RDIST>()>(g); \
            A->GetRealPartOfDiagonal( *d, offset ); \
            *dAbs = d;
        #include "El/macros/GuardAndPayload.h"
        #undef GUARD
        #undef PAYLOAD
    }
    EL_CATCH
    return EL_SUCCESS;
}

// DistMatrix<Base<T>,UDiag,VDiag> 
// DistMatrix<T,U,V>::GetImagPartOfDiagonal( Int offset ) const
// ------------------------------------------------------------
template<typename T>
ElError ElDistMatrixGetImagPartOfDiagonal
( const AbstractDistMatrix<Complex<T>>* AAbs, Int offset, 
        AbstractDistMatrix<T>** dAbs )
{
    const Grid& g = AAbs->Grid();
    Dist U = AAbs->DistData().colDist;
    Dist V = AAbs->DistData().rowDist;
    try 
    {
        #define GUARD(CDIST,RDIST) U == CDIST && V == RDIST
        #define PAYLOAD(CDIST,RDIST) \
            auto A = \
              dynamic_cast<const DistMatrix<Complex<T>,CDIST,RDIST>*>(AAbs); \
            DynamicCastCheck(A); \
            auto* d = new DistMatrix<T,DiagColDist<CDIST,RDIST>(), \
                                       DiagRowDist<CDIST,RDIST>()>(g); \
            A->GetImagPartOfDiagonal( *d, offset ); \
            *dAbs = d;
        #include "El/macros/GuardAndPayload.h"
        #undef GUARD
        #undef PAYLOAD
    }
    EL_CATCH
    return EL_SUCCESS;
}

extern "C" {

#define DISTMATRIX_CREATE(SIG,SIGBASE,T) \
  /* DistMatrix<T,MC,MR>::DistMatrix( const Grid& g ) */ \
  ElError ElDistMatrixCreate_ ## SIG \
  ( ElConstGrid grid, ElDistMatrix_ ## SIG *A ) \
  { EL_TRY( auto gridPtr = CReflect(grid); \
            *A = CReflect( new DistMatrix<T>(*gridPtr) ) ) } \
  /* DistMatrix<T,U,V>::DistMatrix( const Grid& g ) */ \
  ElError ElDistMatrixCreateSpecific_ ## SIG \
  ( ElDist U, ElDist V, ElConstGrid grid, \
    ElDistMatrix_ ## SIG *A ) \
  { \
    AbstractDistMatrix<T>* ADM; \
    ElError error = ElDistMatrixCreateSpecific( U, V, grid, &ADM ); \
    *A = CReflect(ADM); \
    return error; \
  } \
  /* DistMatrix<T,U,V>::~DistMatrix() */ \
  ElError ElDistMatrixDestroy_ ## SIG ( ElConstDistMatrix_ ## SIG A ) \
  { EL_TRY( CReflect(A) ) } 

#define DISTMATRIX_RECONFIG(SIG,SIGBASE,T) \
  /* void DistMatrix<T,U,V>::Empty() */ \
  ElError ElDistMatrixEmpty_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( CReflect(A)->Empty() ) } \
  /* void DistMatrix<T,U,V>::EmptyData() */ \
  ElError ElDistMatrixEmptyData_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( CReflect(A)->EmptyData() ) } \
  /* void DistMatrix<T,U,V>::SetGrid( const Grid& g ) */ \
  ElError ElDistMatrixSetGrid_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElConstGrid grid ) \
  { EL_TRY( CReflect(A)->SetGrid(*CReflect(grid)) ) } \
  /* void DistMatrix<T,U,V>::Resize( Int height, Int width ) */ \
  ElError ElDistMatrixResize_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt height, ElInt width ) \
  { EL_TRY( CReflect(A)->Resize(height,width) ) } \
  /* void DistMatrix<T,U,V>::Resize( Int height, Int width, Int ldim ) */ \
  ElError ElDistMatrixResizeWithLDim_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt height, ElInt width, ElInt ldim ) \
  { EL_TRY( CReflect(A)->Resize(height,width,ldim) ) } \
  /* void DistMatrix<T,U,V>::MakeConsistent() */ \
  ElError ElDistMatrixMakeConsistent_ ## SIG \
  ( ElDistMatrix_ ## SIG A, bool includeViewers ) \
  { EL_TRY( CReflect(A)->MakeConsistent(includeViewers) ) } \
  /* void DistMatrix<T,U,V>::MakeSizeConsistent() */ \
  ElError ElDistMatrixMakeSizeConsistent_ ## SIG \
  ( ElDistMatrix_ ## SIG A, bool includeViewers ) \
  { EL_TRY( CReflect(A)->MakeSizeConsistent(includeViewers) ) } \
  /* void DistMatrix<T,U,V>::Align \
     ( Int colAlign, Int rowAlign, bool constrain ) */ \
  ElError ElDistMatrixAlign_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt colAlign, ElInt rowAlign, \
    bool constrain ) \
  { EL_TRY( CReflect(A)->Align(colAlign,rowAlign,constrain) ) } \
  /* void DistMatrix<T,U,V>::AlignCols( Int colAlign, bool constrain ) */ \
  ElError ElDistMatrixAlignCols_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt colAlign, bool constrain ) \
  { EL_TRY( CReflect(A)->AlignCols(colAlign,constrain) ) } \
   /* void DistMatrix<T,U,V>::AlignRows( Int rowAlign, bool constrain ) */ \
  ElError ElDistMatrixAlignRows_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt rowAlign, bool constrain ) \
  { EL_TRY( CReflect(A)->AlignRows(rowAlign,constrain) ) } \
  /* void DistMatrix<T,U,V>::FreeAlignments() */ \
  ElError ElDistMatrixFreeAlignments_ ## SIG ( ElDistMatrix_ ## SIG A ) \
  { EL_TRY( CReflect(A)->FreeAlignments() ) } \
  /* void DistMatrix<T,U,V>::SetRoot( Int root ) */ \
  ElError ElDistMatrixSetRoot_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt root ) \
  { EL_TRY( CReflect(A)->SetRoot(root) ) } \
  /* void DistMatrix<T,U,V>::AlignWith \
     ( const DistData& data, bool constrain ) */ \
  ElError ElDistMatrixAlignWith_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistData distData, bool constrain ) \
  { EL_TRY( CReflect(A)->AlignWith \
            ( CReflect(distData), constrain ) ) } \
  /* void DistMatrix<T,U,V>::AlignColsWith \
     ( const DistData& data, bool constrain ) */ \
  ElError ElDistMatrixAlignColsWith_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistData distData, bool constrain ) \
  { EL_TRY( CReflect(A)->AlignColsWith \
            ( CReflect(distData), constrain ) ) } \
  /* void DistMatrix<T,U,V>::AlignRowsWith \
     ( const DistData& data, bool constrain ) */ \
  ElError ElDistMatrixAlignRowsWith_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElDistData distData, bool constrain ) \
  { EL_TRY( CReflect(A)->AlignRowsWith \
            ( CReflect(distData), constrain ) ) } \
  /* void DistMatrix<T,U,V>::AlignAndResize \
     ( Int colAlign, Int rowAlign, Int height, Int width, \
       bool force, bool constrain ) */ \
  ElError ElDistMatrixAlignAndResize_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    ElInt colAlign, ElInt rowAlign, ElInt height, ElInt width, \
    bool force, bool constrain ) \
  { EL_TRY( CReflect(A)->AlignAndResize \
            (colAlign,rowAlign,height,width,force,constrain) ) } \
  /* void DistMatrix<T,U,V>::AlignColsAndResize
     ( Int colAlign, Int height, Int width, bool force, bool constrain ) */ \
  ElError ElDistMatrixAlignColsAndResize_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    ElInt colAlign, ElInt height, ElInt width, bool force, bool constrain ) \
  { EL_TRY( CReflect(A)->AlignColsAndResize \
            (colAlign,height,width,force,constrain) ) } \
  /* void DistMatrix<T,U,V>::AlignRowsAndResize
     ( Int rowAlign, Int height, Int width, bool force, bool constrain ) */ \
  ElError ElDistMatrixAlignRowsAndResize_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    ElInt rowAlign, ElInt height, ElInt width, bool force, bool constrain ) \
  { EL_TRY( CReflect(A)->AlignRowsAndResize \
            (rowAlign,height,width,force,constrain) ) } \
  /* void DistMatrix<T,U,V>::Attach
     ( Int height, Int width, const Grid& grid, Int colAlign, Int rowAlign, 
       T* buffer, Int ldim, Int root ) */ \
  ElError ElDistMatrixAttach_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt height, ElInt width, \
    ElConstGrid grid, ElInt colAlign, ElInt rowAlign, \
    CREFLECT(T)* buffer, ElInt ldim, ElInt root ) \
  { EL_TRY( CReflect(A)->Attach \
            (height,width,*CReflect(grid),colAlign,rowAlign, \
             CReflect(buffer),ldim,root) ) } \
  /* void DistMatrix<T,U,V>::LockedAttach
     ( Int height, Int width, const Grid& grid, Int colAlign, Int rowAlign, 
       const T* buffer, Int ldim, Int root ) */ \
  ElError ElDistMatrixLockedAttach_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt height, ElInt width, \
    ElConstGrid grid, ElInt colAlign, ElInt rowAlign, \
    const CREFLECT(T)* buffer, ElInt ldim, ElInt root ) \
  { EL_TRY( CReflect(A)->LockedAttach \
            (height,width,*CReflect(grid),colAlign,rowAlign, \
             CReflect(buffer),ldim,root) ) }

#define DISTMATRIX_BASIC(SIG,SIGBASE,T) \
  /* Int DistMatrix<T,U,V>::Height() const */ \
  ElError ElDistMatrixHeight_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* height ) \
  { EL_TRY( *height = CReflect(A)->Height() ) } \
  /* Int DistMatrix<T,U,V>::Width() const */ \
  ElError ElDistMatrixWidth_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* width ) \
  { EL_TRY( *width = CReflect(A)->Width() ) } \
  /* Int DistMatrix<T,U,V>::DiagonalLength( Int offset ) const */ \
  ElError ElDistMatrixDiagonalLength_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt offset, ElInt* length ) \
  { EL_TRY( *length = CReflect(A)->DiagonalLength(offset) ) } \
  /* bool DistMatrix<T,U,V>::Viewing() const */ \
  ElError ElDistMatrixViewing_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, bool* viewing ) \
  { EL_TRY( *viewing = CReflect(A)->Viewing() ) } \
  /* bool DistMatrix<T,U,V>::Locked() const */ \
  ElError ElDistMatrixLocked_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, bool* locked ) \
  { EL_TRY( *locked = CReflect(A)->Locked() ) } \
  /* Int DistMatrix<T,U,V>::LocalHeight() const */ \
  ElError ElDistMatrixLocalHeight_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* localHeight ) \
  { EL_TRY( *localHeight = CReflect(A)->LocalHeight() ) } \
  /* Int DistMatrix<T,U,V>::LocalWidth() const */ \
  ElError ElDistMatrixLocalWidth_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* localWidth ) \
  { EL_TRY( *localWidth = CReflect(A)->LocalWidth() ) } \
  /* Int DistMatrix<T,U,V>::LDim() const */ \
  ElError ElDistMatrixLDim_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* ldim ) \
  { EL_TRY( *ldim = CReflect(A)->LDim() ) } \
  /* Matrix<T>& DistMatrix<T,U,V>::Matrix() */ \
  ElError ElDistMatrixMatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElMatrix_ ## SIG *ALoc ) \
  { EL_TRY( *ALoc = CReflect(&CReflect(A)->Matrix()) ) } \
  /* const Matrix<T>& DistMatrix<T,U,V>::LockedMatrix() const */ \
  ElError ElDistMatrixLockedMatrix_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElConstMatrix_ ## SIG *ALoc ) \
  { EL_TRY( *ALoc = CReflect \
                          (&CReflect(A)->LockedMatrix()) ) } \
  /* size_t DistMatrix<T,U,V>::AllocatedMemory() const */ \
  ElError ElDistMatrixAllocatedMemory_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, size_t* mem ) \
  { EL_TRY( *mem = CReflect(A)->AllocatedMemory() ) } \
  /* T* DistMatrix<T,U,V>::Buffer() */ \
  ElError ElDistMatrixBuffer_ ## SIG \
  ( ElDistMatrix_ ## SIG A, CREFLECT(T)** buffer ) \
  { EL_TRY( *buffer = CReflect(CReflect(A)->Buffer()) ) } \
  /* const T* DistMatrix<T,U,V>::LockedBuffer() const */ \
  ElError ElDistMatrixLockedBuffer_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, const CREFLECT(T)** buffer ) \
  { EL_TRY( *buffer = CReflect(CReflect(A)->LockedBuffer()) ) } \
  /* const Grid& DistMatrix<T,U,V>::Grid() const */ \
  ElError ElDistMatrixGrid_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElConstGrid* grid ) \
  { EL_TRY( *grid = CReflect(&CReflect(A)->Grid()) ) } \
  /* bool DistMatrix<T,U,V>::ColConstrained() const */ \
  ElError ElDistMatrixColConstrained_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, bool* colConst ) \
  { EL_TRY( *colConst = CReflect(A)->ColConstrained() ) } \
  /* bool DistMatrix<T,U,V>::RowConstrained() const */ \
  ElError ElDistMatrixRowConstrained_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, bool* rowConst ) \
  { EL_TRY( *rowConst = CReflect(A)->RowConstrained() ) } \
  /* bool DistMatrix<T,U,V>::RootConstrained() const */ \
  ElError ElDistMatrixRootConstrained_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, bool* rootConst ) \
  { EL_TRY( *rootConst = CReflect(A)->RootConstrained() ) } \
  /* Int DistMatrix<T,U,V>::ColAlign() const */ \
  ElError ElDistMatrixColAlign_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* colAlign ) \
  { EL_TRY( *colAlign = CReflect(A)->ColAlign() ) } \
  /* Int DistMatrix<T,U,V>::RowAlign() const */ \
  ElError ElDistMatrixRowAlign_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* rowAlign ) \
  { EL_TRY( *rowAlign = CReflect(A)->RowAlign() ) } \
  /* Int DistMatrix<T,U,V>::ColShift() const */ \
  ElError ElDistMatrixColShift_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* colShift ) \
  { EL_TRY( *colShift = CReflect(A)->ColShift() ) } \
  /* Int DistMatrix<T,U,V>::RowShift() const */ \
  ElError ElDistMatrixRowShift_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* rowShift ) \
  { EL_TRY( *rowShift = CReflect(A)->RowShift() ) } \
  /* Int DistMatrix<T,U,V>::ColRank() const */ \
  ElError ElDistMatrixColRank_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* colRank ) \
  { EL_TRY( *colRank = CReflect(A)->ColRank() ) } \
  /* Int DistMatrix<T,U,V>::RowRank() const */ \
  ElError ElDistMatrixRowRank_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* rowRank ) \
  { EL_TRY( *rowRank = CReflect(A)->RowRank() ) } \
  /* Int DistMatrix<T,U,V>::PartialColRank() const */ \
  ElError ElDistMatrixPartialColRank_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* rank ) \
  { EL_TRY( *rank = CReflect(A)->PartialColRank() ) } \
  /* Int DistMatrix<T,U,V>::PartialRowRank() const */ \
  ElError ElDistMatrixPartialRowRank_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* rank ) \
  { EL_TRY( *rank = CReflect(A)->PartialRowRank() ) } \
  /* Int DistMatrix<T,U,V>::PartialUnionColRank() const */ \
  ElError ElDistMatrixPartialUnionColRank_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* rank ) \
  { EL_TRY( *rank = CReflect(A)->PartialUnionColRank() ) } \
  /* Int DistMatrix<T,U,V>::PartialUnionRowRank() const */ \
  ElError ElDistMatrixPartialUnionRowRank_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* rank ) \
  { EL_TRY( *rank = CReflect(A)->PartialUnionRowRank() ) } \
  /* Int DistMatrix<T,U,V>::DistRank() const */ \
  ElError ElDistMatrixDistRank_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* rank ) \
  { EL_TRY( *rank = CReflect(A)->DistRank() ) } \
  /* Int DistMatrix<T,U,V>::CrossRank() const */ \
  ElError ElDistMatrixCrossRank_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* rank ) \
  { EL_TRY( *rank = CReflect(A)->CrossRank() ) } \
  /* Int DistMatrix<T,U,V>::RedundantRank() const */ \
  ElError ElDistMatrixRedundantRank_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* rank ) \
  { EL_TRY( *rank = CReflect(A)->RedundantRank() ) } \
  /* Int DistMatrix<T,U,V>::Root() const */ \
  ElError ElDistMatrixRoot_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* root ) \
  { EL_TRY( *root = CReflect(A)->Root() ) } \
  /* bool DistMatrix<T,U,V>::Participating() const */ \
  ElError ElDistMatrixParticipating_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, bool* participating ) \
  { EL_TRY( *participating = CReflect(A)->Participating() ) } \
  /* Int DistMatrix<T,U,V>::RowOwner( Int i ) const */ \
  ElError ElDistMatrixRowOwner_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt i, ElInt* rowOwner ) \
  { EL_TRY( *rowOwner = CReflect(A)->RowOwner(i) ) } \
  /* Int DistMatrix<T,U,V>::ColOwner( Int i ) const */ \
  ElError ElDistMatrixColOwner_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt j, ElInt* colOwner ) \
  { EL_TRY( *colOwner = CReflect(A)->ColOwner(j) ) } \
  /* Int DistMatrix<T,U,V>::Owner( Int i, Int j ) const */ \
  ElError ElDistMatrixOwner_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt i, ElInt j, ElInt* owner ) \
  { EL_TRY( *owner = CReflect(A)->Owner(i,j) ) } \
  /* Int DistMatrix<T,U,V>::LocalRow( Int i ) const */ \
  ElError ElDistMatrixLocalRow_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt i, ElInt* iLoc ) \
  { EL_TRY( *iLoc = CReflect(A)->LocalRow(i) ) } \
  /* Int DistMatrix<T,U,V>::LocalCol( Int j ) const */ \
  ElError ElDistMatrixLocalCol_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt j, ElInt* jLoc ) \
  { EL_TRY( *jLoc = CReflect(A)->LocalCol(j) ) } \
  /* Int DistMatrix<T,U,V>::LocalRowOffset( Int i ) const */ \
  ElError ElDistMatrixLocalRowOffset_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt i, ElInt* iLoc ) \
  { EL_TRY( *iLoc = CReflect(A)->LocalRowOffset(i) ) } \
  /* Int DistMatrix<T,U,V>::LocalColOffset( Int j ) const */ \
  ElError ElDistMatrixLocalColOffset_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt j, ElInt* jLoc ) \
  { EL_TRY( *jLoc = CReflect(A)->LocalColOffset(j) ) } \
  /* Int DistMatrix<T,U,V>::GlobalRow( Int iLoc ) const */ \
  ElError ElDistMatrixGlobalRow_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt iLoc, ElInt* i ) \
  { EL_TRY( *i = CReflect(A)->GlobalRow(iLoc) ) } \
  /* Int DistMatrix<T,U,V>::GlobalCol( Int j ) const */ \
  ElError ElDistMatrixGlobalCol_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt jLoc, ElInt* j ) \
  { EL_TRY( *j = CReflect(A)->GlobalCol(jLoc) ) } \
  /* bool DistMatrix<T,U,V>::IsLocalRow( Int i ) const */ \
  ElError ElDistMatrixIsLocalRow_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt i, bool* isLocal ) \
  { EL_TRY( *isLocal = CReflect(A)->IsLocalRow(i) ) } \
  /* bool DistMatrix<T,U,V>::IsLocalCol( Int j ) const */ \
  ElError ElDistMatrixIsLocalCol_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt j, bool* isLocal ) \
  { EL_TRY( *isLocal = CReflect(A)->IsLocalCol(j) ) } \
  /* bool DistMatrix<T,U,V>::IsLocal( Int i, Int j ) const */ \
  ElError ElDistMatrixIsLocal_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt i, ElInt j, bool* isLocal ) \
  { EL_TRY( *isLocal = CReflect(A)->IsLocal(i,j) ) } \
  /* DistData DistMatrix<T,U,V>::DistData() const */ \
  ElError ElDistMatrixDistData_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElDistData* distData ) \
  { EL_TRY( DistData data = CReflect(A)->DistData(); \
            *distData = CReflect( data ) ) } \
  /* mpi::Comm DistMatrix<T,U,V>::DistComm() const */ \
  ElError ElDistMatrixDistComm_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, MPI_Comm* comm ) \
  { EL_TRY( *comm = CReflect(A)->DistComm().comm ) } \
  /* mpi::Comm DistMatrix<T,U,V>::CrossComm() const */ \
  ElError ElDistMatrixCrossComm_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, MPI_Comm* comm ) \
  { EL_TRY( *comm = CReflect(A)->CrossComm().comm ) } \
  /* mpi::Comm DistMatrix<T,U,V>::RedundantComm() const */ \
  ElError ElDistMatrixRedundantComm_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, MPI_Comm* comm ) \
  { EL_TRY( *comm = CReflect(A)->RedundantComm().comm ) } \
  /* mpi::Comm DistMatrix<T,U,V>::ColComm() const */ \
  ElError ElDistMatrixColComm_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, MPI_Comm* comm ) \
  { EL_TRY( *comm = CReflect(A)->ColComm().comm ) } \
  /* mpi::Comm DistMatrix<T,U,V>::RowComm() const */ \
  ElError ElDistMatrixRowComm_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, MPI_Comm* comm ) \
  { EL_TRY( *comm = CReflect(A)->RowComm().comm ) } \
  /* mpi::Comm DistMatrix<T,U,V>::PartialColComm() const */ \
  ElError ElDistMatrixPartialColComm_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, MPI_Comm* comm ) \
  { EL_TRY( *comm = CReflect(A)->PartialColComm().comm ) } \
  /* mpi::Comm DistMatrix<T,U,V>::PartialRowComm() const */ \
  ElError ElDistMatrixPartialRowComm_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, MPI_Comm* comm ) \
  { EL_TRY( *comm = CReflect(A)->PartialRowComm().comm ) } \
  /* mpi::Comm DistMatrix<T,U,V>::PartialUnionColComm() const */ \
  ElError ElDistMatrixPartialUnionColComm_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, MPI_Comm* comm ) \
  { EL_TRY( *comm = CReflect(A)->PartialUnionColComm().comm ) } \
  /* mpi::Comm DistMatrix<T,U,V>::PartialUnionRowComm() const */ \
  ElError ElDistMatrixPartialUnionRowComm_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, MPI_Comm* comm ) \
  { EL_TRY( *comm = CReflect(A)->PartialUnionRowComm().comm ) } \
  /* Int DistMatrix<T,U,V>::ColStride() const */ \
  ElError ElDistMatrixColStride_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* stride ) \
  { EL_TRY( *stride = CReflect(A)->ColStride() ) } \
  /* Int DistMatrix<T,U,V>::RowStride() const */ \
  ElError ElDistMatrixRowStride_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* stride ) \
  { EL_TRY( *stride = CReflect(A)->RowStride() ) } \
  /* Int DistMatrix<T,U,V>::PartialColStride() const */ \
  ElError ElDistMatrixPartialColStride_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* stride ) \
  { EL_TRY( *stride = CReflect(A)->PartialColStride() ) } \
  /* Int DistMatrix<T,U,V>::PartialRowStride() const */ \
  ElError ElDistMatrixPartialRowStride_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* stride ) \
  { EL_TRY( *stride = CReflect(A)->PartialRowStride() ) } \
  /* Int DistMatrix<T,U,V>::PartialUnionColStride() const */ \
  ElError ElDistMatrixPartialUnionColStride_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* stride ) \
  { EL_TRY( *stride = CReflect(A)->PartialUnionColStride() ) } \
  /* Int DistMatrix<T,U,V>::PartialUnionRowStride() const */ \
  ElError ElDistMatrixPartialUnionRowStride_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* stride ) \
  { EL_TRY( *stride = CReflect(A)->PartialUnionRowStride() ) } \
  /* Int DistMatrix<T,U,V>::DistSize() const */ \
  ElError ElDistMatrixDistSize_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* commSize ) \
  { EL_TRY( *commSize = CReflect(A)->DistSize() ) } \
  /* Int DistMatrix<T,U,V>::CrossSize() const */ \
  ElError ElDistMatrixCrossSize_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* commSize ) \
  { EL_TRY( *commSize = CReflect(A)->CrossSize() ) } \
  /* Int DistMatrix<T,U,V>::RedundantSize() const */ \
  ElError ElDistMatrixRedundantSize_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt* commSize ) \
  { EL_TRY( *commSize = CReflect(A)->RedundantSize() ) }

#define DISTMATRIX_SINGLEENTRY(SIG,SIGBASE,T) \
  /* T DistMatrix<T,U,V>::Get( Int i, Int j ) const */ \
  ElError ElDistMatrixGet_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt i, ElInt j, CREFLECT(T)* val ) \
  { EL_TRY( *val = CReflect(CReflect(A)->Get(i,j)) ) } \
  /* void DistMatrix<T,U,V>::Set( Int i, Int j, T alpha ) */ \
  ElError ElDistMatrixSet_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt i, ElInt j, CREFLECT(T) alpha ) \
  { EL_TRY( CReflect(A)->Set(i,j,CReflect(alpha)) ) } \
  /* void DistMatrix<T,U,V>::Update( Int i, Int j, T alpha ) */ \
  ElError ElDistMatrixUpdate_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt i, ElInt j, CREFLECT(T) alpha ) \
  { EL_TRY( CReflect(A)->Update(i,j,CReflect(alpha)) ) } \
  /* T DistMatrix<T,U,V>::GetLocal( Int iLoc, Int jLoc ) const */ \
  ElError ElDistMatrixGetLocal_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt iLoc, ElInt jLoc, \
    CREFLECT(T)* val ) \
  { EL_TRY( *val = CReflect(CReflect(A)->GetLocal(iLoc,jLoc)) ) } \
  /* void DistMatrix<T,U,V>::SetLocal( Int iLoc, Int jLoc, T alpha ) */ \
  ElError ElDistMatrixSetLocal_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt iLoc, ElInt jLoc, CREFLECT(T) alpha ) \
  { EL_TRY( CReflect(A)->SetLocal(iLoc,jLoc,CReflect(alpha)) ) } \
  /* void DistMatrix<T,U,V>::UpdateLocal( Int iLoc, Int jLoc, T alpha ) */ \
  ElError ElDistMatrixUpdateLocal_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt iLoc, ElInt jLoc, CREFLECT(T) alpha ) \
  { EL_TRY( CReflect(A)->UpdateLocal \
            (iLoc,jLoc,CReflect(alpha)) ) }

#define DISTMATRIX_SINGLEENTRY_COMPLEX(SIG,SIGBASE,T) \
  /* Base<T> DistMatrix<T,U,V>::GetRealPart( Int i, Int j ) const */ \
  ElError ElDistMatrixGetRealPart_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt i, ElInt j, Base<T>* val ) \
  { EL_TRY( *val = CReflect(A)->GetRealPart(i,j) ) } \
  /* Base<T> DistMatrix<T,U,V>::GetImagPart( Int i, Int j ) const */ \
  ElError ElDistMatrixGetImagPart_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt i, ElInt j, Base<T>* val ) \
  { EL_TRY( *val = CReflect(A)->GetImagPart(i,j) ) } \
  /* void DistMatrix<T,U,V>::SetRealPart( Int i, Int j, Base<T> alpha ) */ \
  ElError ElDistMatrixSetRealPart_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt i, ElInt j, Base<T> alpha ) \
  { EL_TRY( CReflect(A)->SetRealPart(i,j,alpha) ) } \
  /* void DistMatrix<T,U,V>::SetImagPart( Int i, Int j, Base<T> alpha ) */ \
  ElError ElDistMatrixSetImagPart_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt i, ElInt j, Base<T> alpha ) \
  { EL_TRY( CReflect(A)->SetImagPart(i,j,alpha) ) } \
  /* void DistMatrix<T,U,V>::UpdateRealPart( Int i, Int j, Base<T> alpha ) */ \
  ElError ElDistMatrixUpdateRealPart_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt i, ElInt j, Base<T> alpha ) \
  { EL_TRY( CReflect(A)->UpdateRealPart(i,j,alpha) ) } \
  /* void DistMatrix<T,U,V>::UpdateImagPart( Int i, Int j, Base<T> alpha ) */ \
  ElError ElDistMatrixUpdateImagPart_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt i, ElInt j, Base<T> alpha ) \
  { EL_TRY( CReflect(A)->UpdateImagPart(i,j,alpha) ) } \
  /* void DistMatrix<T,U,V>::MakeReal( Int i, Int j ) */ \
  ElError ElDistMatrixMakeReal_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt i, ElInt j ) \
  { EL_TRY( CReflect(A)->MakeReal(i,j) ) } \
  /* void DistMatrix<T,U,V>::Conjugate( Int i, Int j ) */ \
  ElError ElDistMatrixConjugate_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt i, ElInt j ) \
  { EL_TRY( CReflect(A)->Conjugate(i,j) ) } \
  /* Base<T> DistMatrix<T,U,V>::GetLocalRealPart \
     ( Int iLoc, Int jLoc ) const */ \
  ElError ElDistMatrixGetLocalRealPart_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt iLoc, ElInt jLoc, Base<T>* val ) \
  { EL_TRY( *val = CReflect(A)->GetLocalRealPart(iLoc,jLoc) ) } \
  /* Base<T> DistMatrix<T,U,V>::GetLocalImagPart \
     ( Int iLoc, Int jLoc ) const */ \
  ElError ElDistMatrixGetLocalImagPart_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt iLoc, ElInt jLoc, Base<T>* val ) \
  { EL_TRY( *val = CReflect(A)->GetLocalImagPart(iLoc,jLoc) ) } \
  /* void DistMatrix<T,U,V>::SetLocalRealPart
     ( Int iLoc, Int jLoc, Base<T> alpha ) */ \
  ElError ElDistMatrixSetLocalRealPart_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt iLoc, ElInt jLoc, Base<T> alpha ) \
  { EL_TRY( CReflect(A)->SetLocalRealPart(iLoc,jLoc,alpha) ) } \
  /* void DistMatrix<T,U,V>::SetLocalImagPart
     ( Int iLoc, Int jLoc, Base<T> alpha ) */ \
  ElError ElDistMatrixSetLocalImagPart_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt iLoc, ElInt jLoc, Base<T> alpha ) \
  { EL_TRY( CReflect(A)->SetLocalImagPart(iLoc,jLoc,alpha) ) } \
  /* void DistMatrix<T,U,V>::UpdateLocalRealPart
     ( Int iLoc, Int jLoc, Base<T> alpha ) */ \
  ElError ElDistMatrixUpdateLocalRealPart_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt iLoc, ElInt jLoc, Base<T> alpha ) \
  { EL_TRY( CReflect(A)->UpdateLocalRealPart(iLoc,jLoc,alpha) ) } \
  /* void DistMatrix<T,U,V>::UpdateLocalImagPart
     ( Int iLoc, Int jLoc, Base<T> alpha ) */ \
  ElError ElDistMatrixUpdateLocalImagPart_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt iLoc, ElInt jLoc, Base<T> alpha ) \
  { EL_TRY( CReflect(A)->UpdateLocalImagPart(iLoc,jLoc,alpha) ) }

#define DISTMATRIX_DIAGONAL(SIG,SIGBASE,T) \
  /* bool DistMatrix<T,U,V>::DiagonalAlignedWith
     ( const DistData& data, Int offset ) const */ \
  ElError ElDistMatrixDiagonalAlignedWith_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElDistData distData, ElInt offset, \
    bool* aligned ) \
  { EL_TRY( *aligned = CReflect(A)->DiagonalAlignedWith \
                       (CReflect(distData),offset) ) } \
  /* Int DistMatrix<T,U,V>::DiagonalRoot( Int offset ) const */ \
  ElError ElDistMatrixDiagonalRoot_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt offset, ElInt* root ) \
  { EL_TRY( *root = CReflect(A)->DiagonalRoot(offset) ) } \
  /* Int DistMatrix<T,U,V>::DiagonalAlign( Int offset ) const */ \
  ElError ElDistMatrixDiagonalAlign_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt offset, ElInt* align ) \
  { EL_TRY( *align = CReflect(A)->DiagonalAlign(offset) ) } \
  /* DistMatrix<T,UDiag,VDiag> 
     DistMatrix<T,U,V>::GetDiagonal( Int offset ) const */ \
  ElError ElDistMatrixGetDiagonal_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt offset, \
    ElDistMatrix_ ## SIG *d ) \
  { auto AAbsPtr = CReflect(A); \
    AbstractDistMatrix<T>* dAbsPtr; \
    ElError error = ElDistMatrixGetDiagonal( AAbsPtr, offset, &dAbsPtr ); \
    *d = CReflect(dAbsPtr); \
    return error; }

#define DISTMATRIX_DIAGONAL_COMPLEX(SIG,SIGBASE,T) \
  /* void DistMatrix<T,U,V>::MakeDiagonalReal( Int offset ) */ \
  ElError ElDistMatrixMakeDiagonalReal_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt offset ) \
  { EL_TRY( CReflect(A)->MakeDiagonalReal(offset) ) } \
  /* void DistMatrix<T,U,V>::ConjugateDiagonal( Int offset ) */ \
  ElError ElDistMatrixConjugateDiagonal_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt offset ) \
  { EL_TRY( CReflect(A)->ConjugateDiagonal(offset) ) } \
  /* DistMatrix<Base<T>,UDiag,VDiag> 
     DistMatrix<T,U,V>::GetRealPartOfDiagonal( Int offset ) const */ \
  ElError ElDistMatrixGetRealPartOfDiagonal_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt offset, \
    ElDistMatrix_ ## SIGBASE *d ) \
  { auto AAbsPtr = CReflect(A); \
    AbstractDistMatrix<Base<T>>* dAbsPtr; \
    ElError error = \
      ElDistMatrixGetRealPartOfDiagonal( AAbsPtr, offset, &dAbsPtr ); \
    *d = CReflect(dAbsPtr); \
    return error; } \
  /* DistMatrix<Base<T>,UDiag,VDiag> 
     DistMatrix<T,U,V>::GetImagPartOfDiagonal( Int offset ) const */ \
  ElError ElDistMatrixGetImagPartOfDiagonal_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, ElInt offset, \
    ElDistMatrix_ ## SIGBASE *d ) \
  { auto AAbsPtr = CReflect(A); \
    AbstractDistMatrix<Base<T>>* dAbsPtr; \
    ElError error = \
      ElDistMatrixGetImagPartOfDiagonal( AAbsPtr, offset, &dAbsPtr ); \
    *d = CReflect(dAbsPtr); \
    return error; }

#define DISTMATRIX_SUBMATRIX(SIG,SIGBASE,T) \
  /* DistMatrix<T,STAR,STAR> DistMatrix<T,U,V>::GetSubmatrix
     ( const std::vector<Int>& rowInds, 
       const std::vector<Int>& colInds ) const */ \
  ElError ElDistMatrixGetSubmatrix_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, \
    ElInt numRowInds, const ElInt* rowInds, \
    ElInt numColInds, const ElInt* colInds, ElDistMatrix_ ## SIG *ASub ) \
  { try { auto APtr = CReflect(A); \
          auto ASubPtr = new DistMatrix<T,STAR,STAR>(APtr->Grid()); \
          std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds); \
          std::vector<Int> colIndVec(colInds,colInds+numColInds); \
          APtr->GetSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
          *ASub = CReflect(ASubPtr); } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::SetSubmatrix
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      const DistMatrix<T,STAR,STAR>& ASub ); */ \
  ElError ElDistMatrixSetSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, const ElInt* rowInds, const ElInt* colInds, \
    ElConstDistMatrix_ ## SIG ASub ) \
  { try { \
      auto APtr = CReflect(A); \
      auto ASubADMPtr = CReflect(ASub); \
      auto ASubPtr = dynamic_cast<const DistMatrix<T,STAR,STAR>*>(ASubADMPtr); \
      DynamicCastCheck(ASubPtr); \
      const Int numRowInds = ASubPtr->Height(); \
      const Int numColInds = ASubPtr->Width(); \
      std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds), \
                       colIndVec(colInds,colInds+numColInds); \
      APtr->SetSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::UpdateSubmatrix
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      T alpha, const DistMatrix<T,STAR,STAR>& ASub ); */ \
  ElError ElDistMatrixUpdateSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, const ElInt* rowInds, const ElInt* colInds, \
    CREFLECT(T) alpha, ElConstDistMatrix_ ## SIG ASub ) \
  { try { \
      auto APtr = CReflect(A); \
      auto ASubADMPtr = CReflect(ASub); \
      auto ASubPtr = dynamic_cast<const DistMatrix<T,STAR,STAR>*>(ASubADMPtr); \
      DynamicCastCheck(ASubPtr); \
      const Int numRowInds = ASubPtr->Height(); \
      const Int numColInds = ASubPtr->Width(); \
      std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds), \
                       colIndVec(colInds,colInds+numColInds); \
      APtr->UpdateSubmatrix \
      ( rowIndVec, colIndVec, CReflect(alpha), *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* Matrix<T> DistMatrix<T,U,V>::GetLocalSubmatrix
     ( const std::vector<Int>& rowIndsLoc, 
       const std::vector<Int>& colIndsLoc ) const */ \
  ElError ElDistMatrixGetLocalSubmatrix_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, \
    ElInt numRowInds, const ElInt* rowIndsLoc, \
    ElInt numColInds, const ElInt* colIndsLoc, ElMatrix_ ## SIG *ASub ) \
  { try { \
      auto APtr = CReflect(A); \
      auto ASubPtr = new Matrix<T>; \
      std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                       colIndVec(colIndsLoc,colIndsLoc+numColInds); \
      APtr->GetLocalSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
      *ASub = CReflect(ASubPtr); } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::SetLocalSubmatrix
     ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
       const Matrix<T>& ASub ); */ \
  ElError ElDistMatrixSetLocalSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    const ElInt* rowIndsLoc, const ElInt* colIndsLoc, \
    ElConstMatrix_ ## SIG ASub ) \
  { try { \
        auto APtr = CReflect(A); \
        auto ASubPtr = CReflect(ASub); \
        const Int numRowInds = ASubPtr->Height(); \
        const Int numColInds = ASubPtr->Width(); \
        std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                         colIndVec(colIndsLoc,colIndsLoc+numColInds); \
        APtr->SetLocalSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::UpdateLocalSubmatrix
     ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
       const Matrix<T>& ASub ); */ \
  ElError ElDistMatrixUpdateLocalSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    const ElInt* rowIndsLoc, const ElInt* colIndsLoc, \
    CREFLECT(T) alpha, ElConstMatrix_ ## SIG ASub ) \
  { try { \
        auto APtr = CReflect(A); \
        auto ASubPtr = CReflect(ASub); \
        const Int numRowInds = ASubPtr->Height(); \
        const Int numColInds = ASubPtr->Width(); \
        std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                         colIndVec(colIndsLoc,colIndsLoc+numColInds); \
        APtr->UpdateLocalSubmatrix \
        ( rowIndVec, colIndVec, CReflect(alpha), *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } 

#define DISTMATRIX_SUBMATRIX_COMPLEX(SIG,SIGBASE,T) \
  /* DistMatrix<Base<T>,STAR,STAR> DistMatrix<T,U,V>::GetRealPartOfSubmatrix
     ( const std::vector<Int>& rowInds, const std::vector<Int>& colInds ) const
  */ \
  ElError ElDistMatrixGetRealPartOfSubmatrix_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, \
  ElInt numRowInds, const ElInt* rowInds, \
  ElInt numColInds, const ElInt* colInds, \
  ElDistMatrix_ ## SIGBASE *ASub ) \
  { try { auto APtr = CReflect(A); \
          auto ASubPtr = new DistMatrix<Base<T>,STAR,STAR>(APtr->Grid()); \
          std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds); \
          std::vector<Int> colIndVec(colInds,colInds+numColInds); \
          APtr->GetRealPartOfSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
          *ASub = CReflect(ASubPtr); } EL_CATCH; return EL_SUCCESS; } \
  /* DistMatrix<Base<T>,STAR,STAR> DistMatrix<T,U,V>::GetImagPartOfSubmatrix
     ( const std::vector<Int>& rowInds, const std::vector<Int>& colInds ) const
  */ \
  ElError ElDistMatrixGetImagPartOfSubmatrix_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, \
  ElInt numRowInds, const ElInt* rowInds, \
  ElInt numColInds, const ElInt* colInds, \
  ElDistMatrix_ ## SIGBASE *ASub ) \
  { try { auto APtr = CReflect(A); \
          auto ASubPtr = new DistMatrix<Base<T>,STAR,STAR>(APtr->Grid()); \
          std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds); \
          std::vector<Int> colIndVec(colInds,colInds+numColInds); \
          APtr->GetImagPartOfSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
          *ASub = CReflect(ASubPtr); } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::SetRealPartOfSubmatrix
     ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
       const DistMatrix<Base<T>,STAR,STAR>& ASub ); */ \
  ElError ElDistMatrixSetRealPartOfSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, const ElInt* rowInds, const ElInt* colInds, \
    ElConstDistMatrix_ ## SIGBASE ASub ) \
  { try { \
      auto APtr = CReflect(A); \
      auto ASubADMPtr = CReflect(ASub); \
      auto ASubPtr = \
        dynamic_cast<const DistMatrix<Base<T>,STAR,STAR>*>(ASubADMPtr); \
      DynamicCastCheck(ASubPtr); \
      const Int numRowInds = ASubPtr->Height(); \
      const Int numColInds = ASubPtr->Width(); \
      std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds), \
                       colIndVec(colInds,colInds+numColInds); \
      APtr->SetRealPartOfSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::SetImagPartOfSubmatrix
     ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
       const DistMatrix<Base<T>,STAR,STAR>& ASub ); */ \
  ElError ElDistMatrixSetImagPartOfSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, const ElInt* rowInds, const ElInt* colInds, \
    ElConstDistMatrix_ ## SIGBASE ASub ) \
  { try { \
      auto APtr = CReflect(A); \
      auto ASubADMPtr = CReflect(ASub); \
      auto ASubPtr = \
        dynamic_cast<const DistMatrix<Base<T>,STAR,STAR>*>(ASubADMPtr); \
      DynamicCastCheck(ASubPtr); \
      const Int numRowInds = ASubPtr->Height(); \
      const Int numColInds = ASubPtr->Width(); \
      std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds), \
                       colIndVec(colInds,colInds+numColInds); \
      APtr->SetImagPartOfSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::UpdateRealPartOfSubmatrix
     ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
       Base<T> alpha, const DistMatrix<Base<T>,STAR,STAR>& ASub ); */ \
  ElError ElDistMatrixUpdateRealPartOfSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, const ElInt* rowInds, const ElInt* colInds, \
    Base<T> alpha, ElConstDistMatrix_ ## SIGBASE ASub ) \
  { try { \
      auto APtr = CReflect(A); \
      auto ASubADMPtr = CReflect(ASub); \
      auto ASubPtr = \
        dynamic_cast<const DistMatrix<Base<T>,STAR,STAR>*>(ASubADMPtr); \
      DynamicCastCheck(ASubPtr); \
      const Int numRowInds = ASubPtr->Height(); \
      const Int numColInds = ASubPtr->Width(); \
      std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds), \
                       colIndVec(colInds,colInds+numColInds); \
      APtr->UpdateRealPartOfSubmatrix \
      ( rowIndVec, colIndVec, alpha, *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::UpdateImagPartOfSubmatrix
     ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
       Base<T> alpha, const DistMatrix<Base<T>,STAR,STAR>& ASub ); */ \
  ElError ElDistMatrixUpdateImagPartOfSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, const ElInt* rowInds, const ElInt* colInds, \
    Base<T> alpha, ElConstDistMatrix_ ## SIGBASE ASub ) \
  { try { \
      auto APtr = CReflect(A); \
      auto ASubADMPtr = CReflect(ASub); \
      auto ASubPtr = \
        dynamic_cast<const DistMatrix<Base<T>,STAR,STAR>*>(ASubADMPtr); \
      DynamicCastCheck(ASubPtr); \
      const Int numRowInds = ASubPtr->Height(); \
      const Int numColInds = ASubPtr->Width(); \
      std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds), \
                       colIndVec(colInds,colInds+numColInds); \
      APtr->UpdateImagPartOfSubmatrix \
      ( rowIndVec, colIndVec, alpha, *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::MakeSubmatrixReal
     ( const std::vector<Int>& rowInds, const std::vector<Int>& colInds ) */ \
  ElError ElDistMatrixMakeSubmatrixReal_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt numRowInds, const ElInt* rowInds, \
                                  ElInt numColInds, const ElInt* colInds ) \
  { try { \
      std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds), \
                       colIndVec(colInds,colInds+numColInds); \
          CReflect(A)->MakeSubmatrixReal( rowIndVec, colIndVec ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::ConjugateSubmatrix
     ( const std::vector<Int>& rowInds, const std::vector<Int>& colInds ) */ \
  ElError ElDistMatrixConjugateSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, ElInt numRowInds, const ElInt* rowInds, \
                                  ElInt numColInds, const ElInt* colInds ) \
  { try { \
      std::vector<Int> rowIndVec(rowInds,rowInds+numRowInds), \
                       colIndVec(colInds,colInds+numColInds); \
      CReflect(A)->ConjugateSubmatrix( rowIndVec, colIndVec ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* Matrix<Base<T>> DistMatrix<T,U,V>::GetRealPartOfLocalSubmatrix
     ( const std::vector<Int>& rowInds, 
       const std::vector<Int>& colInds ) const */ \
  ElError ElDistMatrixGetRealPartOfLocalSubmatrix_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, \
    ElInt numRowInds, const ElInt* rowIndsLoc, \
    ElInt numColInds, const ElInt* colIndsLoc, \
    ElMatrix_ ## SIGBASE *ASub ) \
  { try { \
        auto APtr = CReflect(A); \
        auto ASubPtr = new Matrix<Base<T>>; \
        std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                         colIndVec(colIndsLoc,colIndsLoc+numColInds); \
        APtr->GetRealPartOfLocalSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
        *ASub = CReflect(ASubPtr); } EL_CATCH; return EL_SUCCESS; } \
  /* Matrix<Base<T>> DistMatrix<T,U,V>::GetImagPartOfLocalSubmatrix
     ( const std::vector<Int>& rowInds, 
       const std::vector<Int>& colInds ) const */ \
  ElError ElDistMatrixGetImagPartOfLocalSubmatrix_ ## SIG \
  ( ElConstDistMatrix_ ## SIG A, \
    ElInt numRowInds, const ElInt* rowIndsLoc, \
    ElInt numColInds, const ElInt* colIndsLoc, \
    ElMatrix_ ## SIGBASE *ASub ) \
  { try { \
        auto APtr = CReflect(A); \
        auto ASubPtr = new Matrix<Base<T>>; \
        std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                         colIndVec(colIndsLoc,colIndsLoc+numColInds); \
        APtr->GetImagPartOfLocalSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
        *ASub = CReflect(ASubPtr); } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::SetRealPartOfLocalSubmatrix
     ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
       const Matrix<Base<T>>& ASub ); */ \
  ElError ElDistMatrixSetRealPartOfLocalSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    const ElInt* rowIndsLoc, const ElInt* colIndsLoc, \
    ElConstMatrix_ ## SIGBASE ASub ) \
  { try { \
        auto APtr = CReflect(A); \
        auto ASubPtr = CReflect(ASub); \
        const Int numRowInds = ASubPtr->Height(); \
        const Int numColInds = ASubPtr->Width(); \
        std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                         colIndVec(colIndsLoc,colIndsLoc+numColInds); \
        APtr->SetRealPartOfLocalSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::SetImagPartOfLocalSubmatrix
     ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
       const Matrix<Base<T>>& ASub ); */ \
  ElError ElDistMatrixSetImagPartOfLocalSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    const ElInt* rowIndsLoc, const ElInt* colIndsLoc, \
    ElConstMatrix_ ## SIGBASE ASub ) \
  { try { \
        auto APtr = CReflect(A); \
        auto ASubPtr = CReflect(ASub); \
        const Int numRowInds = ASubPtr->Height(); \
        const Int numColInds = ASubPtr->Width(); \
        std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                         colIndVec(colIndsLoc,colIndsLoc+numColInds); \
        APtr->SetImagPartOfLocalSubmatrix( rowIndVec, colIndVec, *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::UpdateRealPartOfLocalSubmatrix
     ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
       Base<T> alpha, const Matrix<Base<T>>& ASub ); */ \
  ElError ElDistMatrixUpdateRealPartOfLocalSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    const ElInt* rowIndsLoc, const ElInt* colIndsLoc, \
    Base<T> alpha, ElConstMatrix_ ## SIGBASE ASub ) \
  { try { \
        auto APtr = CReflect(A); \
        auto ASubPtr = CReflect(ASub); \
        const Int numRowInds = ASubPtr->Height(); \
        const Int numColInds = ASubPtr->Width(); \
        std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                         colIndVec(colIndsLoc,colIndsLoc+numColInds); \
        APtr->UpdateRealPartOfLocalSubmatrix \
        ( rowIndVec, colIndVec, alpha, *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::UpdateImagPartOfLocalSubmatrix
     ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
       Base<T> alpha, const Matrix<Base<T>>& ASub ); */ \
  ElError ElDistMatrixUpdateImagPartOfLocalSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    const ElInt* rowIndsLoc, const ElInt* colIndsLoc, \
    Base<T> alpha, ElConstMatrix_ ## SIGBASE ASub ) \
  { try { \
        auto APtr = CReflect(A); \
        auto ASubPtr = CReflect(ASub); \
        const Int numRowInds = ASubPtr->Height(); \
        const Int numColInds = ASubPtr->Width(); \
        std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                         colIndVec(colIndsLoc,colIndsLoc+numColInds); \
        APtr->UpdateImagPartOfLocalSubmatrix \
        ( rowIndVec, colIndVec, alpha, *ASubPtr ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::MakeLocalSubmatrixReal
     ( const std::vector<Int>& rowIndsLoc, 
       const std::vector<Int>& colIndsLoc ) */ \
  ElError ElDistMatrixMakeLocalSubmatrixReal_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    ElInt numRowInds, const ElInt* rowIndsLoc, \
    ElInt numColInds, const ElInt* colIndsLoc ) \
  { try { \
        std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                         colIndVec(colIndsLoc,colIndsLoc+numColInds); \
        CReflect(A)->MakeLocalSubmatrixReal( rowIndVec, colIndVec ); \
    } EL_CATCH; return EL_SUCCESS; } \
  /* void DistMatrix<T,U,V>::ConjugateLocalSubmatrix
     ( const std::vector<Int>& rowIndsLoc, 
       const std::vector<Int>& colIndsLoc ) */ \
  ElError ElDistMatrixConjugateLocalSubmatrix_ ## SIG \
  ( ElDistMatrix_ ## SIG A, \
    ElInt numRowInds, const ElInt* rowIndsLoc, \
    ElInt numColInds, const ElInt* colIndsLoc ) \
  { try { \
        std::vector<Int> rowIndVec(rowIndsLoc,rowIndsLoc+numRowInds), \
                         colIndVec(colIndsLoc,colIndsLoc+numColInds); \
        CReflect(A)->ConjugateLocalSubmatrix( rowIndVec, colIndVec ); \
    } EL_CATCH; return EL_SUCCESS; }

#define C_PROTO(SIG,SIGBASE,T) \
  DISTMATRIX_CREATE(SIG,SIGBASE,T) \
  DISTMATRIX_RECONFIG(SIG,SIGBASE,T) \
  DISTMATRIX_BASIC(SIG,SIGBASE,T) \
  DISTMATRIX_SINGLEENTRY(SIG,SIGBASE,T) \
  DISTMATRIX_DIAGONAL(SIG,SIGBASE,T) \
  DISTMATRIX_SUBMATRIX(SIG,SIGBASE,T)

#define C_PROTO_COMPLEX(SIG,SIGBASE,T) \
  C_PROTO(SIG,SIGBASE,T) \
  DISTMATRIX_SINGLEENTRY_COMPLEX(SIG,SIGBASE,T) \
  DISTMATRIX_DIAGONAL_COMPLEX(SIG,SIGBASE,T) \
  DISTMATRIX_SUBMATRIX_COMPLEX(SIG,SIGBASE,T)

#include "El/macros/CInstantiate.h"

// TODO: More diagonal manipulation
// ================================

} // extern "C"
