/*******************************************************************************
 * Copyright (c) 2014, Art Clarke.  All rights reserved.
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
package io.humble.video.demos;

import java.io.File;
import java.io.IOException;
import java.net.URL;

import org.junit.Test;

/**
 * @author aclarke
 *
 */
public class ContainerSegmenterTest {

  /**
   * Test method for {@link io.humble.video.demos.ContainerSegmenter#main(java.lang.String[])}.
   */
  @Test
  public void testMain() throws InterruptedException, IOException {
    final URL s = this.getClass().getResource("/testfile_h264_mp4a_tmcd.mov");
    final String f = new File(s.getPath()).getPath();
    
    final String[] args = new String[]{
        "-vf", "h264_mp4toannexb", // pass a video bit stream filter in
        f,
        this.getClass().getCanonicalName()+".testMain.m3u8"};
    ContainerSegmenter.main(args);
  }

}
