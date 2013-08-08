/*******************************************************************************
 * Copyright (c) 2013, Art Clarke.  All rights reserved.
 *  
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include "Rational.h"
#include "Global.h"
#include "RationalImpl.h"

namespace io { namespace humble { namespace video
  {

    Rational::Rational()
    {
    }

    Rational::~Rational()
    {
    }

    int32_t
    Rational :: sCompareTo(Rational *a, Rational *b)
    {
      return a->compareTo(b);
    }

    int32_t
    Rational :: sReduce(Rational *dst, int64_t num,
        int64_t den, int64_t max)
    {
      return (dst ? dst->reduce(num, den, max) : 0);
    }

    Rational*
    Rational :: sDivide(Rational *a, Rational* b)
    {
      return a->divide(b);
    }

    Rational*
    Rational :: sSubtract(Rational *a, Rational* b)
    {
      return a->subtract(b);
    }

    Rational*
    Rational :: sAdd(Rational *a, Rational* b)
    {
      return a->add(b);
    }

    Rational*
    Rational :: sMultiply(Rational *a, Rational* b)
    {
      return a->multiply(b);
    }

    int64_t
    Rational :: sRescale(int64_t origValue, Rational* origBase, Rational* newBase)
    {
      return newBase->rescale(origValue, origBase);
    }

    Rational*
    Rational :: make()
    {
      return RationalImpl::make();
    }
    
    Rational*
    Rational :: make(double d)
    {
      return RationalImpl::make(d);
    }
    
    Rational*
    Rational :: make(Rational *aSrc)
    {
      RationalImpl* src = dynamic_cast<RationalImpl*>(aSrc);
      Rational* retval = 0;
      if (src)
      {
        retval = RationalImpl::make(src);
      }
      return retval;
    }
    
    Rational*
    Rational :: make(int32_t num, int32_t den)
    {
      return RationalImpl::make(num, den);
    }
    
    int64_t
    Rational :: sRescale(int64_t origValue,
        Rational* origBase, Rational* newBase,
        Rounding rounding)
    {
      if (!origBase || !newBase)
        return origValue;
      return newBase->rescale(origValue, origBase, rounding);
    }

    int64_t
    Rational :: rescale(int64_t srcValue,
        int32_t dstNumerator,
        int32_t dstDenominator,
        int32_t srcNumerator,
        int32_t srcDenominator,
        Rounding rounding)
    {
      return RationalImpl::rescale(srcValue,
          dstNumerator, dstDenominator,
          srcNumerator, srcDenominator,
          rounding);
    }

  }}}
