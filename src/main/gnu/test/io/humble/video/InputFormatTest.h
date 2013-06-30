/*
 * Copyright (c) 2013-Forward, Andrew "Art" Clarke
 *
 * This file is part of Humble Video.
 * 
 * Humble Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Humble Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble Video.  If not, see <http://www.gnu.org/licenses/>.
 *
 * InputFormatTest.h
 *
 *  Created on: Jun 28, 2013
 *      Author: aclarke
 */

#ifndef INPUTFORMATTEST_H_
#define INPUTFORMATTEST_H_

#include <io/humble/testutils/TestUtils.h>

class InputFormatTest : public CxxTest::TestSuite
{
public:
  InputFormatTest();
  virtual ~InputFormatTest();
  void setUp();
  void tearDown();
  void testCreateInputFormat();
};

#endif /* INPUTFORMATTEST_H_ */
