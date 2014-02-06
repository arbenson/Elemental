/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_MATRIX_HPP
#define ELEM_MATRIX_HPP

namespace elem {

// Matrix base for arbitrary rings
template<typename T>
class Matrix
{
public:    
    // Constructors and destructors
    // ============================
    // Create a 0x0 matrix
    Matrix( bool fixed=false );
    // Create a matrix with the specified dimensions
    Matrix( Int height, Int width, bool fixed=false );
    // Create a matrix with the specified dimensions and leading dimension
    Matrix( Int height, Int width, Int ldim, bool fixed=false );
    // Construct a matrix around an existing (possibly immutable) buffer
    Matrix
    ( Int height, Int width, const T* buffer, Int ldim, bool fixed=false );
    Matrix( Int height, Int width, T* buffer, Int ldim, bool fixed=false );
    // Create a copy of a matrix
    Matrix( const Matrix<T>& A );
#ifndef SWIG
    // Move the metadata from a given matrix
    Matrix( Matrix<T>&& A );
#endif 
    // Destructor
    ~Matrix();

    // Assignment and reconfiguration
    // ==============================
    const Matrix<T>& operator=( const Matrix<T>& A );
#ifndef SWIG
    // Move the metadata from the given matrix
    Matrix<T>& operator=( Matrix<T>&& A );
#endif
    void Empty();
    void Resize( Int height, Int width );
    void Resize( Int height, Int width, Int ldim );
    // Reconfigure around the given buffer, but do not assume ownership
    void Attach( Int height, Int width, T* buffer, Int ldim );
    void LockedAttach( Int height, Int width, const T* buffer, Int ldim );
    // Reconfigure around the given buffer and assume ownership
    void Control( Int height, Int width, T* buffer, Int ldim );

    // Basic queries
    // =============
    Int Height() const;
    Int Width() const;
    Int LDim() const;
    Int MemorySize() const;
    Int DiagonalLength( Int offset=0 ) const;
    T* Buffer();
    T* Buffer( Int i, Int j );
    const T* LockedBuffer() const;
    const T* LockedBuffer( Int i, Int j ) const;
    bool Viewing()   const;
    bool FixedSize() const;
    bool Locked()    const;

    // Single-entry manipulation
    // =========================
    T Get( Int i, Int j ) const;
    BASE(T) GetRealPart( Int i, Int j ) const;
    BASE(T) GetImagPart( Int i, Int j ) const;
    void Set( Int i, Int j, T alpha );
    void SetRealPart( Int i, Int j, BASE(T) alpha );
    void SetImagPart( Int i, Int j, BASE(T) alpha );
    void Update( Int i, Int j, T alpha );
    void UpdateRealPart( Int i, Int j, BASE(T) alpha );
    void UpdateImagPart( Int i, Int j, BASE(T) alpha );
    void MakeReal( Int i, Int j );
    void Conjugate( Int i, Int j );

    // Diagonal manipulation
    // =====================
    void GetDiagonal( Matrix<T>& d, Int offset=0 ) const;
    void GetRealPartOfDiagonal( Matrix<BASE(T) >& d, Int offset=0 ) const;
    void GetImagPartOfDiagonal( Matrix<BASE(T) >& d, Int offset=0 ) const;
    Matrix<T> GetDiagonal( Int offset=0 ) const;
    Matrix<BASE(T) > GetRealPartOfDiagonal( Int offset=0 ) const;
    Matrix<BASE(T) > GetImagPartOfDiagonal( Int offset=0 ) const;
    void SetDiagonal( const Matrix<T>& d, Int offset=0 );
    void SetRealPartOfDiagonal( const Matrix<BASE(T) >& d, Int offset=0 );
    void SetImagPartOfDiagonal( const Matrix<BASE(T) >& d, Int offset=0 );
    void UpdateDiagonal( const Matrix<T>& d, Int offset=0 );
    void UpdateRealPartOfDiagonal( const Matrix<BASE(T) >& d, Int offset=0 );
    void UpdateImagPartOfDiagonal( const Matrix<BASE(T) >& d, Int offset=0 );

