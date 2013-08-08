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

package io.humble.ferry;

/**
 * Internal Only.
 * 
 * This class is held on to by the {@link RefCounted} classes and nulled when
 * that object is collected. It has no references to any (non static) members
 * and so its {@link #finalize()} method will not hold up the collection of any
 * other object.
 * <p>
 * It exists so that we still have a mechanism that always frees native memory;
 * in most cases the {@link JNIReference} will enqueue correctly with the
 * {@link JNIMemoryManager}, and then the next call to a Ferry based method will
 * drain that queue, but sometimes there is no extra call to one of those
 * methods; in this case we'll drain the queue when this gets finalized.
 * </p>
 * <p>
 * It does a {@link JNIMemoryManager#gc()} which might race with the
 * {@link JNIMemoryManager#gc()} that a {@link JNIReference} does on allocation
 * of a new object but that's a safe race.
 * </p>
 * 
 * @author aclarke
 * 
 */
public final class JNINativeFinalizer
{
  /**
   * Internal Only. Creates a new {@link JNINativeFinalizer}. This object must
   * contain <strong>no references</strong> to any other objects in the system.
   */
  public JNINativeFinalizer()
  {
  }

  /**
   * Runs a {@link JNIMemoryManager#gc()} to free up any
   * {@link RefCounted} objects that are pending release.
   */
  protected void finalize()
  {
    JNIReference.getMgr().gcInternal();
  }
}
