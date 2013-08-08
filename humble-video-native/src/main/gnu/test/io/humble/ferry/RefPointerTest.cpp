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

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/RefPointer.h>
#include "RefPointerTest.h"

using namespace VS_CPP_NAMESPACE;

VS_LOG_SETUP(VS_CPP_PACKAGE);

// This is the object we'll attempt to create and destroy.
class RefCountedObject : public RefCounted
{
  VS_JNIUTILS_REFCOUNTED_OBJECT(RefCountedObject);
  protected:
  RefCountedObject() {}
  virtual ~RefCountedObject() {}
};


void
RefPointerTestSuite :: testCreateAndDestroy()
{
  RefCountedObject * obj = RefCountedObject::make();

  TSM_ASSERT("could not create our object", obj);

  // now we're going to cheat, and add a reference to ensure obj
  // exists across the RefPointer tests
  obj->acquire();
  {
    RefPointer<RefCountedObject> p(obj);
    TSM_ASSERT("RefPointer has wrong reference count", p->getCurrentRefCount() == 2);
  }
  TSM_ASSERT("RefPointer did not release correctly", obj->getCurrentRefCount() == 1);
  VS_REF_RELEASE(obj);
}

void
RefPointerTestSuite :: testPointerOperations()
{
  RefCountedObject * obj = RefCountedObject::make();

  TSM_ASSERT("could not create our object", obj);

  // now we're going to cheat, and add a reference to ensure obj
  // exists across the RefPointer tests
  obj->acquire();
  {
    RefPointer<RefCountedObject> p(obj);
    
    TSM_ASSERT("RefPointer has a valid reference", p.value());

    TSM_ASSERT_EQUALS("Object has wrong reference count",
        obj->getCurrentRefCount(),
        2
    );
    
    RefCountedObject *anotherRef=0;
    anotherRef = p.get();
    TSM_ASSERT_EQUALS("Object has wrong reference count after get",
        obj->getCurrentRefCount(),
        3
    );
    VS_REF_RELEASE(anotherRef);
    TSM_ASSERT_EQUALS("Object has wrong reference count after other object release",
        obj->getCurrentRefCount(),
        2
    );
    
    p.reset();
    TSM_ASSERT_EQUALS("Object has wrong reference count after other object release",
        obj->getCurrentRefCount(),
        1
    );
    TSM_ASSERT("p has something it shouldn't", !p.value());
    
    // test assignment
    // give obj another acquire since it's about to pass ownership back
    // to p.
    obj->acquire();
    p = obj;
    TSM_ASSERT_EQUALS("Object has wrong reference count after other object release",
            obj->getCurrentRefCount(),
            2
    );
    // and make sure we re-release and re-acquire
    // give obj another acquire since it's about to pass ownership back
    // to p.
    obj->acquire();
    p = obj;
    TSM_ASSERT_EQUALS("Object has wrong reference count after other object release",
            obj->getCurrentRefCount(),
            2
    );

  }
  TSM_ASSERT("RefPointer did not release correctly", obj->getCurrentRefCount() == 1);
  VS_REF_RELEASE(obj);
}

void
RefPointerTestSuite :: testDefaultConstructor()
{
  RefPointer<RefCountedObject> p;
  TSM_ASSERT("pointer initialized to nothing", !p.value());
}

void
RefPointerTestSuite :: testRefPointerCopy()
{  
  RefCountedObject * obj = RefCountedObject::make();
  {
    RefPointer<RefCountedObject> p;
    RefPointer<RefCountedObject> q;
    TSM_ASSERT("have an object", obj);
    TSM_ASSERT_EQUALS("unexpected ref count", obj->getCurrentRefCount(), 1);

    TSM_ASSERT("pointer initialized to nothing", !p);
    TSM_ASSERT("pointer initialized to nothing", !q);

    p = obj;
    TSM_ASSERT_EQUALS("unexpected ref count", p->getCurrentRefCount(), 1);
    obj = 0;
    // copy the object
    q = p;
    TSM_ASSERT_EQUALS("unexpected ref count", p->getCurrentRefCount(), 2);
    TSM_ASSERT_EQUALS("unexpected ref count", q->getCurrentRefCount(), 2);

    p = 0;
    TSM_ASSERT("unexpected ref count", !p);
    TSM_ASSERT_EQUALS("unexpected ref count", q->getCurrentRefCount(), 1);
  }
  TSM_ASSERT("make sure we nuked this", !obj);
}
