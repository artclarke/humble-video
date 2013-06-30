
// for isinf()
#include <math.h>

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
  VS_TUT_ENSURE("", num);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 0);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 1);

  num = Rational::make(6.0);
  VS_TUT_ENSURE("", num);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 6);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 1);

  num = Rational::make(6.1);
  VS_TUT_ENSURE("", num);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 61);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 10);
}

void
RationalTest :: testReduction()
{
  int retval = -1;
  num = Rational::make(2.2);
  VS_TUT_ENSURE("", num);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 11);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 5);

  retval = num->reduce(num->getNumerator()*5,
      num->getDenominator()*10, 100);
  VS_TUT_ENSURE("not exact", retval == 1);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 11);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 10);

  retval = Rational::sReduce(num.value(), 33, 32, 10);
  VS_TUT_ENSURE("exact?", retval == 0);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 1);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 1);

  // should be infinity
  retval = num->reduce(33, 0, 10);
  VS_TUT_ENSURE("not exact", retval == 1);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 1);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 0);
  VS_TUT_ENSURE("", isinf(num->getDouble()));

}

void
RationalTest :: testGetDouble()
{
  double retval = -1;
  num = Rational::make();

  retval = num->getDouble();
  VS_TUT_ENSURE_DISTANCE("", retval, 0, 0.0001);

  // now let make sure we can create infinity (and beyond...)
  num = Rational::make();
  num->reduce(1, 0, 10);
  retval = num->getDouble();
  VS_TUT_ENSURE("", isinf(retval));
}

void
RationalTest :: testMultiplication()
{
  RefPointer<Rational> a;
  RefPointer<Rational> b;

  a = Rational::make(12);
  b = Rational::make(3);
  num = Rational::sMultiply(a.value(), b.value());
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 36, 0.0001);
}

void
RationalTest :: testAddition()
{
  RefPointer<Rational> a;
  RefPointer<Rational> b;

  a = Rational::make(12);
  b = Rational::make(3);
  num = Rational::sAdd(a.value(), b.value());
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 15, 0.0001);
}

void
RationalTest :: testSubtraction()
{
  RefPointer<Rational> a;
  RefPointer<Rational> b;

  a = Rational::make(12);
  b = Rational::make(3);
  num = Rational::sSubtract(a.value(), b.value());
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 9, 0.0001);
}

void
RationalTest :: testDivision()
{
  RefPointer<Rational> a;
  RefPointer<Rational> b;

  a = Rational::make(12);
  b = Rational::make(3);
  num = Rational::sDivide(a.value(), b.value());
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 4, 0.0001);

  a = Rational::make(1);
  b = Rational::make(0.0);
  num = Rational::sDivide(a.value(), b.value());
  VS_TUT_ENSURE("", isinf(num->getDouble()));

  a = Rational::make(0.0);
  b = Rational::make(0.0);
  num = Rational::sDivide(a.value(), b.value());
  VS_TUT_ENSURE("", isnan(num->getDouble()));
}

void
RationalTest :: testConstructionFromNumeratorAndDenominatorPair()
{
  num = Rational::make(1, 10);
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 0.1, 0.0001);
  num = Rational::make(2, 10);
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 0.2, 0.0001);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 1);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 5);
}

void
RationalTest :: testRescaling()
{
  RefPointer<Rational> a;
  RefPointer<Rational> b;
  a = Rational::make(1, 100);
  VS_TUT_ENSURE_DISTANCE("", a->getDouble(), 0.01, 0.0001);
  b = Rational::make(1,5);
  VS_TUT_ENSURE_DISTANCE("", b->getDouble(), 0.2, 0.0001);

  VS_TUT_ENSURE_EQUALS("", a->rescale(1, b.value()), 20);
  VS_TUT_ENSURE_EQUALS("", b->rescale(1, a.value()), 0);
}
