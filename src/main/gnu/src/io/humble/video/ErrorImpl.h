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
 */

#ifndef ERRORIMPL_H_
#define ERRORIMPL_H_

#include <io/humble/video/Error.h>

namespace io { namespace humble { namespace video
{

class ErrorImpl : public Error
{
  VS_JNIUTILS_REFCOUNTED_OBJECT_PRIVATE_MAKE(ErrorImpl);
public:
  virtual const char* getDescription();
  virtual int32_t getErrorNumber();
  virtual Type getType();
  
  static ErrorImpl* make(int32_t errNo);
  static ErrorImpl* make(Type type);
  static Type errorNumberToType(int32_t errorNo);
  static int32_t typeToErrorNumber(Type type);
protected:
  ErrorImpl();
  virtual ~ErrorImpl();
  
private:
  static ErrorImpl* make(int32_t errNo, Type type);
  
  Type mType;
  int32_t mErrorNo;
  char mErrorStr[256];
  
};

}}}

#endif /* ERRORIMPL_H_ */
