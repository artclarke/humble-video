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

#include <io/humble/ferry/Logger.h>
#include <io/humble/video/ErrorImpl.h>
#include <io/humble/video/FfmpegIncludes.h>

#include <cstring>
#include <stdexcept>

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io { namespace humble { namespace video
{

struct ErrorMappingTable {
  int32_t mFfmpegError;
  Error::Type mHumbleError;
} ;

static struct ErrorMappingTable sErrorMappingTable[] = {
    { AVERROR(EIO),         Error::ERROR_IO },
    { AVERROR(EDOM),        Error::ERROR_NUMEXPECTED },
    { AVERROR(EINVAL),      Error::ERROR_INVALIDDATA },
    { AVERROR(ENOMEM),      Error::ERROR_NOMEM },
    { AVERROR(EILSEQ),      Error::ERROR_NOFMT },
    { AVERROR(ENOSYS),      Error::ERROR_NOTSUPPORTED },
    { AVERROR(ENOENT),      Error::ERROR_NOENT },
    { AVERROR(EPIPE),       Error::ERROR_EOF },
    { AVERROR_PATCHWELCOME, Error::ERROR_PATCHWELCOME },
    { AVERROR(EAGAIN),      Error::ERROR_AGAIN },
    { AVERROR(ERANGE),      Error::ERROR_RANGE },
    { AVERROR(EINTR),       Error::ERROR_INTERRUPTED },
    { AVERROR_EOF,          Error::ERROR_EOF },
    { AVERROR_DECODER_NOT_FOUND, Error::ERROR_NOFMT },
    { AVERROR_DEMUXER_NOT_FOUND, Error::ERROR_NOFMT },
    { AVERROR_ENCODER_NOT_FOUND, Error::ERROR_NOFMT },
    { AVERROR_MUXER_NOT_FOUND, Error::ERROR_NOFMT },
    { AVERROR_OPTION_NOT_FOUND, Error::ERROR_NOFMT },
};
static int32_t sErrorMappingTableSize = sizeof(sErrorMappingTable)/sizeof(struct ErrorMappingTable);

ErrorImpl::ErrorImpl()
{
  mType = Error::ERROR_UNKNOWN;
  mErrorNo = 0;
  *mErrorStr = 0;
  mErrorStr[sizeof(mErrorStr)-1]=0;
}

ErrorImpl::~ErrorImpl()
{
  
}

const char*
ErrorImpl::getDescription()
{
  const char* retval = 0;
  if (!*mErrorStr && mErrorNo != 0)
  {
#ifdef HAVE_STRERROR_R
#ifdef STRERROR_R_CHAR_P
    retval = strerror_r(AVUNERROR(mErrorNo), mErrorStr, sizeof(mErrorStr));
#else
    strerror_r(AVUNERROR(mErrorNo), mErrorStr, sizeof(mErrorStr));
    retval = mErrorStr;
#endif
#else
    retval = strerror(AVUNERROR(mErrorNo));    
#endif // HAVE_STRERROR_R
    if (retval != (const char*) mErrorStr)
      strncpy(mErrorStr, retval, sizeof(mErrorStr)-1);
  }
  return *mErrorStr ? mErrorStr : 0;
}

int32_t
ErrorImpl::getErrorNumber()
{
  return mErrorNo;
}
Error::Type
ErrorImpl::getType()
{
  return mType;
}

ErrorImpl*
ErrorImpl::make(int32_t aErrorNo)
{
  if (aErrorNo >= 0)
    return 0;
  
  return make(aErrorNo, errorNumberToType(aErrorNo));
}

ErrorImpl*
ErrorImpl::make(Type aType)
{
  return make(typeToErrorNumber(aType), aType);
}

ErrorImpl*
ErrorImpl::make(int32_t aErrorNo, Type aType)
{
  ErrorImpl* retval = 0;
  try
  {
    retval = make();
    if (!retval)
      throw std::bad_alloc();
    retval->mErrorNo = aErrorNo;
    retval->mType = aType;
    // null out and don't fill unless description is asked for
    *(retval->mErrorStr) = 0;
  }
  catch (std::bad_alloc & e)
  {
    VS_REF_RELEASE(retval);
    throw e;
  }
  catch (std::exception & e)
  {
    VS_LOG_TRACE("Error: %s", e.what());
    VS_REF_RELEASE(retval);
  }
  return retval;
  
}

Error::Type
ErrorImpl::errorNumberToType(int32_t errNo)
{
  Error::Type retval = Error::ERROR_UNKNOWN;
  int i = 0;
  for(; i < sErrorMappingTableSize; i++)
  {
    if (sErrorMappingTable[i].mFfmpegError == errNo)
    {
      retval = sErrorMappingTable[i].mHumbleError;
      break;
    }
  }
  if (i >= sErrorMappingTableSize) {
    retval = Error::ERROR_UNKNOWN;
  }
  return retval;
}

int32_t
ErrorImpl::typeToErrorNumber(Type type)
{
  int32_t retval = AVERROR(EINVAL);
  int i = 0;
  for(; i < sErrorMappingTableSize; i++)
  {
    if (sErrorMappingTable[i].mHumbleError == type)
    {
      retval = sErrorMappingTable[i].mFfmpegError;
      break;
    }
  }
  if (i >= sErrorMappingTableSize) {
    retval = AVERROR(EINVAL);
  }
  return retval;
}

}}}