    // Arbitrary submatrix manipulation
    // ================================
    void Get
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      Matrix<T>& ASub ) const;
    void GetRealPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      Matrix<BASE(T)>& ASub ) const;
    void GetImagPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      Matrix<BASE(T)>& ASub ) const;
    Matrix<T> Get
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd ) const;
    Matrix<BASE(T)> GetRealPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd ) const;
    Matrix<BASE(T)> GetImagPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd ) const;

    void Set
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      const Matrix<T>& ASub );
    void SetRealPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      const Matrix<BASE(T)>& ASub );
    void SetImagPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      const Matrix<BASE(T)>& ASub );

    void Update
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      T alpha, const Matrix<T>& ASub );
    void UpdateRealPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      BASE(T) alpha, const Matrix<BASE(T)>& ASub );
    void UpdateImagPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      BASE(T) alpha, const Matrix<BASE(T)>& ASub );

    void MakeReal
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd );
    void Conjugate
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd );

private:
    // Member variables
    // ================
    ViewType viewType_;
    Int height_, width_, ldim_;
    const T* data_;
    Memory<T> memory_;

    // Exchange metadata with another matrix
    // =====================================
    void ShallowSwap( Matrix<T>& A );

    // Reconfigure without error-checking
    // ==================================
    void Empty_();
    void Resize_( Int height, Int width );
    void Resize_( Int height, Int width, Int ldim );
    void Control_( Int height, Int width, T* buffer, Int ldim );
    void Attach_( Int height, Int width, T* buffer, Int ldim );
    void LockedAttach_( Int height, Int width, const T* buffer, Int ldim );

    // Return a reference to a single entry without error-checking
    // ===========================================================
    const T& Get_( Int i, Int j ) const;
    T& Set_( Int i, Int j );

    // Assertions
    // ==========
    void ComplainIfReal() const;
    void AssertValidDimensions( Int height, Int width ) const;
    void AssertValidDimensions( Int height, Int width, Int ldim ) const;
    void AssertValidEntry( Int i, Int j ) const;
   
    // Friend declarations
    // ===================
#ifndef SWIG
    template <typename F> 
    friend class Matrix;
    template <typename F,Dist U,Dist V> 
    friend class DistMatrix;
    friend class AbstractDistMatrix<T>;

    friend void View<T>( Matrix<T>& A, Matrix<T>& B );
    friend void View<T>
    ( Matrix<T>& A, Matrix<T>& B, Int i, Int j, Int height, Int width );
    friend void View1x2<T>( Matrix<T>& A, Matrix<T>& BL, Matrix<T>& BR );
    friend void View2x1<T>( Matrix<T>& A, Matrix<T>& BT, Matrix<T>& BB );
    friend void View2x2<T>
    ( Matrix<T>& A, Matrix<T>& BTL, Matrix<T>& BTR,
                    Matrix<T>& BBL, Matrix<T>& BBR );

    friend void LockedView<T>( Matrix<T>& A, const Matrix<T>& B );
    friend void LockedView<T>
    ( Matrix<T>& A, const Matrix<T>& B, Int i, Int j, Int height, Int width );
    friend void LockedView1x2<T>
    ( Matrix<T>& A, const Matrix<T>& BL, const Matrix<T>& BR );
    friend void LockedView2x1<T>
    ( Matrix<T>& A, const Matrix<T>& BT, const Matrix<T>& BB );
    friend void LockedView2x2<T>
    ( Matrix<T>& A, const Matrix<T>& BTL, const Matrix<T>& BTR,
                    const Matrix<T>& BBL, const Matrix<T>& BBR );
#endif // ifndef SWIG
};

} // namespace elem

#endif // ifndef ELEM_MATRIX_HPP
