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
#ifndef __RATIONAL_TEST_H__
#define __RATIONAL_TEST_H__

#include <io/humble/testutils/TestUtils.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/Rational.h>

using namespace io::humble::video;

class RationalTest : public CxxTest::TestSuite
{
  public:
    void setUp();
    void testCreationAndDestruction();
    void testReduction();
    void testGetDouble();
    void testMultiplication();
    void testAddition();
    void testSubtraction();
    void testDivision();
    void testConstructionFromNumeratorAndDenominatorPair();
    void testRescaling();
  private:
    io::humble::ferry::RefPointer<Rational> num;
};


#endif // __RATIONAL_TEST_H__

