/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/

// This file should be included into each of the DistMatrix specializations
// as a workaround for the fact that C++11 constructor inheritance is not 
// yet widely supported.

namespace El {

#define DM DistMatrix<T,ColDist,RowDist>
#define GDM GeneralDistMatrix<T,ColDist,RowDist>

// Public section
// ##############

// Constructors and destructors
// ============================

template<typename T>
DM::DistMatrix( const El::Grid& grid, Int root )
: GDM(grid,root)
{ 
    if( ColDist == CIRC && RowDist == CIRC )
        this->matrix_.viewType_ = OWNER;
    this->SetShifts(); 
}

template<typename T>
DM::DistMatrix( Int height, Int width, const El::Grid& grid, Int root )
: GDM(grid,root)
{ 
    if( ColDist == CIRC && RowDist == CIRC )
        this->matrix_.viewType_ = OWNER;
    this->SetShifts(); 
    this->Resize(height,width); 
}

template<typename T>
DM::DistMatrix( const DM& A )
: GDM(A.Grid())
{
    DEBUG_ONLY(CallStackEntry cse("DistMatrix::DistMatrix"))
    if( ColDist == CIRC && RowDist == CIRC )
        this->matrix_.viewType_ = OWNER;
    this->SetShifts();
    if( &A != this )
        *this = A;
    else
        LogicError("Tried to construct DistMatrix with itself");
}

template<typename T>
template<Dist U,Dist V>
DM::DistMatrix( const DistMatrix<T,U,V>& A )
: GDM(A.Grid())
{
    DEBUG_ONLY(CallStackEntry cse("DistMatrix::DistMatrix"))
    if( ColDist == CIRC && RowDist == CIRC )
        this->matrix_.viewType_ = OWNER;
    this->SetShifts();
    if( ColDist != U || RowDist != V ||
        reinterpret_cast<const DM*>(&A) != this )
        *this = A;
    else
        LogicError("Tried to construct DistMatrix with itself");
}

template<typename T>
DM::DistMatrix( const AbstractDistMatrix<T>& A )
{
    DEBUG_ONLY(CallStackEntry cse("DM(ADM)"))
    if( ColDist == CIRC && RowDist == CIRC )
        this->matrix_.viewType_ = OWNER;
    this->SetShifts();
    #define GUARD(CDIST,RDIST) \
      A.DistData().colDist == CDIST && A.DistData().rowDist == RDIST
    #define PAYLOAD(CDIST,RDIST) \
      auto& ACast = dynamic_cast<const DistMatrix<T,CDIST,RDIST>&>(A); \
      if( ColDist != CDIST || RowDist != RDIST || \
          reinterpret_cast<const DM*>(&A) != this ) \
          *this = ACast; \
      else \
          LogicError("Tried to construct DistMatrix with itself");
    #include "El/macros/GuardAndPayload.h"
}

template<typename T>
template<Dist U,Dist V>
DM::DistMatrix( const BlockDistMatrix<T,U,V>& A )
: GDM(A.Grid())
{
    DEBUG_ONLY(CallStackEntry cse("DistMatrix::DistMatrix"))
    if( ColDist == CIRC && RowDist == CIRC )
        this->matrix_.viewType_ = OWNER;
    this->SetShifts();
    *this = A;
}

template<typename T>
DM::DistMatrix( DM&& A ) EL_NOEXCEPT : GDM(std::move(A)) { }

template<typename T> DM::~DistMatrix() { }

template<typename T> 
DistMatrix<T,ColDist,RowDist>* DM::Construct
( const El::Grid& g, Int root ) const
{ return new DistMatrix<T,ColDist,RowDist>(g,root); }

template<typename T> 
DistMatrix<T,RowDist,ColDist>* DM::ConstructTranspose
( const El::Grid& g, Int root ) const
{ return new DistMatrix<T,RowDist,ColDist>(g,root); }

template<typename T>
DistMatrix<T,DiagColDist<ColDist,RowDist>(),
             DiagRowDist<ColDist,RowDist>()>* DM::ConstructDiagonal
( const El::Grid& g, Int root ) const
{ return new DistMatrix<T,this->UDiag,this->VDiag>(g,root); }

template<typename T>
template<Dist U,Dist V>
DM& DM::operator=( const BlockDistMatrix<T,U,V>& A )
{
    DEBUG_ONLY(CallStackEntry cse("DM = BDM[U,V]"))
    BlockDistMatrix<T,ColDist,RowDist> AElem(A.Grid(),1,1);
    AElem = A;
    DistMatrix<T,ColDist,RowDist> AElemView(A.Grid());
    LockedView( AElemView, AElem ); 
    *this = AElemView;
    return *this;
}

template<typename T>
DM& DM::operator=( const AbstractDistMatrix<T>& A )
{
    DEBUG_ONLY(CallStackEntry cse("DM = ADM"))
    #define GUARD(CDIST,RDIST) \
      A.DistData().colDist == CDIST && A.DistData().rowDist == RDIST
    #define PAYLOAD(CDIST,RDIST) \
      auto& ACast = dynamic_cast<const DistMatrix<T,CDIST,RDIST>&>(A); \
      *this = ACast;
    #include "El/macros/GuardAndPayload.h"
    return *this;
}

template<typename T>
DM& DM::operator=( DM&& A )
{
    if( this->Viewing() && !A.Viewing() )
        this->operator=( (const DM&)A );
    else
        GDM::operator=( std::move(A) );
    return *this;
}

template<typename T>
El::DistData DM::DistData() const { return El::DistData(*this); }

} // namespace El
