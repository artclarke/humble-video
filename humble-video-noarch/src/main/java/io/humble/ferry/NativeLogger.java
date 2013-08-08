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

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.humble.ferry.NativeLogger;

/**
 * Internal Only.
 * <p>
 * This object is called by Native code when it logs inside a JVM. It forwards
 * to our SLF4J logger.
 * </p>
 * <p>
 * <strong>Do not change the signatures of methods in this class without
 * changing the corresponding native code that depends on it.</strong>
 * </p>
 * 
 */
public final class NativeLogger
{
  // This is a logger this class can use for its own logging
  private static final Logger log = LoggerFactory.getLogger(NativeLogger.class);

  public static NativeLogger getLogger(String loggerName)
  {
    log.trace("Native code asked for logger: {}", loggerName);
    NativeLogger retval = null;
    Logger logger = LoggerFactory.getLogger(loggerName);
    if (logger != null)
      retval = new NativeLogger(logger);
    return retval;
  }

  private final Logger mLogger;

  private NativeLogger(Logger logger)
  {
    mLogger = logger;
  }

  public String getName()
  {
    return mLogger.getName();
  }

  public boolean log(int level, String msg)
  {
    boolean didLog = false;
    log.trace("log level: {}, msg: {}", level, msg);

    switch (level)
    {
      case 1:
        if (mLogger.isWarnEnabled())
        {
          mLogger.warn(msg);
          didLog = true;
        }
        break;
      case 2:
        if (mLogger.isInfoEnabled())
        {
          mLogger.info(msg);
          didLog = true;
        }
        break;
      case 3:
        if (mLogger.isDebugEnabled())
        {
          mLogger.debug(msg);
          didLog = true;
        }
        break;
      case 4:
        if (mLogger.isTraceEnabled())
        {
          mLogger.trace(msg);
          didLog = true;
        }
        break;
      case 0:
      default: // treat default as Error
        if (mLogger.isErrorEnabled())
        {
          mLogger.error(msg);
          didLog = true;
        }
        break;

    }

    return didLog;
  }

}
