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
 * InputFormat.cpp
 *
 *  Created on: Jun 29, 2013
 *      Author: aclarke
 */

#include "InputFormat.h"
#include "Global.h"

namespace io {
namespace humble {
namespace video {

InputFormat::InputFormat() : mFormat(0)
{

}

InputFormat::~InputFormat()
{
}

InputFormat*
InputFormat::make(AVInputFormat* f) {
  InputFormat* retval = 0;
  if (f) {
    retval = make();
    retval->mFormat = f;
  }
  return retval;
}


InputFormat *
InputFormat::findFormat(const char *shortName)
{
  Global::init();
  AVInputFormat *f = av_find_input_format(shortName);
  return make(f);
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
