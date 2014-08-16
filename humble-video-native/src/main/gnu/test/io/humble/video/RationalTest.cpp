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
// for isinf()
#include <cmath>

#include <io/humble/video/Global.h>
#include "RationalTest.h"

using namespace io::humble::ferry;

void
RationalTest :: setUp()
{
  num = 0;
}

void
RationalTest :: testCreationAndDestruction()
{
  num = Rational::make();
  TSM_ASSERT("", num);
  TSM_ASSERT_EQUALS("", num->getNumerator(), 0);
  TSM_ASSERT_EQUALS("", num->getDenominator(), 1);

  num = Rational::make(6.0);
  TSM_ASSERT("", num);
  TSM_ASSERT_EQUALS("", num->getNumerator(), 6);
  TSM_ASSERT_EQUALS("", num->getDenominator(), 1);

  num = Rational::make(6.1);
  TSM_ASSERT("", num);
  TSM_ASSERT_EQUALS("", num->getNumerator(), 61);
  TSM_ASSERT_EQUALS("", num->getDenominator(), 10);
}

void
RationalTest :: testReduction()
{
  int retval = -1;
  num = Rational::make(2.2);
  TSM_ASSERT("", num);
  TSM_ASSERT_EQUALS("", num->getNumerator(), 11);
  TSM_ASSERT_EQUALS("", num->getDenominator(), 5);

  retval = num->reduce(num->getNumerator()*5,
      num->getDenominator()*10, 100);
  TSM_ASSERT("not exact", retval == 1);
  TSM_ASSERT_EQUALS("", num->getNumerator(), 11);
  TSM_ASSERT_EQUALS("", num->getDenominator(), 10);

  retval = Rational::sReduce(num.value(), 33, 32, 10);
  TSM_ASSERT("exact?", retval == 0);
  TSM_ASSERT_EQUALS("", num->getNumerator(), 1);
  TSM_ASSERT_EQUALS("", num->getDenominator(), 1);

  // should be infinity
  retval = num->reduce(33, 0, 10);
  TSM_ASSERT("not exact", retval == 1);
  TSM_ASSERT_EQUALS("", num->getNumerator(), 1);
  TSM_ASSERT_EQUALS("", num->getDenominator(), 0);
  double n = num->getDouble();
  bool inf = std::isinf(n);
  TSM_ASSERT("", inf);

}

void
RationalTest :: testGetDouble()
{
  double retval = -1;
  num = Rational::make();

  retval = num->getDouble();
  TSM_ASSERT_DELTA("", retval, 0, 0.0001);

  // now let make sure we can create infinity (and beyond...)
  num = Rational::make();
  num->reduce(1, 0, 10);
  retval = num->getDouble();
  TSM_ASSERT("", std::isinf(retval));
}

void
RationalTest :: testMultiplication()
{
  RefPointer<Rational> a;
  RefPointer<Rational> b;

  a = Rational::make(12);
  b = Rational::make(3);
  num = Rational::sMultiply(a.value(), b.value());
  TSM_ASSERT_DELTA("", num->getDouble(), 36, 0.0001);
}

void
RationalTest :: testAddition()
{
  RefPointer<Rational> a;
  RefPointer<Rational> b;

  a = Rational::make(12);
  b = Rational::make(3);
  num = Rational::sAdd(a.value(), b.value());
  TSM_ASSERT_DELTA("", num->getDouble(), 15, 0.0001);
}

void
RationalTest :: testSubtraction()
{
  RefPointer<Rational> a;
  RefPointer<Rational> b;

  a = Rational::make(12);
  b = Rational::make(3);
  num = Rational::sSubtract(a.value(), b.value());
  TSM_ASSERT_DELTA("", num->getDouble(), 9, 0.0001);
}

void
RationalTest :: testDivision()
{
  RefPointer<Rational> a;
  RefPointer<Rational> b;

  a = Rational::make(12);
  b = Rational::make(3);
  num = Rational::sDivide(a.value(), b.value());
  TSM_ASSERT_DELTA("", num->getDouble(), 4, 0.0001);

  a = Rational::make(1);
  b = Rational::make(0.0);
  num = Rational::sDivide(a.value(), b.value());
  TSM_ASSERT("", std::isinf(num->getDouble()));

  a = Rational::make(0.0);
  b = Rational::make(0.0);
  num = Rational::sDivide(a.value(), b.value());
  TSM_ASSERT("", std::isnan(num->getDouble()));
}

void
RationalTest :: testConstructionFromNumeratorAndDenominatorPair()
{
  num = Rational::make(1, 10);
  TSM_ASSERT_DELTA("", num->getDouble(), 0.1, 0.0001);
  num = Rational::make(2, 10);
  TSM_ASSERT_DELTA("", num->getDouble(), 0.2, 0.0001);
  TSM_ASSERT_EQUALS("", num->getNumerator(), 1);
  TSM_ASSERT_EQUALS("", num->getDenominator(), 5);
}

void
RationalTest :: testRescaling()
{
  RefPointer<Rational> a;
  RefPointer<Rational> b;
  a = Rational::make(1, 100);
  TSM_ASSERT_DELTA("", a->getDouble(), 0.01, 0.0001);
  b = Rational::make(1,5);
  TSM_ASSERT_DELTA("", b->getDouble(), 0.2, 0.0001);

  TSM_ASSERT_EQUALS("", a->rescale(1, b.value()), 20);
  TSM_ASSERT_EQUALS("", b->rescale(1, a.value()), 0);
}
