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

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GraphicsEnvironment;
import java.awt.Image;
import java.awt.image.BufferedImage;

import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.SwingUtilities;

/**
 * Displays a {@link BufferedImage} in a {@link JFrame}. Helpful for
 * building quick and simple video players.
 * 
 * @author aclarke
 *
 */
public class ImageFrame extends JFrame
{

  /**
   * To avoid a warning... 
   */

  private static final long serialVersionUID = -4752966848100689153L;
  private final ImageComponent mOnscreenPicture;
  
  private static boolean mIsHeadless = GraphicsEnvironment.isHeadless();

  /**
   * Create the frame
   */

  public static ImageFrame make() {
    return mIsHeadless ? null : new ImageFrame();
  }
  private ImageFrame()
  {
    super();
    mOnscreenPicture = new ImageComponent();
    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    getContentPane().add(mOnscreenPicture);
    this.setVisible(true);
    this.pack();
  }
  
  public void setImage(final Image aImage)
  {
    mOnscreenPicture.setImage(aImage);
  }

  public class ImageComponent extends JComponent
  {
    /**
     * yeah... good idea to add this.
     */
    private static final long serialVersionUID = 5584422798735147930L;
    private Image mImage;
    private Dimension mSize;

    public void setImage(Image image)
    {
      SwingUtilities.invokeLater(new ImageRunnable(image));
    }
    
    public void setImageSize(Dimension newSize)
    {
    }
    
    private class ImageRunnable implements Runnable
    {
      private final Image newImage;

      public ImageRunnable(Image newImage)
      {
        super();
        this.newImage = newImage;
      }
  
      public void run()
      {
        ImageComponent.this.mImage = newImage;
        final Dimension newSize = new Dimension(mImage.getWidth(null), 
          mImage.getHeight(null));
        if (!newSize.equals(mSize))
        {
          ImageComponent.this.mSize = newSize;
          ImageFrame.this.setSize(mImage.getWidth(null), mImage.getHeight(null));
          ImageFrame.this.setVisible(true);
        }
        repaint();
      }
    }
    
    public ImageComponent()
    {
      mSize = new Dimension(0, 0);
      setSize(mSize);
    }

    public synchronized void paint(Graphics g)
    {
      if (mImage != null)
        g.drawImage(mImage, 0, 0, this);
    }
  }
}
