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
 * Configuration.java
 *
 *  Created on: Jun 30, 2013
 *      Author: aclarke
 */
package io.humble.video;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Properties;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * A global configuration class for Humble Video
 * <p>
 * This object can help print setting options
 * on {@link Configurable} classes, and also
 * provides a convenient method {@link #configure(Properties, Configurable)}
 * that lets you configure {@link Configurable} objects from
 * a Java properties or FFmpeg preset file.
 * </p> 
 * @see #configure(Properties, Configurable)
 * @see Configurable
 * 
 * @author aclarke
 *
 */
public class Configuration
{
  static private final Logger log = LoggerFactory.getLogger(Configuration.class);

  /** don't allow construction */
  private Configuration() {}
  
  /**
   * Print out all configurable options on the {@link Configurable} object.
   * 
   * @param stream stream to print to
   * @param configObj {@link Configurable} object.
   */
  public static void printConfigurable(java.io.PrintStream stream,
      Configurable configObj)
  {
    stream.println("=======================================");
    stream.println("  " + configObj.getClass().getName() + " Properties");
    stream.println("=======================================");
    int numOptions = configObj.getNumProperties();
    
    for(int i = 0; i < numOptions; i++)
    {
      IProperty prop = configObj.getPropertyMetaData(i);
      printOption(stream, configObj, prop);
    }
   
  }
  
  /**
   * Print information about the property on the configurable object.
   * 
   * @param stream stream to print to
   * @param configObj configurable object
   * @param prop property on object
   */
  public static void printOption(java.io.PrintStream stream,
      Configurable configObj, IProperty prop)
  {
    if (prop.getType() != IProperty.Type.PROPERTY_FLAGS)
    {
      stream.printf("  %s; default= %s; type=%s;\n",
          prop.getName(),
          configObj.getPropertyAsString(prop.getName()),
          prop.getType());
    } else {
      // it's a flag
      stream.printf("  %s; default= %d; valid values=(",
          prop.getName(),
          configObj.getPropertyAsLong(prop.getName()));
      int numSettings = prop.getNumFlagSettings();
      long value = configObj.getPropertyAsLong(prop.getName());
      for(int i = 0; i < numSettings; i++)
      {
        IProperty fprop = prop.getFlagConstant(i);
        long flagMask = fprop.getDefault();
        boolean isSet = (value & flagMask)>0;
        stream.printf("%s%s; ",
            isSet ? "+" : "-",
                fprop.getName());
      }
      stream.printf("); type=%s;\n", prop.getType());
    }
    stream.printf("    help for %s: %s\n",
        prop.getName(),
        prop.getHelp() == null ? "no help available" : prop.getHelp());
  }
  
  /**
   * Configures an {@link Configurable} from a set of Java {@link Properties}.
   * <p>
   * Here's some sample code that shows configuring a IStreamCoder
   * from a FFmpeg preset file:
   * </p>
   * <pre>
   *   Properties props = new Properties();
   *   props.load(new FileInputStream(file));
   *   IStreamCoder coder = IStreamCoder.make(Direction.ENCODING);
   *   int retval = Configuration.configure(props, coder);
   *   if (retval < 0)
   *      throw new RuntimeException("Could not configure object");
   * </pre>
   * @param properties The properties to use.
   * @param config The item to configure.
   * @return <0 on error; >= 0 on success.
   */
  @SuppressWarnings("unchecked")
  public static int configure(final Properties properties, final Configurable config)
  {
    for (
        final Enumeration<String> names = (Enumeration<String>) properties.propertyNames();
        names.hasMoreElements();
    )
    {
      final String name = names.nextElement();
      final String value = properties.getProperty(name);
      if (value != null) {
        final int retval = config.setProperty(name, value);
        if (retval < 0) {
          log.warn("Invalid property on object {}; name=\"{}\"; value=\"{}\"",
              new Object[]{
              config,
              name,
              value
          });
          return retval;
        }
      }
    }
    return 0;
  }
  /**
   * Configures an {@link Configurable} from a file.
   * <p>
   * This is a handy way to configure a Humble Video {@link Configurable}
   * object, and will also work with FFmpeg preset files.
   * </p>
   * @param file A filename for the properties file.
   * @param config The item to configure.
   * @return <0 on error; >= 0 on success.
   */
  @SuppressWarnings("unchecked")
  public static int configure(final String file, final Configurable config)
  {
    Properties props = new Properties();
    try
    {
      props.load(new FileInputStream(file));
    }
    catch (FileNotFoundException e)
    {
      e.printStackTrace();
      return -1;
    }
    catch (IOException e)
    {
      e.printStackTrace();
      return -1;
    }
    return Configuration.configure(props, config);
  }

}
