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

#ifndef REFCOUNTEDTESTER_H_
#define REFCOUNTEDTESTER_H_

#include <io/humble/ferry/RefCounted.h>

namespace io { namespace humble { namespace ferry {


/**
 * Internal Only.  This object exists in order for the ferry test
 * libraries to test the memory management functionality
 * of the RefCounted class from Java.
 *
 * It is NOT part of the public API.
 */
class RefCountedTester : public RefCounted
{
  VS_JNIUTILS_REFCOUNTED_OBJECT(RefCountedTester);
public:

  /**
   * Acquires a reference to a passed in object,
   * and returns the new object.  This, when wrapped
   * by Swig, will be wrapped in a new Java object.
   */
  static RefCountedTester *make(RefCountedTester *objToAcquire);

protected:
  RefCountedTester();
  virtual ~RefCountedTester();
};

}

}

}

#endif /* REFCOUNTEDTESTER_H_ */
