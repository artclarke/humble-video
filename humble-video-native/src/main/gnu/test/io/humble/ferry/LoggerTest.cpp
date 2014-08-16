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

#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>
#include "LoggerTest.h"

using namespace VS_CPP_NAMESPACE;

VS_LOG_SETUP(VS_CPP_PACKAGE);

void
LoggerTestSuite :: testOutputToStandardError()
{
  // Temporarily turn down logging to make a quiet output
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_ERROR, false);
  
  VS_LOG_ERROR("This is an error message");
  VS_LOG_WARN("This is a warning message");
  VS_LOG_INFO("This is an info message");
  VS_LOG_DEBUG("This is a debug message");
  VS_LOG_TRACE("This is a trace message");

  TSM_ASSERT("this test really just shouldn't crash.  check log files to ensure it outputted", true);
}

