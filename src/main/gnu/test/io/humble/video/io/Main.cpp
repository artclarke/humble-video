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

#include <cxxtest/ErrorPrinter.h>
#include <io/humble/ferry/JNIHelper.h>
#include <io/humble/video/io/URLProtocolManager.h>

int main() {
 int retval = CxxTest::ErrorPrinter().run();
 // This method is used to clean up static memory
 // to that Valgrind doesn't think I'm a sloppy leaker.
 io::humble::ferry::JNIHelper::shutdownHelper();
 // And unregister all protocols
 io::humble::video::io::URLProtocolManager::unregisterAllProtocols();
 return retval;
}

