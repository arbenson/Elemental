/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_DISTMATRIX_MD_STAR_DECL_HPP
#define EL_DISTMATRIX_MD_STAR_DECL_HPP

namespace El {

// Partial specialization to A[MD,* ].
// 
// The columns of these distributed matrices will be distributed like 
// "Matrix Diagonals" (MD). It is important to recognize that the diagonal
// of a sufficiently large distributed matrix is distributed amongst the 
// entire process grid if and only if the dimensions of the process grid
// are coprime.
template<typename T>
class DistMatrix<T,MD,STAR> : public GeneralDistMatrix<T,MD,STAR>
{
public:
    // Typedefs
    // ========
    typedef AbstractDistMatrix<T> absType;
    typedef GeneralDistMatrix<T,MD,STAR> genType;
    typedef DistMatrix<T,MD,STAR> type;

    // Constructors and destructors
    // ============================

    // Inherited constructors are part of C++11 but not yet widely supported.
    //using GeneralDistMatrix<T,MD,STAR>::GeneralDistMatrix;

    // Create a 0 x 0 distributed matrix
    DistMatrix( const El::Grid& g=DefaultGrid(), Int root=0 );
    // Create a height x width distributed matrix
    DistMatrix
    ( Int height, Int width, const El::Grid& g=DefaultGrid(), Int root=0 );
    // Create a copy of distributed matrix A
    DistMatrix( const type& A );
    DistMatrix( const absType& A );
    template<Dist U,Dist V> DistMatrix( const DistMatrix<T,U,V>& A );
    template<Dist U,Dist V> DistMatrix( const BlockDistMatrix<T,U,V>& A );
    // Move constructor
    DistMatrix( type&& A ) EL_NOEXCEPT;
    // Destructor
    ~DistMatrix();

    DistMatrix<T,MD,STAR>* Construct
    ( const El::Grid& g=DefaultGrid(), Int root=0 ) const override;
    DistMatrix<T,STAR,MD>* ConstructTranspose
    ( const El::Grid& g=DefaultGrid(), Int root=0 ) const override;
    DistMatrix<T,MD,STAR>* ConstructDiagonal
    ( const El::Grid& g=DefaultGrid(), Int root=0 ) const override;

    // Assignment and reconfiguration
    // ==============================
    template<Dist U,Dist V> type& operator=( const BlockDistMatrix<T,U,V>& A );
    type& operator=( const absType& A );
    type& operator=( const DistMatrix<T,MC,  MR  >& A );
    type& operator=( const DistMatrix<T,MC,  STAR>& A );
    type& operator=( const DistMatrix<T,STAR,MR  >& A );
    type& operator=( const DistMatrix<T,MD,  STAR>& A );
    type& operator=( const DistMatrix<T,STAR,MD  >& A );
    type& operator=( const DistMatrix<T,MR,  MC  >& A );
    type& operator=( const DistMatrix<T,MR,  STAR>& A );
    type& operator=( const DistMatrix<T,STAR,MC  >& A );
    type& operator=( const DistMatrix<T,VC,  STAR>& A );
    type& operator=( const DistMatrix<T,STAR,VC  >& A );
    type& operator=( const DistMatrix<T,VR,  STAR>& A );
    type& operator=( const DistMatrix<T,STAR,VR  >& A );
    type& operator=( const DistMatrix<T,STAR,STAR>& A );
    type& operator=( const DistMatrix<T,CIRC,CIRC>& A );
    // Move assignment
    type& operator=( type&& A );

    // Basic queries
    // =============
    El::DistData DistData() const override;
    mpi::Comm DistComm() const override;
    mpi::Comm CrossComm() const override;
    mpi::Comm RedundantComm() const override;
    mpi::Comm ColComm() const override;
    mpi::Comm RowComm() const override;
    Int RowStride() const override;
    Int ColStride() const override;
    Int DistSize() const override;
    Int CrossSize() const override;
    Int RedundantSize() const override;

private:
    // Friend declarations
    // ===================
    template<typename S,Dist U,Dist V> friend class DistMatrix;
    template<typename S,Dist U,Dist V> friend class BlockDistMatrix;
};

} // namespace El

#endif // ifndef EL_DISTMATRIX_MD_STAR_DECL_HPP
