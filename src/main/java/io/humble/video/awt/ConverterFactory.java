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
import io.humble.video.PixelFormat;

import java.util.Map;
import java.util.HashMap;
import java.util.Collection;
import java.util.Collections;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

import java.awt.image.BufferedImage;

/**
 * This factory class creates {@link Converter} objects for
 * translation between a
 * number of {@link MediaPicture} and {@link BufferedImage} types.  Not
 * all image and picture types are supported.  When an unsupported
 * converter is requested, a descriptive {@link
 * UnsupportedOperationException} is thrown.
 * <p>  Each converter can
 * translate between any supported {@link io.humble.video.PixelFormat.Type}
 *  and a single {@link
 * BufferedImage} type.  Converters can optionally resize during the
 * conversion process.
 * </p>
 * <p>
 * Each converter will re-sample the {@link MediaPicture} it is converting
 * to get the data in the same native byte layout as a {@link BufferedImage}
 * if needed (using {@link MediaPictureResampler}).  This step takes time and creates
 * a new temporary copy of the image data.  To avoid this step
 * you can make sure the {@link MediaPicture#getPixelType()} is of the same
 * binary type as a {@link BufferedImage}.
 * </p>
 * <p>  Put another way, if you're
 * converting to {@link BufferedImage#TYPE_3BYTE_BGR}, we will not resample
 * if the {@link MediaPicture#getPixelType()} is
 *  {@link io.humble.video.PixelFormat.Type#BGR24}. 
 * </p>
 */

public class ConverterFactory
{
  /**
   * Default constructor
   */
  protected ConverterFactory() {
    
  }

  /** Converts between MediaPictures and {@link BufferedImage} of type
   * {@link BufferedImage#TYPE_3BYTE_BGR} */

  public static final String HUMBLE_BGR_24 = "HUMBLE-BGR-24";

  // the registered converter types
  
  private static Map<String, Type> mConverterTypes = new HashMap<String, Type>();

  // register the known converters

  static
  {
    registerConverter(new Type(HUMBLE_BGR_24, BgrConverter.class, 
        PixelFormat.Type.PIX_FMT_BGR24, BufferedImage.TYPE_3BYTE_BGR));
  }

  /**
   * Register a converter with this factory. The factory oragnizes the
   * converters by descriptor, and thus each should be unique unless one
   * whishes to replace an existing converter.
   *
   * @param converterType the type for the converter to be registered
   *
   * @return the converter type which this converter displaced, or NULL
   * of this is a novel converter
   */

  public static Type registerConverter(Type converterType)
  {
    return mConverterTypes.put(converterType.getDescriptor(), converterType);
  }

  /**
   * Unregister a converter with this factory.
   *
   * @param converterType the type for the converter to be unregistered
   *
   * @return the converter type which removed, or NULL of this converter
   * was not recognized
   */

  public static Type unregisterConverter(Type converterType)
  {
    return mConverterTypes.remove(converterType.getDescriptor());
  }

  /**
   * Get a collection of the registered converters.  The collection is unmodifiable.
   *
   * @return an unmodifiable collection of converter types.
   */

  public static Collection<Type> getRegisteredConverters()
  {
    return Collections.unmodifiableCollection(mConverterTypes.values());
  }

  /**
   * Find a converter given a type descriptor.
   *
   * @param descriptor a unique string which describes this converter
   * 
   * @return the converter found or NULL if it was not found.
   */

  public static Type findRegisteredConverter(String descriptor)
  {
    return mConverterTypes.get(descriptor);
  }

  /**
   * Find a descriptor given a {@link BufferedImage}.
   *
   * @param image a buffered image for which to find a descriptor
   * 
   * @return the descriptor which matches the image or NULL if it was
   *         not found
   */

  public static String findDescriptor(BufferedImage image)
  {
    for (Type converterType: getRegisteredConverters())
      if (converterType.getImageType() == image.getType())
        return converterType.getDescriptor();

    return null;
  }

  /** 
   * Create a converter which translates betewen {@link BufferedImage}
   * and {@link MediaPicture} types.  The {@link
   * io.humble.video.PixelFormat.Type} and size are extracted from
   * the passed in picture.  This factory will attempt to create a
   * converter which can perform the translation.  If no converter can
   * be created, a descriptive {@link UnsupportedOperationException} is
   * thrown.
   *
   * @param converterDescriptor the unique string descriptor of the
   *        converter which is to be created
   * @param picture the picture from which size and type are extracted
   *
   * @throws UnsupportedOperationException if the converter can not be
   *         found
   * @throws UnsupportedOperationException if the found converter can
   *         not be properly initialized
   * @throws IllegalArgumentException if the passed {@link
   *         MediaPicture} is NULL;
   */

