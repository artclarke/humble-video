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
package io.humble.video.awt;

import io.humble.video.PixelFormat;
import java.awt.image.BufferedImage;
import io.humble.video.MediaPicture;

/** This interface describes a converter which can perform bidirectional
 * translation between a given {@link MediaPicture} type and a {@link
 * BufferedImage} type.  Converters are created by {@link
 * ConverterFactory}.  Each converter can translate between any
 * supported {@link com.xuggle.xuggler.IPixelFormat.Type} and a single
 * {@link BufferedImage} type.  Converters can optionally resize images
 * during
 * the conversion process.
 */

public interface Converter
{
  /** Get the picture type, as defined by {@link PixelFormat.Type}, which this converter
   * recognizes.
   * 
   * @return the picture type of this converter.
   *
   * @see com.xuggle.xuggler.IPixelFormat.Type
   */

  public PixelFormat.Type getPictureType();

  /** Get the image type, as defined by {@link BufferedImage}, which
   * this converter recognizes.
   * 
   * @return the image type of this converter.
   *
   * @see BufferedImage
   */

  public int getImageType();

  /** Test if this converter is going to re-sample during conversion.
   * For some conversions between {@link BufferedImage} and {@link
   * MediaPicture}, the MediaPicture will need to be re-sampled into
   * another pixel type, commonly between YUV and RGB types.  This
   * re-sample is time consuming, and may not be available for
   * all installations of Humble Video.
   * 
   * @return true if this converter will re-sample during conversion.
   * 
   * @see com.xuggle.xuggler.IVideoResampler
   * @see com.xuggle.xuggler.IVideoResampler#isSupported(com.xuggle.xuggler.IVideoResampler.Feature)
   */

  public boolean willResample();

  /** Converts a {@link BufferedImage} to an {@link MediaPicture}.
   *
   * @param image the source buffered image.
   * @param timestamp the time stamp which should be attached to the the
   *        video picture (in microseconds).
   *
   * @throws IllegalArgumentException if the passed {@link
   *         BufferedImage} is NULL;
   * @throws IllegalArgumentException if the passed {@link
   *         BufferedImage} is not the correct type. See {@link
   *         #getImageType}.
   * @throws IllegalArgumentException if the underlying data buffer of
   *         the {@link BufferedImage} is composed elements other bytes
   *         or integers.
   */

  public MediaPicture toPicture(BufferedImage image, long timestamp);
  
  /** Converts a {@link BufferedImage} to an {@link MediaPicture}.
  *
  * @param output the image to write the output to. any data previous in output will be overritten.
  * @param image the source buffered image.
  * @param timestamp the time stamp which should be attached to the the
  *        video picture (in microseconds).
  *        
  * @returns output is returned (nice for chaining/reuse).
  *
  * @throws IllegalArgumentException if the passed {@link
  *         BufferedImage} is NULL;
  * @throws IllegalArgumentException if the passed {@link
  *         BufferedImage} is not the correct type. See {@link
  *         #getImageType}.
  * @throws IllegalArgumentException if the underlying data buffer of
  *         the {@link BufferedImage} is composed elements other bytes
  *         or integers.
  */
  public MediaPicture toPicture(MediaPicture output, BufferedImage image, long timestamp);

  /** Converts an {@link MediaPicture} to a {@link BufferedImage}.
   *
   * @param picture the source video picture.
   *
   * @throws IllegalArgumentException if the passed {@link
   *         MediaPicture} is NULL;
   * @throws IllegalArgumentException if the passed {@link
   *         MediaPicture} is not the correct type. See {@link
   *         #getPictureType}.
   */

  public BufferedImage toImage(MediaPicture picture);



  /** Converts an {@link MediaPicture} to a {@link BufferedImage}.
   *
   * @param picture the source video picture.
   *
   * @returns output is returned (nice for chaining/reuse).
   * 
   * @throws IllegalArgumentException if the passed {@link
   *         MediaPicture} is NULL;
   * @throws IllegalArgumentException if the passed {@link
   *         MediaPicture} is not the correct type. See {@link
   *         #getPictureType}.
   */

  public BufferedImage toImage(BufferedImage output, MediaPicture picture);
  
  /** Return a written description of the converter. 
   *
   * @return a detailed description of what this converter does.
   */

  public String getDescription();
  
  /**
   * Release any resources used by this converter.  Calls to
   * the converter after this call are illegal and may fail
   * in unspecified ways.  
   */
  public void delete();
}
