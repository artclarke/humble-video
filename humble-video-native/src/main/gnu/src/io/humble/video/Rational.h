/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef IRATIONAL_H_
#define IRATIONAL_H_

#include <io/humble/ferry/RefCounted.h>
#include <io/humble/video/HumbleVideo.h>

namespace io { namespace humble { namespace video
{

  /**
   * This class wraps represents a Rational number.
   * <p>
   * Video formats often use rational numbers, and converting between
   * them willy nilly can lead to rounding errors, and eventually, out
   * of sync problems.  Therefore we use IRational objects to pass
   * around Rational Numbers and avoid conversion until the very last moment.
   * </p><p>
   * Note: There are some static convenience methods
   * in this class that start with s*.  They start with s
   * (as opposed to overloading methods (e.g. sAdd(...) vs. add(...)).
   * </p>
   */
  class VS_API_HUMBLEVIDEO Rational : public io::humble::ferry::RefCounted
  {
  public:
    
    /**
     * A type to specify how rounding should be done.
     */
    typedef enum {
      /** Round toward zero. */
      ROUND_ZERO     = 0,
      /** Round away from zero. */
      ROUND_INF      = 1,
      /** Round toward -infinity. */
      ROUND_DOWN     = 2,
      /** Round toward +infinity. */
      ROUND_UP       = 3,
      /** Round to nearest and halfway cases away from zero. */
      ROUND_NEAR_INF = 5,
      /** Flag to pass INT64_MIN/MAX through instead of rescaling, this avoids special cases for AV_NOPTS_VALUE */
      ROUND_PASS_MINMAX = 8192,

    } Rounding;

    /**
     * Get the numerator for this rational.
     * @return the numerator.
     */
    
    virtual int32_t getNumerator()=0;
    
    /**
     * Get the denominator for this rational.
     * @return the denominator.
     */
    virtual int32_t getDenominator()=0;

    /**
     * Creates a new IRational object by copying (by value) this object.
     *
     * @return the new object
     */
    virtual Rational * copy()=0;

    /**
     * Compare a rational to this rational
     * @param other second rational
     * @return 0 if this==other, 1 if this>other and -1 if this<other.
     */
    virtual int32_t compareTo(Rational*other)=0;
    
    /**
     * Compare two rationals
     * @param a the first rational
     * @param b the second rational
     * @return 0 if a==b, 1 if a>b and -1 if b<a.
     */
    
    static int32_t sCompareTo(Rational *a, Rational *b);

    /**
     * Rational to double conversion.
     *
     * @return (double) a
     */
    virtual double getDouble()=0;

    /**
     * Reduce a fraction to it's lowest common denominators.
     * This is useful for framerate calculations.
     * @param num       the src numerator.
     * @param den       the src denominator.
     * @param max the maximum allowed for nom & den in the reduced fraction.
     * @return 1 if exact, 0 otherwise
     */
    virtual int32_t reduce(int64_t num, int64_t den, int64_t max)=0;
    
    /**
     * Reduce a fraction to it's lowest common denominators.
     * This is useful for framerate calculations.
     * @param dst The destination rational  
     * @param num       the src numerator.
     * @param den       the src denominator.
     * @param max the maximum allowed for nom & den in the reduced fraction.
     * @return 1 if exact, 0 otherwise
     */
    static int32_t sReduce(Rational *dst, int64_t num,
        int64_t den, int64_t max);

    /**
     * Multiplies this number by arg
     * @param arg number to mulitply by.
     * @return this*arg.
     */
    virtual Rational* multiply(Rational *arg)=0;
    
    /**
     * Multiples a by b.
     * @param a the first number
     * @param b the second number.
     * @return a*b
     */
    static Rational* sMultiply(Rational* a, Rational*b);

    /**
     * Divides this rational by arg.
     * @param arg The divisor to use.
     * @return this/arg.
     */
    virtual Rational* divide(Rational *arg)=0;

    /**
     * Divides a by b.
     * @param a The first number.
     * @parma b The second number.
     * @return a/b.
     */
    static Rational* sDivide(Rational *a, Rational* b);

    /**
     * Subtracts arg from this rational
     * @param arg The amount to subtract from this.
     * @return this-arg.
     */
    virtual Rational* subtract(Rational *arg)=0;
    /**
     * Subtracts a from b.
     * @param a The first number.
     * @parma b The second number.
     * @return a-b.
     */
    static Rational* sSubtract(Rational *a, Rational* b);

    /**
     * Adds arg to this rational
     * @param arg The amount to add to this.
     * @return this+arg.
     */
    virtual Rational* add(Rational *arg)=0;
    /**
     * Adds a to b.
     * @param a The first number.
     * @parma b The second number.
     * @return a+b.
     */
    static Rational* sAdd(Rational *a, Rational* b);

    /**
     * Takes a value scaled in increments of origBase and gives the
     * equivalent value scaled in terms of this Rational.
     *
     * @param origValue The original int64_t value you care about.
     * @param origBase The original base Rational that origValue is scaled with.
     *
     * @return The new integer value, scaled in units of this IRational.
     */
    virtual int64_t rescale(int64_t origValue, Rational* origBase)=0;

