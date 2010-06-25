/*
   This file is part of elemental, a library for distributed-memory dense
   linear algebra.

   Copyright (C) 2009-2010 Jack Poulson <jack.poulson@gmail.com>

   This program is released under the terms of the license contained in the
   file LICENSE.
*/
#include "elemental/lapack_internal.hpp"
using namespace std;
using namespace elemental;

template<typename T>
void
elemental::lapack::Hegst
( bool bothOnLeft, Shape shape, 
  DistMatrix<T,MC,MR>& A, const DistMatrix<T,MC,MR>& B )
{
#ifndef RELEASE
    PushCallStack("lapack::Hegst");
#endif
    if( bothOnLeft )
    {
        if( shape == Lower )
            lapack::internal::HegstTrueL( A, B );
        else
            lapack::internal::HegstTrueU( A, B );
    }
    else
    {
        if( shape == Lower )
            lapack::internal::HegstFalseL( A, B );
        else
            lapack::internal::HegstFalseU( A, B );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T>
void
elemental::lapack::internal::HegstNaive
( bool bothOnLeft, Shape shape, 
  DistMatrix<T,MC,MR>& A, const DistMatrix<T,MC,MR>& B )
{
#ifndef RELEASE
    PushCallStack("lapack::internal::HegstNaive");
#endif
    if( bothOnLeft )
    {
        if( shape == Lower )
            lapack::internal::HegstTrueLNaive( A, B );
        else
            lapack::internal::HegstTrueUNaive( A, B );
    }
    else
    {
        if( shape == Lower )
            lapack::internal::HegstFalseLNaive( A, B );
        else
            lapack::internal::HegstFalseUNaive( A, B );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}


template void elemental::lapack::Hegst
( bool bothOnLeft, Shape shape, 
  DistMatrix<float,MC,MR>& A, const DistMatrix<float,MC,MR>& B );

template void elemental::lapack::internal::HegstNaive
( bool bothOnLeft, Shape shape, 
  DistMatrix<float,MC,MR>& A, const DistMatrix<float,MC,MR>& B );

template void elemental::lapack::Hegst
( bool bothOnLeft, Shape shape,
  DistMatrix<double,MC,MR>& A, const DistMatrix<double,MC,MR>& B );

template void elemental::lapack::internal::HegstNaive
( bool bothOnLeft, Shape shape,
  DistMatrix<double,MC,MR>& A, const DistMatrix<double,MC,MR>& B );

#ifndef WITHOUT_COMPLEX
template void elemental::lapack::Hegst
( bool bothOnLeft, Shape shape,
  DistMatrix<scomplex,MC,MR>& A, const DistMatrix<scomplex,MC,MR>& B );

template void elemental::lapack::internal::HegstNaive
( bool bothOnLeft, Shape shape,
  DistMatrix<scomplex,MC,MR>& A, const DistMatrix<scomplex,MC,MR>& B );

template void elemental::lapack::Hegst
( bool bothOnLeft, Shape shape,
  DistMatrix<dcomplex,MC,MR>& A, const DistMatrix<dcomplex,MC,MR>& B );


template void elemental::lapack::internal::HegstNaive
( bool bothOnLeft, Shape shape,
  DistMatrix<dcomplex,MC,MR>& A, const DistMatrix<dcomplex,MC,MR>& B );
#endif
