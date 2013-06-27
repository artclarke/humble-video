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

#include <io/humble/video/Rational.h>

namespace io { namespace humble { namespace video
{

  Rational :: Rational()
  {
    // default to 0 for the value.
    mRational.den = 1;
    mRational.num = 0;
    mInitialized = false;
  }

  Rational :: ~Rational()
  {
  }

  void
  Rational :: init()
  {
    if (!mInitialized) {
      (void) reduce(mRational.num,
          mRational.den,
          FFMAX(mRational.den, mRational.num));
    }
    mInitialized = true;
  }
  
  Rational *
  Rational :: make(double d)
  {
    Rational *result=0;
    result = Rational::make();
    if (result) {
      result->setValue(d);
      result->init();
    }
    return result;
  }
  
  void
  Rational :: setValue(double d)
  {
    if (!mInitialized)
      mRational = av_d2q(d, 0x7fffffff);
  }
  
  double
  Rational :: getValue()
  {
    return getDouble();
  }

  Rational *
  Rational :: make(AVRational *src)
  {
    Rational *result=0;
    if (src)
    {
      result = Rational::make();
      if (result) {
        result->mRational = *src;
        result->init();
      }
    }
    return result;
  }

  IRational *
  Rational :: copy()
  {
    return Rational::make(this);
  }

  Rational *
  Rational :: make(Rational *src)
  {
    Rational *result=0;
    if (src)
    {
      result = Rational::make();
      if (result) {
        result->mRational = src->mRational;
        result->init();
      }
    }
    return result;
  }
  void
  Rational :: setNumerator(int32_t num)
  {
    if (!mInitialized)
      mRational.num = num;
  }
  void
  Rational :: setDenominator(int32_t den)
  {
    if (!mInitialized)
      mRational.den = den;
  }
  bool
  Rational :: isFinalized()
  {
    return mInitialized;
  }
  Rational *
  Rational :: make(int32_t num, int32_t den)
  {
    Rational *result=0;
    result = Rational::make();
    if (result) {
      result->setNumerator(num);
      result->setDenominator(den);
      result->init();
    }
    return result;
  }
  int32_t
  Rational :: compareTo(IRational *other)
  {
    int32_t result = 0;
    Rational *arg=dynamic_cast<Rational*>(other);
    if (arg)
      result = av_cmp_q(mRational, arg->mRational);
    return result;
  }

  double
  Rational :: getDouble()
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
  Rational :: reduce(int64_t num, int64_t den, int64_t max)
  {
    int32_t result = 0;
    result =  av_reduce(&mRational.num, &mRational.den,
        num, den, max);
    return result;
  }

  IRational *
  Rational :: multiply(IRational *other)
  {
    Rational *result = 0;
    Rational *arg=dynamic_cast<Rational*>(other);
    if (arg)
    {
      result = Rational::make();
      if (result)
      {
        result->mRational = av_mul_q(this->mRational,
            arg->mRational);
      }
    }
    return result;
  }

  IRational *
  Rational :: divide(IRational *other)
  {
    Rational *result = 0;
    Rational *arg=dynamic_cast<Rational*>(other);
    if (arg)
    {
      result = Rational::make();
      if (result)
      {
        result->mRational = av_div_q(this->mRational,
            arg->mRational);
      }
    }
    return result;
  }

  IRational *
  Rational :: subtract(IRational *other)
  {
    Rational *result = 0;
    Rational *arg=dynamic_cast<Rational*>(other);
    if (arg)
    {
      result = Rational::make();
      if (result)
      {
        result->mRational = av_sub_q(this->mRational,
            arg->mRational);
      }
    }
    return result;
  }
  IRational *
  Rational :: add(IRational *other)
  {
    Rational *result = 0;
    Rational *arg=dynamic_cast<Rational*>(other);
    if (arg)
    {
      result = Rational::make();
      if (result)
      {
        result->mRational = av_add_q(this->mRational,
            arg->mRational);
      }
    }
    return result;
  }

  int64_t
  Rational :: rescale(int64_t origValue, IRational *origBase)
  {
    int64_t retval=origValue;
    Rational *arg=dynamic_cast<Rational*>(origBase);

    if (arg)
    {
      retval = av_rescale_q(origValue, arg->mRational, this->mRational);
    }
    return retval;
  }
  
  int64_t
  Rational :: rescale(int64_t origValue, IRational *origBase,
      Rounding rounding)
  {
    int64_t retval=origValue;
    Rational *arg=dynamic_cast<Rational*>(origBase);

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
  Rational :: rescale(int64_t srcValue,
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
