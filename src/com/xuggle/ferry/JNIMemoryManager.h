/*******************************************************************************
 * Copyright (c) 2008, 2010 Xuggle Inc.  All rights reserved.
 *  
 * This file is part of Xuggle-Xuggler-Main.
 *
 * Xuggle-Xuggler-Main is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xuggle-Xuggler-Main is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Xuggle-Xuggler-Main.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

/*
 * JNIMemoryManagement.h
 *
 *  Created on: Aug 28, 2008
 *      Author: aclarke
 */

#ifndef JNIMEMORYMANAGER_H_
#define JNIMEMORYMANAGER_H_

#include <cstdlib>

#include <com/xuggle/ferry/Ferry.h>

namespace com { namespace xuggle { namespace ferry {

/**
 * This class's implementation of malloc() and free()
 * will forward to the standard library's malloc() and
 * free(), UNLESS you're running in a JVM, in which case
 * it will use Java to manage the memory for us.
 *
 * Use it when you have large memory structures that
 * live for a long time that you want the JVM to resource
 * manage for you, or if you're trying to track down
 * an object in Java that is collecting large objects; this
 * will allow you to tie back big chunks of memory to the
 * misbehaving java object in your code (using a memory
 * profiler like SAP's Eclipse plugin).
 *
 */
class VS_API_FERRY JNIMemoryManager
{
public:
  /**
   * Create a malloced block of AT LEAST site_t bounds long.
   *
   * This block will be aligned on a 128-bit boundary, suitable
   * for passing to libraries that do SSE-based operations on it.
   *
   * @param requested_size Requested minimum size of memory, in bytes
   *
   * @throws std::std_alloc If memory cannot be allocated
   *
   * @return a block of memory of at least requested size aligned on a 128-bit boundary.
   */
  static void* malloc(size_t requested_size);

  /**
   * Free memory previously allocated by malloc(void*, size_t).
   * @param mem previously allocated by malloc(void*, size_t)
   */
  static void free(void *mem);

  /**
   * Create a malloced block of AT LEAST site_t bounds long by using an allocator.
   *
   * Allocates a block of memory using the passed in allocator.  If running
   * inside a JVM, allocator must be a jobject, and must be a subclass of
   * com.xuggle.ferry.JNIMemoryManager.  Also you must ensure
   * that allocator lives longer than the block of memory you allocate
   * (i.e. does not get collected by the GC).
   *
   * This method is useful for Swig proxy objects where you want to fake
   * a reference from the proxy object to memory allocated by the object the
   * proxy objet mallocs.  Then, when checked using profiling tools, the
   * native memory will accumulate to the proxy object, which will help you
   * find proxy leaks.
   *
   * If you are not allocating with the JVM, or we are using JVM
   * Direct Buffers for memory, the allocator object is only used to
   * help print debugging messages.
   *
   * This block will be aligned on a 128-bit boundary, suitable
   * for passing to libraries that do SSE-based operations on it.
   *
   * Lastly if you understood or followed any of the above comments
   * you're not drunk enough to work on this code.
   *
   * @param requested_size Requested minimum size of memory, in bytes
   * @param allocator Either a jobject as documented above, or 0.
   *
   * @throws std::std_alloc If memory cannot be allocated
   *
   */
  static void* malloc(void *allocator, size_t requested_size);
};
}}}
#endif /* JNIMEMORYMANAGER_H_ */
