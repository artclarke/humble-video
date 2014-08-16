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
#ifndef INDEXENTRYTEST_H_
#define INDEXENTRYTEST_H_

#include <io/humble/testutils/TestUtils.h>

class IndexEntryTest: public CxxTest::TestSuite
{
public:
  IndexEntryTest();
  virtual
  ~IndexEntryTest();
  void setUp();
  void tearDown();
  void testCreation();
#if 0
  void notestAddIndexEntry();
  void notestGetIndexEntry();
#endif
};

#endif /* INDEXENTRYTEST_H_ */
