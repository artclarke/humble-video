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
/*
 * PropertyNotFoundException.java
 *
 *  Created on: Aug 1, 2013
 *      Author: aclarke
 */
package io.humble.video;

/**
 * This exception can be raised by {@link Configurable} objects if an attempt
 * is made to set or get a property that does not exist.
 * 
 * You can get the name of the unfound property from: {@link #getMessage()}.
 * @author aclarke
 *
 */
public class PropertyNotFoundException extends RuntimeException {

  /**
   * Serialization
   */
  private static final long serialVersionUID = 9110553041486456764L;

  /**
   * Create an unknown property not found exception.
   */
  public PropertyNotFoundException() {
    super("unknown");
  }

  /**
   * Create with property name.
   * 
   * @param property properyt name
   */
  public PropertyNotFoundException(String property) {
    super(property);
  }

  /**
   * Create with exception
   * @param e exception
   */
  public PropertyNotFoundException(Throwable e) {
    super(e);
  }

  /**
   * Create with property and exception
   * @param property property name
   * @param e exception
   */
  public PropertyNotFoundException(String property, Throwable e) {
    super(property, e);
  }

}
