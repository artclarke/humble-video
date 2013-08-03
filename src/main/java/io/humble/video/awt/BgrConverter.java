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

import io.humble.ferry.Buffer;
import io.humble.ferry.JNIReference;
import io.humble.video.MediaPicture;
import io.humble.video.PixelFormat;

import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;
import java.awt.image.DataBufferByte;
import java.awt.image.DataBuffer;
import java.awt.image.ColorModel;
import java.awt.color.ColorSpace;
import java.awt.image.ComponentColorModel;
import java.awt.image.SampleModel;
import java.awt.image.PixelInterleavedSampleModel;
import java.awt.image.WritableRaster;
import java.awt.image.Raster;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.util.concurrent.atomic.AtomicReference;

/**
 * A converter to translate {@link MediaPicture}s to and from
 * {@link BufferedImage}s of type {@link BufferedImage#TYPE_3BYTE_BGR}.
 */

public class BgrConverter extends AConverter {
  // band offsets requried by the sample model

  private static final int[] mBandOffsets = { 2, 1, 0 };

  // color space for this converter

  private static final ColorSpace mColorSpace = ColorSpace
      .getInstance(ColorSpace.CS_sRGB);

  // a private copy we use as the resample buffer when converting back and forth. saves time.
  private MediaPicture mResampleMediaPicture;

  /**
   * Construct as converter to translate {@link MediaPicture}s to and from
   * {@link BufferedImage}s of type {@link BufferedImage#TYPE_3BYTE_BGR}.
   * 
   * @param pictureType
   *          the picture type recognized by this converter
   * @param pictureWidth
   *          the width of pictures
   * @param pictureHeight
   *          the height of pictures
   * @param imageWidth
   *          the width of images
   * @param imageHeight
   *          the height of images
   */

  public BgrConverter(PixelFormat.Type pictureType, int pictureWidth,
      int pictureHeight, int imageWidth, int imageHeight) {
    super(pictureType, PixelFormat.Type.PIX_FMT_BGR24,
        BufferedImage.TYPE_3BYTE_BGR, pictureWidth, pictureHeight, imageWidth,
        imageHeight);
    mResampleMediaPicture = willResample() ? MediaPicture.make(imageWidth,
        imageHeight, getRequiredPictureType()) : null;
  }

  /** {@inheritDoc} */
  @Override
  public MediaPicture toPicture(MediaPicture output,
      final BufferedImage input, long timestamp) {
    // validate the image

    validateImage(input);
    
    if (output == null) {
      output = MediaPicture.make(mPictureWidth,
          mPictureHeight, getPictureType());
    }

    // get the image byte buffer buffer

    DataBuffer imageBuffer = input.getRaster().getDataBuffer();
    byte[] imageBytes = null;
    int[] imageInts = null;

    // handle byte buffer case

    if (imageBuffer instanceof DataBufferByte) {
      imageBytes = ((DataBufferByte) imageBuffer).getData();
    }

    // handel integer buffer case

    else if (imageBuffer instanceof DataBufferInt) {
      imageInts = ((DataBufferInt) imageBuffer).getData();
    }

    // if it's some other type, throw

    else {
      throw new IllegalArgumentException(
          "Unsupported BufferedImage data buffer type: "
              + imageBuffer.getDataType());
    }

    // create the video picture and get it's underlying buffer

    final AtomicReference<JNIReference> ref = new AtomicReference<JNIReference>(
        null);
    final MediaPicture picture = willResample() ? mResampleMediaPicture : output;
    try {
      Buffer buffer = picture.getData(0);
      int size = picture.getDataPlaneSize(0);
      ByteBuffer pictureByteBuffer = buffer.getByteBuffer(0,
          size, ref);
      buffer.delete();
      buffer = null;

      if (imageInts != null) {
        pictureByteBuffer.order(ByteOrder.BIG_ENDIAN);
        IntBuffer pictureIntBuffer = pictureByteBuffer.asIntBuffer();
        pictureIntBuffer.put(imageInts);
      } else {
        pictureByteBuffer.put(imageBytes);
      }
      pictureByteBuffer = null;
      picture.setTimeStamp(timestamp);
      picture.setComplete(true);

      // resample as needed
      if (willResample()) {
        resample(output, picture, mToPictureResampler);
      }
      return output;
    } finally {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  /** {@inheritDoc} */
  @Override
  public BufferedImage toImage(BufferedImage output, final MediaPicture input) {
    validatePicture(input);
    // test that the picture is valid
    if (output == null) {
      final byte[] bytes = new byte[willResample() ? mResampleMediaPicture.getDataPlaneSize(0) : input.getDataPlaneSize(0)];
      // create the data buffer from the bytes
      
      final DataBufferByte db = new DataBufferByte(bytes, bytes.length);
      
      // create an a sample model which matches the byte layout of the
      // image data and raster which contains the data which now can be
      // properly interpreted
      int w = mImageWidth;
      int h = mImageHeight;
      
      final SampleModel sm = new PixelInterleavedSampleModel(
        db.getDataType(), w, h, 3, 3 * w, mBandOffsets);
      final WritableRaster wr = Raster.createWritableRaster(sm, db, null);
      
      // create a color model
      
      final ColorModel colorModel = new ComponentColorModel(
        mColorSpace, false, false, ColorModel.OPAQUE, db.getDataType());
      
      // return a new image created from the color model and raster
      
      output = new BufferedImage(colorModel, wr, false, null);
    }

    MediaPicture picture;
    // resample as needed
    AtomicReference<JNIReference> ref = new AtomicReference<JNIReference>(null);
    try {
      if (willResample()) {
        picture = resample(mResampleMediaPicture, input, mToImageResampler);
      } else {
        picture = input;
      }

      final Buffer buffer = picture.getData(0);
      final int size = picture.getDataPlaneSize(0);
      final ByteBuffer byteBuf = buffer.getByteBuffer(0,
          size, ref);
      buffer.delete();

      // get the bytes out of the image
      final DataBufferByte db = (DataBufferByte) output.getRaster()
          .getDataBuffer();
      final byte[] bytes = db.getData();
      // and copy them in.
      byteBuf.get(bytes, 0, size);

      // return a new image created from the color model and raster

      return output;
    } finally {
      if (ref.get() != null)
        ref.get().delete();
    }
  }

  public void delete() {
    if (mResampleMediaPicture != null)
      mResampleMediaPicture.delete();
    mResampleMediaPicture = null;
    
    super.delete();
  }
}
