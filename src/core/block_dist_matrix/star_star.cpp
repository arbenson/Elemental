/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "elemental-lite.hpp"

#define ColDist STAR
#define RowDist STAR

#include "./setup.hpp"

namespace elem {

// Public section
// ##############

// Assignment and reconfiguration
// ==============================

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,MC,MR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [MC,MR]"))
    A.AllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,MC,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [MC,STAR]"))
    A.ColAllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,MR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [STAR,MR]"))
    A.RowAllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,MD,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [MD,STAR]"))
    A.ColAllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,MD>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [STAR,MD]"))
    A.RowAllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,MR,MC>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [MR,MC]"))
    A.AllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,MR,STAR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [MR,STAR]"))
    A.ColAllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,MC>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [STAR,MC]"))
    A.RowAllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,VC,STAR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [VC,STAR]"))
    A.ColAllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,VC>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [STAR,VC]"))
    A.RowAllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,VR,STAR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [VR,STAR]"))
    A.ColAllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,STAR,VR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [STAR,VR]"))
    A.RowAllGather( *this );
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BDM& A )
{
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [STAR,STAR]"))
    this->Resize( A.Height(), A.Width() ); 
    if( this->Grid() == A.Grid() )
    {
        this->matrix_ = A.LockedMatrix();
    }
    else
    {
        LogicError("This option is not yet written");
    }
    return *this;
}

template<typename T>
BDM&
BDM::operator=( const BlockDistMatrix<T,CIRC,CIRC>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[STAR,STAR] = [CIRC,CIRC]"))
    LogicError("This routine is not yet written");
    return *this;
}

// Basic queries
// =============

template<typename T>
mpi::Comm BDM::DistComm() const { return mpi::COMM_SELF; }
template<typename T>
mpi::Comm BDM::CrossComm() const { return mpi::COMM_SELF; }
template<typename T>
mpi::Comm BDM::RedundantComm() const { return this->grid_->VCComm(); }
template<typename T>
mpi::Comm BDM::ColComm() const { return mpi::COMM_SELF; }
template<typename T>
mpi::Comm BDM::RowComm() const { return mpi::COMM_SELF; }

template<typename T>
Int BDM::ColStride() const { return 1; }
template<typename T>
Int BDM::RowStride() const { return 1; }

// Instantiate {Int,Real,Complex<Real>} for each Real in {float,double}
// ####################################################################

#define PROTO(T) template class BlockDistMatrix<T,ColDist,RowDist>
#define COPY(T,U,V) \
  template BlockDistMatrix<T,ColDist,RowDist>::BlockDistMatrix\
  ( const BlockDistMatrix<T,U,V>& A );\
  template BlockDistMatrix<T,ColDist,RowDist>::BlockDistMatrix\
  ( const DistMatrix<T,U,V>& A );
#define FULL(T) \
  PROTO(T); \
  COPY(T,CIRC,CIRC); \
  COPY(T,MC,  MR  ); \
  COPY(T,MC,  STAR); \
  COPY(T,MD,  STAR); \
  COPY(T,MR,  MC  ); \
  COPY(T,MR,  STAR); \
  COPY(T,STAR,MC  ); \
  COPY(T,STAR,MD  ); \
  COPY(T,STAR,MR  ); \
  COPY(T,STAR,VC  ); \
  COPY(T,STAR,VR  ); \
  COPY(T,VC,  STAR); \
  COPY(T,VR,  STAR);

FULL(Int);
#ifndef DISABLE_FLOAT
FULL(float);
#endif
FULL(double);

#ifndef DISABLE_COMPLEX
#ifndef DISABLE_FLOAT
FULL(Complex<float>);
#endif
FULL(Complex<double>);
#endif 

} // namespace elem
