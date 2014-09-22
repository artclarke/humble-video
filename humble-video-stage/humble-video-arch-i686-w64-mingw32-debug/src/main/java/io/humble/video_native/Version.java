/*
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
 */
package io.humble.video_native;

/**
 * A class for finding out version information about this jar.
 * @author aclarke
 *
 */
public class Version {
  /** Private class; cannot construct */
  private Version() {};

  /**
   * Prints the version of this library to System.out along with
   * some information on what this jar is.
   */
  public static String getVersionInfo() {
    final Class<?> c = Version.class;
    final Package p = c.getPackage();
    final StringBuilder b = new StringBuilder();
    
    b.append("Package: " + p.getName() + ";");
    b.append("Class: " + c.getName() + ";");
    b.append("Specification Vendor: " + p.getSpecificationVendor() + ";");
    b.append("Specification Title: " + p.getSpecificationTitle() + ";");
    b.append("Specification Version: " + p.getSpecificationVersion() + ";");
    b.append("Implementation Vendor: " + p.getImplementationVendor() + ";");
    b.append("Implementation Title: " + p.getImplementationTitle() + ";");
    b.append("Implementation Version: " + p.getImplementationVersion() + ";");
    
    return b.toString();
  }

}

