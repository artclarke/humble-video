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

%typemap (javacode) io::humble::video::Rational,io::humble::video::Rational*,io::humble::video::Rational& %{

  /**
   * Prints the contents of this object as a fraction.
   * @return &quot;{@link #getNumerator()}/{@link #getDenominator()}&quot;
   */
   
  @Override
  public String toString()
  {
    return "" + getNumerator() + "/" + getDenominator();
  }
  /**
   * Is this number positive?
   * @return true if > 0; false if not.
   */
  public boolean isPositive()
  {
    if (getDenominator() == 0)
      return false;
    return getDouble() > 0;
  }
  /**
   * Is this number not null and positive.
   * @param num the number; may be null
   * @return true if not-null and > 0; else false;
   */
  public static boolean positive(Rational num)
  {
    if (num == null)
      return false;
    return num.isPositive();
  }
  /**
   * Is this number negative?
   * @return true if < 0; false if not.
   */
  public boolean isNegative()
  {
    if (getDenominator() == 0)
      return false;
    return getDouble() < 0;
  }
  /**
   * Is this number not null and negative.
   * @param num the number; may be null
   * @return true if not-null and < 0; else false;
   */
  public static boolean negative(Rational num)
  {
    if (num == null)
      return false;
    return num.isNegative();
  }
%}

%include <io/humble/video/Rational.h>