    /**
     * Takes a value scaled in increments of origBase and gives the
     * equivalent value scaled in terms of this Rational.
     *
     * @param origValue The original int64_t value you care about.
     * @param origBase The original base Rational that origValue is scaled with.
     * @param newBase The rational you want to rescale origValue into.
     *
     * @return The new integer value, scaled in units of this IRational.
     */
    static int64_t sRescale(int64_t origValue, Rational* origBase, Rational* newBase);
    
    /**
     * Get a new rational that will be set to 0/1.
     * The rational will not have #init() called
     * and hence will be modifiable by #setValue(double)
     * until #init() is called.
     * @return a rational number object
     */
    static Rational *make();
    
    /**
     * Converts a double precision floating point number to a rational.
     * @param d double to convert
     * @return A new Rational; caller must release() when done.
     */
    static Rational *make(double d);
    /**
     * Creates deep copy of a Rational from another Rational.
     *
     * @param src       The source Rational to copy.
     * @return A new Rational; Returns null
     *         if src is null.
     */
    static Rational* make(Rational *src);

    /**
     * Create a rational from a numerator and denominator.
     *
     * We will always reduce this to the lowest num/den pair
     * we can, but never having den exceed what was passed in.
     *
     * @param num The numerator of the resulting Rational
     * @param den The denominator of the resulting Rational
     *
     * @return A new Rational; 
     */
    static Rational *make(int32_t num, int32_t den);

    /**
     * Takes a value scaled in increments of origBase and gives the
     * equivalent value scaled in terms of this Rational.
     *
     * @param origValue The original int64_t value you care about.
     * @param origBase The original base Rational that origValue is scaled with.
     * @param rounding How you want rounding to occur
     * @return The new integer value, scaled in units of this IRational.
     */
    virtual int64_t rescale(int64_t origValue,
        Rational* origBase,
        Rounding rounding)=0;

    /**
     * Takes a value scaled in increments of origBase and gives the
     * equivalent value scaled in terms of this Rational.
     *
     * @param origValue The original int64_t value you care about.
     * @param origBase The original base Rational that origValue is scaled with.
     * @param newBase The rational you want to rescale origValue into.
     * @param rounding How you want rounding to occur
     * 
     * @return The new integer value, scaled in units of this IRational.
     */
    static int64_t sRescale(int64_t origValue,
        Rational* origBase, Rational* newBase,
        Rounding rounding);

    /**
     * Rescales a long value to another long value.
     * <p>
     * This method doesn't use IRational values, but
     * instead uses numerators and denominators
     * passed in by the caller.  It will not result
     * in any memory allocations.
     * </p>
     * 
     * @param srcValue The value to rescale.
     * @param dstNumerator The numerator of the units
     *   you want to scale to.  Must be non-zero.
     * @param dstDenominator The denominator of the units
     *   you want to scale to.  Must be non-zero.
     * @param srcNumerator The numerator of the units
     *   <code>srcValue</code> is expressed in.
     *   Must be non-zero.
     * @param srcDenominator The denominator of the units
     *   <code>srcValue</code> is expressed in.
     *   Must be non-zero.
     * @param rounding How you want rounding to occur
     * 
     * @return The new integer value, scaled in units of
     *   dstNumerator/dstNumerator, or 0 if there
     *   is a parameter error.
     */
    static int64_t rescale(int64_t srcValue,
        int32_t dstNumerator,
        int32_t dstDenominator,
        int32_t srcNumerator,
        int32_t srcDenominator,
        Rounding rounding);

   /**
    * Sets the numerator on this object.
    * <p>
    * If #isFinalized is true, then this method is ignored.
    * </p>
    * 
    * 
    */
   virtual void setNumerator(int32_t value)=0;

   
   /**
    * Sets the denominator on this object.
    * <p>
    * If #isFinalized is true, then this method is ignored.
    * </p>
    * 
    * 
    */
   virtual void setDenominator(int32_t value)=0;
   
   /**
    * Sets the numerator and denominator on this object by
    * reducing the double to the closest integer numerator
    * and denominator.
    * <p>
    * If #isFinalized is true, then this method is ignored.
    * </p>
    * 
    * 
    */
   virtual void setValue(double value)=0;

   /**
    * An alias for #getDouble() but matching JavaBean
    * conventions.
    * 
    */
   virtual double getValue()=0;

   /**
    * Returns true if #init() has been called and
    * this object is now considered finalized and immutable.
    * 
    * 
    */
   virtual bool isFinalized()=0;
   
   /**
    * Marks this object as finalized and immutable.  Any
    * setters called after the first #init() call
    * will be ignored.
    * <p>
    * Most <code>make</code> methods will call this method
    * automatically, with the exception of the blank factory
    * method #make().
    * </p> 
    * 
    */
   virtual void init()=0;
  protected:
    Rational();
    virtual ~Rational();
    
  public:
  };

}}}

#endif /*IRATIONAL_H_*/
