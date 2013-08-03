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

import io.humble.video.MediaPicture;
import io.humble.video.MediaPictureResampler;
import io.humble.video.PixelFormat;

import java.awt.image.BufferedImage;

/** An abstract converter class from which specific converters can be
 * derived to do the actual conversions.  This class establishes if
 * the {@link MediaPicture} needs to be re-sampled, and
 * if so, creates appropriate {@link MediaPictureResampler} objects to do
 * that.
 */

abstract public class AConverter implements Converter
{
  /** Re-sampler called when converting image to picture, may be null. */

  protected MediaPictureResampler mToPictureResampler = null;
  
  /** Re-sampler called when converting picture to image, may be null. */

  protected MediaPictureResampler mToImageResampler = null;

  /** The width of the pictures. */
 
  protected int mPictureWidth;

  /** The height of the pictures. */

  protected int mPictureHeight;

  /** The width of the images. */

  protected int mImageWidth;

  /** The height of the images. */

  protected int mImageHeight;

  // the recognized BufferedImage type

  final private PixelFormat.Type mPictureType;

  // the recognized BufferedImage type

  final private PixelFormat.Type mRequiredPictureType;

  // the recognized BufferedImage type

  final private int mImageType;

  // the description of this convert

  final private String mDescription;

  /** 
   * Construct an abstract Converter.  This will create a
   * {@link MediaPictureResampler}
   * to change color-space or resize the picture as needed for the
   * conversions specified.
   *
   * @param pictureType the recognized {@link MediaPicture} type
   * @param requiredPictureType the picture type requred to translate to
   *        and from the BufferedImage
   * @param imageType the recognized {@link BufferedImage} type
   * @param pictureWidth the width of picture
   * @param pictureHeight the height of picture
   * @param imageWidth the width of image
   * @param imageHeight the height of image
   */

  public AConverter(
    PixelFormat.Type pictureType, 
    PixelFormat.Type requiredPictureType, 
    int imageType,
    int pictureWidth, 
    int pictureHeight,
    int imageWidth, 
    int imageHeight)
  {
    // by default there is no resample description

    String resampleDescription = "";

    // record the image and picture parameters
    
    mPictureType = pictureType;
    mRequiredPictureType = requiredPictureType;
    mImageType = imageType;
    mPictureWidth = pictureWidth;
    mPictureHeight = pictureHeight;
    mImageWidth = imageWidth;
    mImageHeight = imageHeight;

    // if the picture type is not the type or size required, create the
    // resamplers to fix that

    if (!mPictureType.equals(mRequiredPictureType) 
      || (mPictureWidth != mImageWidth)
      || (mPictureHeight != mImageHeight))
    {

      mToImageResampler = MediaPictureResampler.make(
          mImageWidth, mImageHeight, mRequiredPictureType,
        mPictureWidth, mPictureHeight, mPictureType, 0);
      mToImageResampler.open();

      mToPictureResampler = MediaPictureResampler.make(
        mPictureWidth, mPictureHeight, mPictureType,
        mImageWidth, mImageHeight, mRequiredPictureType, 0);
      mToPictureResampler.open();

      resampleDescription = "Pictures will be resampled to and from " + 
          mRequiredPictureType + " during translation.";
    }

    // construct the description of this converter

    mDescription = "A converter which translates [" +
      pictureWidth + "x" + pictureHeight + "] MediaPicture type " + 
      pictureType + " to and from [" + imageWidth + "x" + imageHeight +
      "] BufferedImage type " + imageType + ".  " + resampleDescription;
  }

  /** {@inheritDoc} */

  public PixelFormat.Type getPictureType()
  {
    return mPictureType;
  }

  /**
   * Return the Type which matches the {@link
   * BufferedImage} type.
   * 
   * @return the picture type which allows for image translation.
   */

  protected PixelFormat.Type getRequiredPictureType()
  {
    return mRequiredPictureType;
  }

  /** {@inheritDoc} */

  public int getImageType()
  {
    return mImageType;
  }

  /** {@inheritDoc} */

  public boolean willResample()
  {
    return null != mToPictureResampler && null != mToImageResampler;
  }

  /** 
   * Re-sample a picture.
   * 
   * @param picture1 the picture to re-sample
   * @param resampler the picture re-samper to use
   *
   * @throws RuntimeException if could not re-sample picture
   **/

  protected static MediaPicture resample(MediaPicture input,
    MediaPictureResampler resampler)
  {
    // create new picture object

    MediaPicture output = MediaPicture.make(
      resampler.getOutputWidth(),
      resampler.getOutputHeight(),
      resampler.getOutputFormat());
    return resample(output, input, resampler);
  }
  protected static MediaPicture resample(MediaPicture output, MediaPicture input, MediaPictureResampler resampler) {
    // resample

    resampler.resample(output,  input);
    
    // return the resample picture

    return output;
    
  }

  /** 
   * Test that the passed image is valid and conforms to the
   * converters specifications.
   *
   * @param image the image to test
   *
   * @throws IllegalArgumentException if the passed {@link
   *         BufferedImage} is NULL;
   * @throws IllegalArgumentException if the passed {@link
   *         BufferedImage} is not the correct type. See {@link
   *         #getImageType}.
   */

  protected void validateImage(BufferedImage image)
  {
    // if the image is NULL, throw up

    if (image == null)
      throw new IllegalArgumentException("The passed image is NULL.");

    // if image is not the correct type, throw up

    if (image.getType() != getImageType())
      throw new IllegalArgumentException(
        "The passed image is of type #" + image.getType() +
        " but is required to be of BufferedImage type #" +
        getImageType() + ".");
  }

  /** 
   * Test that the passed picture is valid and conforms to the
   * converters specifications.
   *
   * @param picture the picture to test
   *
   * @throws IllegalArgumentException if the passed {@link
   *         MediaPicture} is NULL;
   * @throws IllegalArgumentException if the passed {@link
   *         MediaPicture} is not complete.
   * @throws IllegalArgumentException if the passed {@link
   *         MediaPicture} is not the correct type.
   */

  protected void validatePicture(MediaPicture picture)
  {
    // if the picture is NULL, throw up
    
    if (picture == null)
      throw new IllegalArgumentException("The picture is NULL.");

    // if the picture is not complete, throw up
    
    if (!picture.isComplete())
      throw new IllegalArgumentException("The picture is not complete.");

    // if the picture is an invalid type throw up

    PixelFormat.Type type = picture.getFormat();
    if ((type != getPictureType()) && (willResample() && 
        type != mToImageResampler.getOutputFormat()))
      throw new IllegalArgumentException(
        "Picture is of type: " + type + ", but must be " + 
        getPictureType() + (willResample() 
          ? " or " + mToImageResampler.getOutputFormat()
          : "") +
        ".");
  }

  /** {@inheritDoc} */

  public String getDescription()
  {
    return mDescription;
  }

  /** Get a string representation of this converter. */

  public String toString()
  {
    return getDescription();
  }

  public void close()
  {
    if (mToPictureResampler != null)
      mToPictureResampler.delete();
    mToPictureResampler = null;
    if (mToImageResampler != null)
      mToImageResampler.delete();
    mToImageResampler = null;
  }

}
