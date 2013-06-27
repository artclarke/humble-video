
// for isinf()
#include <math.h>

#include <io/humble/video/Global.h>
#include "RationalTest.h"

using namespace io::humble::ferry;
using namespace VS_CPP_NAMESPACE;

void
RationalTest :: setUp()
{
  num = 0;
}

void
RationalTest :: testCreationAndDestruction()
{
  num = IRational::make();
  VS_TUT_ENSURE("", num);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 0);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 1);

  num = IRational::make(6.0);
  VS_TUT_ENSURE("", num);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 6);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 1);

  num = IRational::make(6.1);
  VS_TUT_ENSURE("", num);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 61);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 10);
}

void
RationalTest :: testReduction()
{
  int retval = -1;
  num = IRational::make(2.2);
  VS_TUT_ENSURE("", num);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 11);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 5);

  retval = num->reduce(num->getNumerator()*5,
      num->getDenominator()*10, 100);
  VS_TUT_ENSURE("not exact", retval == 1);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 11);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 10);

  retval = IRational::sReduce(num.value(), 33, 32, 10);
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
  num = IRational::make();

  retval = num->getDouble();
  VS_TUT_ENSURE_DISTANCE("", retval, 0, 0.0001);

  // now let make sure we can create infinity (and beyond...)
  num = IRational::make();
  num->reduce(1, 0, 10);
  retval = num->getDouble();
  VS_TUT_ENSURE("", isinf(retval));
}

void
RationalTest :: testMultiplication()
{
  RefPointer<IRational> a;
  RefPointer<IRational> b;

  a = IRational::make(12);
  b = IRational::make(3);
  num = IRational::sMultiply(a.value(), b.value());
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 36, 0.0001);
}

void
RationalTest :: testAddition()
{
  RefPointer<IRational> a;
  RefPointer<IRational> b;

  a = IRational::make(12);
  b = IRational::make(3);
  num = IRational::sAdd(a.value(), b.value());
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 15, 0.0001);
}

void
RationalTest :: testSubtraction()
{
  RefPointer<IRational> a;
  RefPointer<IRational> b;

  a = IRational::make(12);
  b = IRational::make(3);
  num = IRational::sSubtract(a.value(), b.value());
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 9, 0.0001);
}

void
RationalTest :: testDivision()
{
  RefPointer<IRational> a;
  RefPointer<IRational> b;

  a = IRational::make(12);
  b = IRational::make(3);
  num = IRational::sDivide(a.value(), b.value());
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 4, 0.0001);

  a = IRational::make(1);
  b = IRational::make(0.0);
  num = IRational::sDivide(a.value(), b.value());
  VS_TUT_ENSURE("", isinf(num->getDouble()));

  a = IRational::make(0.0);
  b = IRational::make(0.0);
  num = IRational::sDivide(a.value(), b.value());
  VS_TUT_ENSURE("", isnan(num->getDouble()));
}

void
RationalTest :: testConstructionFromNumeratorAndDenominatorPair()
{
  num = IRational::make(1, 10);
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 0.1, 0.0001);
  num = IRational::make(2, 10);
  VS_TUT_ENSURE_DISTANCE("", num->getDouble(), 0.2, 0.0001);
  VS_TUT_ENSURE_EQUALS("", num->getNumerator(), 1);
  VS_TUT_ENSURE_EQUALS("", num->getDenominator(), 5);
}

void
RationalTest :: testRescaling()
{
  RefPointer<IRational> a;
  RefPointer<IRational> b;
  a = IRational::make(1, 100);
  VS_TUT_ENSURE_DISTANCE("", a->getDouble(), 0.01, 0.0001);
  b = IRational::make(1,5);
  VS_TUT_ENSURE_DISTANCE("", b->getDouble(), 0.2, 0.0001);

  VS_TUT_ENSURE_EQUALS("", a->rescale(1, b.value()), 20);
  VS_TUT_ENSURE_EQUALS("", b->rescale(1, a.value()), 0);
}
