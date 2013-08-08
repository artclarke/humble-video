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
#include "ErrorTest.h"
#include <io/humble/video/FfmpegIncludes.h>
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>

#include <io/humble/video/Error.h>
#include <cstring>

using namespace io::humble::ferry;

using namespace io::humble::video;;

VS_LOG_SETUP(VS_CPP_PACKAGE);

ErrorTest :: ErrorTest()
{
}

ErrorTest :: ~ErrorTest()
{
}

void
ErrorTest :: testErrorCreation()
{
  RefPointer<Error> error = Error::make(AVERROR(EIO));
  
  TSM_ASSERT("made error", error);
  
  const char* description = error->getDescription();
  
  TSM_ASSERT("got description", description && *description);

  TSM_ASSERT_EQUALS("should map correctly", 
      Error::ERROR_IO,
      error->getType());
  
  error = Error::make(Error::ERROR_IO);
  TSM_ASSERT("made error", error);
    
  description = error->getDescription();

  TSM_ASSERT("got description", description && *description);

  TSM_ASSERT_EQUALS("should map correctly", 
      AVERROR(EIO),
      error->getErrorNumber());
  
}
