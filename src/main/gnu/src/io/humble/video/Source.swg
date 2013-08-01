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

%typemap (javacode) io::humble::video::Source,io::humble::video::Source*,io::humble::video::Source& %{
%}

/**
 * Tell users which methods can block (and hence, can be interrupted).
 */
%javaexception("java.lang.InterruptedException") io::humble::video::Source::open;
%javaexception("java.lang.InterruptedException") io::humble::video::Source::read;
%javaexception("java.lang.InterruptedException") io::humble::video::Source::seek;
%javaexception("java.lang.InterruptedException") io::humble::video::Source::play;
%javaexception("java.lang.InterruptedException") io::humble::video::Source::pause;
%javaexception("java.lang.InterruptedException") io::humble::video::Source::close;
%javaexception("java.lang.InterruptedException") io::humble::video::Source::getNumStreams;
%javaexception("java.lang.InterruptedException") io::humble::video::Source::getStream;
%javaexception("java.lang.InterruptedException") io::humble::video::Source::getSourceStream;
%javaexception("java.lang.InterruptedException") io::humble::video::Source::queryStreamMetaData;

%include <io/humble/video/Source.h>