  public static Converter createConverter(
    String converterDescriptor,
    MediaPicture picture)
  {
    if (picture == null)
      throw new IllegalArgumentException("The picture is NULL.");

    return createConverter(converterDescriptor, picture.getFormat(), 
      picture.getWidth(), picture.getHeight());
  }

  /** 
   * Create a converter which translates betewen {@link BufferedImage}
   * and {@link MediaPicture} types. The {@link BufferedImage} type and
   * size are extracted from the passed in image.  This factory will
   * attempt to create a converter which can perform the translation.
   * If no converter can be created, a descriptive {@link
   * UnsupportedOperationException} is thrown.
   *
   * @param image the image from which size and type are extracted
   * @param pictureType the picture type of the converter
   *
   * @throws UnsupportedOperationException if no converter for the
   *         specifed BufferedImage type exists
   * @throws UnsupportedOperationException if the found converter can
   *         not be properly initialized
   * @throws IllegalArgumentException if the passed {@link
   *         BufferedImage} is NULL;
   */

  public static Converter createConverter(
    BufferedImage image,
    PixelFormat.Type pictureType)
  {
    if (image == null)
      throw new IllegalArgumentException("The image is NULL.");

    // find the converter type based in image type

    String converterDescriptor = findDescriptor(image);
    if (converterDescriptor == null)
      throw new UnsupportedOperationException(
        "No converter found for BufferedImage type #" + 
        image.getType());

    // create and return the converter

    return createConverter(converterDescriptor, pictureType,
      image.getWidth(), image.getHeight());
  }

  /** 
   * Create a converter which translates betewen {@link BufferedImage}
   * and {@link MediaPicture} types.  This factory will attempt to
   * create a converter which can perform the translation.  If no
   * converter can be created, a descriptive {@link
   * UnsupportedOperationException} is thrown.
   *
   * @param converterDescriptor the unique string descriptor of the
   *        converter which is to be created
   * @param pictureType the picture type of the converter
   * @param width the width of pictures and images
   * @param height the height of pictures and images
   *
   * @throws UnsupportedOperationException if the converter can not be
   *         found
   * @throws UnsupportedOperationException if the found converter can
   *         not be properly initialized
   */

  public static Converter createConverter(
    String converterDescriptor,
    PixelFormat.Type pictureType, 
    int width, int height)
  {
    return createConverter(converterDescriptor, pictureType, 
      width, height, width, height);
  }

  /** 
   * Create a converter which translates betewen {@link BufferedImage}
   * and {@link MediaPicture} types.  This factory will attempt to
   * create a converter which can perform the translation.  If different
   * image and pictures sizes are passed the converter will resize
   * during translation.  If no converter can be created, a descriptive
   * {@link UnsupportedOperationException} is thrown.
   *
   * @param converterDescriptor the unique string descriptor of the
   *        converter which is to be created
   * @param pictureType the picture type of the converter
   * @param pictureWidth the width of pictures
   * @param pictureHeight the height of pictures
   * @param imageWidth the width of images
   * @param imageHeight the height of images
   *
   * @throws UnsupportedOperationException if the converter can not be
   *         found
   * @throws UnsupportedOperationException if the converter can not be
   *         properly created or initialized
   */

