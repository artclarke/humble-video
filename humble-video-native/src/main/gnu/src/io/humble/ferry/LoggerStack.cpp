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

#include "LoggerStack.h"
#include "Logger.h"

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace ferry
  {

  LoggerStack :: LoggerStack()
  {
    VS_LOG_TRACE("Creating LoggerStack");
    for(int i = 0; i < 5; i++)
    {
      mOrigLevel[i] = Logger::isGlobalLogging((Logger::Level)i);
      mHasChangedLevel[i] = false;
    }
  }

  LoggerStack :: ~LoggerStack()
  {
    for(int i = 0; i<5; i++)
    {
      if (mHasChangedLevel[i])
        Logger::setGlobalIsLogging((Logger::Level)i, mOrigLevel[i]);

    }
    VS_LOG_TRACE("Destroying LoggerStack");
  }

  void
  LoggerStack :: setGlobalLevel(Logger::Level level, bool value)
  {
    // Set all levels up to an include this level to value.
    mHasChangedLevel[level] = true;
    Logger::setGlobalIsLogging(level, value);
    for(int i = level; i < 5; i++)
    {
      mHasChangedLevel[i] = true;
      if (value)
      {
        Logger::setGlobalIsLogging((Logger::Level)i, mOrigLevel[i]);
      } else {
        Logger::setGlobalIsLogging((Logger::Level)i, value);
      }
    }
  }

  }}}
