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

// for std::numeric_limits
#include <limits>

#include <io/humble/video/RationalImpl.h>

namespace io { namespace humble { namespace video
{

  RationalImpl :: RationalImpl()
  {
    // default to 0 for the value.
    mRational.den = 1;
    mRational.num = 0;
    mInitialized = false;
  }

  RationalImpl :: ~RationalImpl()
  {
  }

  void
  RationalImpl :: init()
  {
    if (!mInitialized) {
      (void) reduce(mRational.num,
          mRational.den,
          FFMAX(mRational.den, mRational.num));
    }
    mInitialized = true;
  }
  
  RationalImpl *
  RationalImpl :: make(double d)
  {
    RationalImpl *result=0;
    result = RationalImpl::make();
    if (result) {
      result->setValue(d);
      result->init();
    }
    return result;
  }
  
  void
  RationalImpl :: setValue(double d)
  {
    if (!mInitialized)
      mRational = av_d2q(d, 0x7fffffff);
  }
  
  double
  RationalImpl :: getValue()
  {
    return getDouble();
  }

  RationalImpl *
  RationalImpl :: make(AVRational *src)
  {
    RationalImpl *result=0;
    if (src)
    {
      result = RationalImpl::make();
      if (result) {
        result->mRational = *src;
        result->init();
      }
    }
    return result;
  }

  Rational *
  RationalImpl :: copy()
  {
    return RationalImpl::make(this);
  }

  RationalImpl *
  RationalImpl :: make(RationalImpl *src)
  {
    RationalImpl *result=0;
    if (src)
    {
      result = RationalImpl::make();
      if (result) {
        result->mRational = src->mRational;
        result->init();
      }
    }
    return result;
  }
  void
  RationalImpl :: setNumerator(int32_t num)
  {
    if (!mInitialized)
      mRational.num = num;
  }
  void
  RationalImpl :: setDenominator(int32_t den)
  {
    if (!mInitialized)
      mRational.den = den;
  }
  bool
  RationalImpl :: isFinalized()
  {
    return mInitialized;
  }
  RationalImpl *
  RationalImpl :: make(int32_t num, int32_t den)
  {
    RationalImpl *result=0;
    result = RationalImpl::make();
    if (result) {
      result->setNumerator(num);
      result->setDenominator(den);
      result->init();
    }
    return result;
  }
  int32_t
  RationalImpl :: compareTo(Rational *other)
  {
    int32_t result = 0;
    RationalImpl *arg=dynamic_cast<RationalImpl*>(other);
    if (arg)
      result = av_cmp_q(mRational, arg->mRational);
    return result;
  }

  double
  RationalImpl :: getDouble()
  {
    double result = 0;
    // On some runs in Linux calling av_q2d will raise
    // a FPE instead of returning back NaN or infinity,
    // so we try to short-circuit that here.
    if (mRational.den == 0)
      if (mRational.num == 0)
        result = std::numeric_limits<double>::quiet_NaN();
      else
        result = std::numeric_limits<double>::infinity();
    else
      result =  av_q2d(mRational);
    return result;
  }

  int32_t
  RationalImpl :: reduce(int64_t num, int64_t den, int64_t max)
  {
    int32_t result = 0;
    result =  av_reduce(&mRational.num, &mRational.den,
        num, den, max);
    return result;
  }

  Rational *
  RationalImpl :: multiply(Rational *other)
  {
    RationalImpl *result = 0;
    RationalImpl *arg=dynamic_cast<RationalImpl*>(other);
    if (arg)
    {
      result = RationalImpl::make();
      if (result)
      {
        result->mRational = av_mul_q(this->mRational,
            arg->mRational);
      }
    }
    return result;
  }

  Rational *
  RationalImpl :: divide(Rational *other)
  {
    RationalImpl *result = 0;
    RationalImpl *arg=dynamic_cast<RationalImpl*>(other);
    if (arg)
    {
      result = RationalImpl::make();
      if (result)
      {
        result->mRational = av_div_q(this->mRational,
            arg->mRational);
      }
    }
    return result;
  }

  Rational *
  RationalImpl :: subtract(Rational *other)
  {
    RationalImpl *result = 0;
    RationalImpl *arg=dynamic_cast<RationalImpl*>(other);
    if (arg)
    {
      result = RationalImpl::make();
      if (result)
      {
        result->mRational = av_sub_q(this->mRational,
            arg->mRational);
      }
    }
    return result;
  }
  Rational *
  RationalImpl :: add(Rational *other)
  {
    RationalImpl *result = 0;
    RationalImpl *arg=dynamic_cast<RationalImpl*>(other);
    if (arg)
    {
      result = RationalImpl::make();
      if (result)
      {
        result->mRational = av_add_q(this->mRational,
            arg->mRational);
      }
    }
    return result;
  }

  int64_t
  RationalImpl :: rescale(int64_t origValue, Rational *origBase)
  {
    int64_t retval=origValue;
    RationalImpl *arg=dynamic_cast<RationalImpl*>(origBase);

    if (arg)
    {
      retval = av_rescale_q(origValue, arg->mRational, this->mRational);
    }
    return retval;
  }
  
  int64_t
  RationalImpl :: rescale(int64_t origValue, Rational *origBase,
      Rounding rounding)
  {
    int64_t retval=origValue;
    RationalImpl *arg=dynamic_cast<RationalImpl*>(origBase);

    if (arg)
    {
      int64_t b = arg->mRational.num  * (int64_t)this->mRational.den;
      int64_t c = this->mRational.num * (int64_t)arg->mRational.den;

      retval = av_rescale_rnd(origValue, b,
          c, (enum AVRounding)rounding);
    }
    return retval;
  }
  
  int64_t
  RationalImpl :: rescale(int64_t srcValue,
      int32_t dstNumerator,
      int32_t dstDenominator,
      int32_t srcNumerator,
      int32_t srcDenominator,
      Rounding rounding)
  {
    int64_t retval = srcValue;
    if (!dstNumerator || !dstDenominator ||
        !srcNumerator || !srcDenominator)
      return 0;

    int64_t b = srcNumerator * (int64_t)dstDenominator;
    int64_t c = dstNumerator * (int64_t)srcDenominator;

    retval = av_rescale_rnd(srcValue, b,
        c, (enum AVRounding)rounding);

    return retval;
  }
 
}}}