  public static Converter createConverter(
    String converterDescriptor,
    PixelFormat.Type pictureType, 
    int pictureWidth, int pictureHeight,
    int imageWidth, int imageHeight)
  {
    Converter converter = null;
    
    // establish the converter type

    Type converterType = findRegisteredConverter(converterDescriptor);
    if (null == converterType)
      throw new UnsupportedOperationException(
        "No converter \"" + converterDescriptor + "\" found.");

    // create the converter

    try
    {
      // establish the constructor 

      Constructor<? extends Converter> converterConstructor = 
        converterType.getConverterClass().getConstructor(PixelFormat.Type.class,
          int.class, int.class, int.class, int.class);

      // create the converter

      converter = converterConstructor.newInstance(
        pictureType, pictureWidth, pictureHeight, imageWidth, imageHeight);
    }
    catch (NoSuchMethodException e)
    {
      throw new UnsupportedOperationException(
        "Converter " + converterType.getConverterClass() + 
        " requries a constructor of the form "  +
        "(PixelFormat.Type, int, int, int, int)");
    }
    catch (InvocationTargetException e)
    {
      Throwable cause = e.getCause();
      if (cause != null && cause instanceof OutOfMemoryError)
      {
        throw (OutOfMemoryError)cause;
      }
      else
      {
        throw new UnsupportedOperationException(
            "Converter " + converterType.getConverterClass() + 
            " constructor failed with: " + e.getCause());
      }
    }
    catch (IllegalAccessException e)
    {
      Throwable cause = e.getCause();
      if (cause != null && cause instanceof OutOfMemoryError)
      {
        throw (OutOfMemoryError)cause;
      }
      else
      {
        throw new UnsupportedOperationException(
            "Converter " + converterType.getConverterClass() + 
            " constructor failed with: " + e.getCause());
      }
    }
    catch (InstantiationException e)
    {
      Throwable cause = e.getCause();
      if (cause != null && cause instanceof OutOfMemoryError)
      {
        throw (OutOfMemoryError)cause;
      }
      else
      {
        throw new UnsupportedOperationException(
            "Converter " + converterType.getConverterClass() + 
            " constructor failed with: " + e.getCause());
      }
    }

    // return the newly created converter

    return converter;
  }


  /**
   * This class describes a converter type and is used to register and
   * unregister types with {@link ConverterFactory}.  The factory
   * oragnizes the converters by descriptor, and thus each should be
   * unique unless you wish to replace an existing converter.
   */

  public static class Type 
  {
    /** The unique string which describes this converter. */

    final private String mDescriptor;

    /** The class responsible for converting between types. */

    final private Class<? extends Converter> mConverterClass;

    /**
     * The {@link io.humble.video.PixelFormat.Type} which the
     * picture must be in to convert it to a {@link BufferedImage}
     */

    final private PixelFormat.Type mPictureType;

    /**
     * The {@link BufferedImage} type which the image must be in to
     * convert it to a {@link io.humble.video.MediaPicture}
     */

    final private int mImageType;

    /** Construct a complete converter type description.
     *
     * @param descriptor a unique string which describes this converter
     * @param converterClass the class which converts between pictures
     *        and images
     * @param pictureType the {@link
     *        io.humble.video.PixelFormat.Type} type which the
     *        picture must be in to convert it to an image
     * @param imageType the {@link BufferedImage} type which the picture
     *        must be in to convert it to a {@link BufferedImage}
     */

    public Type(String descriptor, Class<? extends Converter> converterClass, 
      PixelFormat.Type pictureType, int imageType)
    {
      mDescriptor = descriptor;
      mConverterClass = converterClass;
      mPictureType = pictureType;
      mImageType = imageType;
    }

    /** Get the unique string which describes this converter. */

    public String getDescriptor()
    {
      return mDescriptor;
    }

    /** Get the class responsible for converting between types. */

    public Class<? extends Converter> getConverterClass()
    {
      return mConverterClass;
    }

    /**
     * Get the {@link io.humble.video.PixelFormat.Type} which the
     * picture must be in to convert it to a {@link BufferedImage}
     */

    public PixelFormat.Type getPictureType()
    {
      return mPictureType;
    }

    /**
     * Get the {@link BufferedImage} type which the image must be in to
     * convert it to a {@link io.humble.video.MediaPicture}
     */

    public int getImageType()
    {
      return mImageType;
    }

    /**
     * Get a string description of this conveter type.
     */

    public String toString()
    {

      return getDescriptor() + ": picture type " + getPictureType() +
        ", image type " + getImageType();
    }
  }

  /** 
   * Convert a {@link BufferedImage} of any type, to {@link
   * BufferedImage} of a specified type.  If the source image is the
   * same type as the target type, then original image is returned,
   * otherwise new image of the correct type is created and the content
   * of the source image is copied into the new image.
   *
   * @param sourceImage the image to be converted
   * @param targetType the desired BufferedImage type 
   *
   * @return a BufferedImage of the specifed target type.
   *
   * @see BufferedImage
   */

  public static BufferedImage convertToType(BufferedImage sourceImage, 
    int targetType)
  {
    BufferedImage image;

    // if the source image is already the target type, return the source image

    if (sourceImage.getType() == targetType)
      image = sourceImage;

    // otherwise create a new image of the target type and draw the new
    // image 

    else
    {
      image = new BufferedImage(sourceImage.getWidth(), sourceImage.getHeight(),
        targetType);
      image.getGraphics().drawImage(sourceImage, 0, 0, null);
    }
    
    return image;
  }
}
