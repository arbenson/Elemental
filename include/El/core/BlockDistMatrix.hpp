/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_BLOCKDISTMATRIX_HPP
#define EL_BLOCKDISTMATRIX_HPP

namespace El {

struct BlockDistData
{
    Dist colDist, rowDist;
    Int blockHeight, blockWidth;
    Int colAlign, rowAlign; 
    Int colCut, rowCut;
    Int root;  // relevant for [o ,o ]/[MD,* ]/[* ,MD]
    const Grid* grid;

    BlockDistData() { }

    template<typename T,Dist U,Dist V>
    BlockDistData( const GeneralBlockDistMatrix<T,U,V>& A )
    : colDist(U), rowDist(V), 
      blockHeight(A.BlockHeight()), blockWidth(A.BlockWidth()),
      colAlign(A.ColAlign()), rowAlign(A.RowAlign()),
      colCut(A.ColCut()), rowCut(A.RowCut()),
      root(A.Root()), grid(&A.Grid())
    { }
};
inline bool operator==( const BlockDistData& A, const BlockDistData& B )
{ return A.colDist     == B.colDist &&
         A.rowDist     == B.rowDist &&
         A.blockHeight == B.blockHeight &&
         A.blockWidth  == B.blockWidth &&
         A.colAlign    == B.colAlign &&
         A.rowAlign    == B.rowAlign &&
         A.root        == B.root &&
         A.grid        == B.grid; }


} // namespace El

#include "./BlockDistMatrix/Abstract.hpp"
#include "./BlockDistMatrix/General.hpp"
#include "./BlockDistMatrix/CIRC_CIRC.hpp"
#include "./BlockDistMatrix/MC_MR.hpp"
#include "./BlockDistMatrix/MC_STAR.hpp"
#include "./BlockDistMatrix/MD_STAR.hpp"
#include "./BlockDistMatrix/MR_MC.hpp"
#include "./BlockDistMatrix/MR_STAR.hpp"
#include "./BlockDistMatrix/STAR_MC.hpp"
#include "./BlockDistMatrix/STAR_MD.hpp"
#include "./BlockDistMatrix/STAR_MR.hpp"
#include "./BlockDistMatrix/STAR_STAR.hpp"
#include "./BlockDistMatrix/STAR_VC.hpp"
#include "./BlockDistMatrix/STAR_VR.hpp"
#include "./BlockDistMatrix/VC_STAR.hpp"
#include "./BlockDistMatrix/VR_STAR.hpp"

namespace El {

#ifdef EL_HAVE_SCALAPACK
template<typename T>
inline typename blacs::Desc
FillDesc( const BlockDistMatrix<T>& A, int context )
{
    if( A.ColCut() != 0 || A.RowCut() != 0 )
        LogicError("Cannot produce a meaningful descriptor if nonzero cut");
    typename blacs::Desc desc = 
        { 1, context, A.Height(), A.Width(), A.BlockHeight(), A.BlockWidth(), 
          A.ColAlign(), A.RowAlign(), A.LDim() };
    return desc;
}
#endif

} // namespace El

#endif // ifndef EL_BLOCKDISTMATRIX_HPP
