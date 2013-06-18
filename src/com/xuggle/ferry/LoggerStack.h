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

#ifndef LOGGERSTACK_H_
#define LOGGERSTACK_H_

#include <com/xuggle/ferry/Logger.h>

namespace com { namespace xuggle { namespace ferry
  {

  /**
   * This class records the Global logging levels when it is created,
   * and, if it changes the Global logging level, restores it to the
   * original values when it is destroyed.
   *
   * It is handy to use in tests where you know a log (and hence error
   * message) will be generated, but you want to temporarily turn off
   * logging when the test runs.
   */
  class VS_API_FERRY LoggerStack
  {
  public:
    LoggerStack();
    virtual ~LoggerStack();
    /**
     * If false, sets level and all lower levels to false.
     *
     * If true, sets current level to true, and all lower levels to original
     * value.
     *
     * @param level The level to set.
     * @param value Whether to log (true) or not (false).
     *
     */
    void setGlobalLevel(Logger::Level level, bool value);
  private:
    bool mHasChangedLevel[5];
    bool mOrigLevel[5];
  };

  }}}

#endif /*LOGGERSTACK_H_*/
