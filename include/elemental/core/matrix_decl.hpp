/*
   Copyright (c) 2009-2012, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/

namespace elem {

// Matrix base for arbitrary rings
template<typename T,typename Int=int>
class Matrix
{
public:    
    //
    // Constructors
    // 

    Matrix(); 
    Matrix( Int height, Int width );
    Matrix( Int height, Int width, Int ldim );
    Matrix( Int height, Int width, const T* buffer, Int ldim );
    Matrix( Int height, Int width, T* buffer, Int ldim );
    Matrix( const Matrix<T,Int>& A );

    //
    // Destructor
    //

    ~Matrix();

    //
    // Basic information
    //

    Int Height() const;
    Int Width() const;
    Int DiagonalLength( Int offset=0 ) const;
    Int LDim() const;
    Int MemorySize() const;

    T* Buffer();
    T* Buffer( Int i, Int j );

    const T* LockedBuffer() const;
    const T* LockedBuffer( Int i, Int j ) const;

    //
    // I/O
    //

    void Print( const std::string msg="" ) const;
    void Print( std::ostream& os, const std::string msg="" ) const;

    //
    // Entry manipulation
    //

    T Get( Int i, Int j ) const;
    void Set( Int i, Int j, T alpha );
    void Update( Int i, Int j, T alpha );

    void GetDiagonal( Matrix<T,Int>& d, Int offset=0 ) const;
    void SetDiagonal( const Matrix<T,Int>& d, Int offset=0 );
    void UpdateDiagonal( const Matrix<T,Int>& d, Int offset=0 );

    //
    // Though the following routines are meant for complex data, all but four
    // logically apply to real data.
    //

    typename Base<T>::type GetRealPart( Int i, Int j ) const;
    typename Base<T>::type GetImagPart( Int i, Int j ) const;
    void SetRealPart( Int i, Int j, typename Base<T>::type alpha );
    // Only valid for complex data
    void SetImagPart( Int i, Int j, typename Base<T>::type alpha );
    void UpdateRealPart( Int i, Int j, typename Base<T>::type alpha );
    // Only valid for complex data
    void UpdateImagPart( Int i, Int j, typename Base<T>::type alpha );

    void GetRealPartOfDiagonal
    ( Matrix<typename Base<T>::type>& d, Int offset=0 ) const;
    void GetImagPartOfDiagonal
    ( Matrix<typename Base<T>::type>& d, Int offset=0 ) const;
    void SetRealPartOfDiagonal
    ( const Matrix<typename Base<T>::type>& d, Int offset=0 );
    // Only valid for complex data
    void SetImagPartOfDiagonal
    ( const Matrix<typename Base<T>::type>& d, Int offset=0 );
    void UpdateRealPartOfDiagonal
    ( const Matrix<typename Base<T>::type>& d, Int offset=0 );
    // Only valid for complex data
    void UpdateImagPartOfDiagonal
    ( const Matrix<typename Base<T>::type>& d, Int offset=0 );

    //
    // Viewing other matrix instances (or buffers)
    //

    bool Viewing() const;
    bool LockedView() const;

    void View( Int height, Int width, T* buffer, Int ldim );
    void View( Matrix<T,Int>& A);
    void View( Matrix<T,Int>& A, Int i, Int j, Int height, Int width );
    void View1x2( Matrix<T,Int>& AL, Matrix<T,Int>& AR );
    void View2x1( Matrix<T,Int>& AT, 
                  Matrix<T,Int>& AB );
    void View2x2( Matrix<T,Int>& ATL, Matrix<T,Int>& ATR,
                  Matrix<T,Int>& ABL, Matrix<T,Int>& ABR );

    void LockedView( Int height, Int width, const T* buffer, Int ldim );
    void LockedView( const Matrix<T,Int>& A );
    void LockedView
    ( const Matrix<T,Int>& A, Int i, Int j, Int height, Int width );
    void LockedView1x2
    ( const Matrix<T,Int>& AL, const Matrix<T,Int>& AR );
    void LockedView2x1
    ( const Matrix<T,Int>& AT, 
      const Matrix<T,Int>& AB );
    void LockedView2x2
    ( const Matrix<T,Int>& ATL, const Matrix<T,Int>& ATR,
      const Matrix<T,Int>& ABL, const Matrix<T,Int>& ABR );

    //
    // Utilities
    //

    const Matrix<T,Int>& operator=( const Matrix<T,Int>& A );

    void Empty();

    void ResizeTo( Int height, Int width );
    void ResizeTo( Int height, Int width, Int ldim );

private:
    bool viewing_, lockedView_;
    Int height_, width_, ldim_;
    T* data_;
    const T* lockedData_;
    Memory<T> memory_;

    void AssertValidEntry( Int i, Int j ) const;

    template<typename Z>
    struct SetRealPartHelper
    {
        static void Func( Matrix<Z,Int>& parent, Int i, Int j, Z alpha );
    };
    template<typename Z>
    struct SetRealPartHelper<Complex<Z> >
    {
        static void Func
        ( Matrix<Complex<Z>,Int>& parent, Int i, Int j, Z alpha );
    };
    template<typename Z> friend struct SetRealPartHelper;

    template<typename Z>
    struct SetImagPartHelper
    {
        static void Func( Matrix<Z,Int>& parent, Int i, Int j, Z alpha );
    };
    template<typename Z>
    struct SetImagPartHelper<Complex<Z> >
    {
        static void Func
        ( Matrix<Complex<Z>,Int>& parent, Int i, Int j, Z alpha );
    };
    template<typename Z> friend struct SetImagPartHelper;

    template<typename Z>
    struct UpdateRealPartHelper
    {
        static void Func( Matrix<Z,Int>& parent, Int i, Int j, Z alpha );
    };
    template<typename Z>
    struct UpdateRealPartHelper<Complex<Z> >
    {
        static void Func
        ( Matrix<Complex<Z>,Int>& parent, Int i, Int j, Z alpha );
    };
    template<typename Z> friend struct UpdateRealPartHelper;

    template<typename Z>
    struct UpdateImagPartHelper
    {
        static void Func( Matrix<Z,Int>& parent, Int i, Int j, Z alpha );
    };
    template<typename Z>
    struct UpdateImagPartHelper<Complex<Z> >
    {
        static void Func
        ( Matrix<Complex<Z>,Int>& parent, Int i, Int j, Z alpha );
    };
    template<typename Z> friend struct UpdateImagPartHelper;
};

} // namespace elem
