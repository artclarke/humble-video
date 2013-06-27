package io.humble.video;

import org.junit.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.humble.video.IRational;

import junit.framework.TestCase;

public class RationalTest extends TestCase
{
  private final Logger log = LoggerFactory.getLogger(this.getClass());

  private IRational mRational=null;

  @Before
  public void setUp()
  {
    log.debug("Executing test case: {}", this.getName());
    if (mRational != null)
      mRational.delete();
    mRational = null;
  }
  
  @Test
  public void testConstructorsAndGetters()
  {
    mRational = IRational.make();    
    assertTrue(mRational.getNumerator() == 0);
    assertTrue(mRational.getDenominator() == 1);
    mRational.delete();
    mRational = null;
    
    mRational = IRational.make(6.0);
    assertTrue(mRational.getNumerator() == 6);
    assertTrue(mRational.getDenominator() == 1);
    mRational.delete();
    mRational = null;

    mRational = IRational.make(6.1);
    assertTrue(mRational.getNumerator() == 61);
    assertTrue(mRational.getDenominator() == 10);
    mRational.delete();
    mRational = null;
  }
  
  @Test
  public void testReduction()
  {
    int retval = -1;
    mRational = IRational.make(2.2);
    
    assertTrue(mRational.getNumerator() == 11);
    assertTrue(mRational.getDenominator() == 5);

    retval = mRational.reduce(mRational.getNumerator()*5,
        mRational.getDenominator()*10, 100);
    assertTrue("not exact reduction", retval ==1);
    assertTrue(mRational.getNumerator()==11);
    assertTrue(mRational.getDenominator()==10);
    
    retval = IRational.sReduce(mRational, 33, 32, 10);
    assertTrue("exact reduction?", retval == 0);
    assertTrue(mRational.getNumerator()==1);
    assertTrue(mRational.getDenominator()==1);

    // A reduction to a den of 0 should be exact
    // but result in an 0 denominator and a 1
    // numerator (i.e. INFINITY).
    retval = mRational.reduce(33, 0, 10);
    assertTrue("exact reduction?", retval == 1);
    assertTrue(mRational.getNumerator()==1);
    assertTrue(mRational.getDenominator()==0);
  }
  
  @Test
  public void testCompareTo()
  {
    int retval = -1;
    IRational otherRational = IRational.make(5);
    mRational = IRational.make(4);

    retval = mRational.compareTo(otherRational);
    assertTrue(retval <0);
    retval = otherRational.compareTo(mRational);
    assertTrue(retval >0);
    retval = mRational.compareTo(mRational);
    assertTrue(retval == 0);
    retval = IRational.sCompareTo(mRational, otherRational);
    assertTrue(retval < 0);
  }
  
  @Test
  public void testGetDouble()
  {
    double retval = -1;
    mRational = IRational.make();
    retval = mRational.getDouble();
    assertTrue(retval == 0);
    
    // Now, let's create an invalid value and see what happens.
    mRational = IRational.make();
    mRational.reduce(1,0, 10);
    retval = mRational.getDouble();
    assertTrue("double is valid: " + retval, Double.isInfinite(retval));
  }
  
  @Test
  public void testMultiply()
  {
    IRational a = IRational.make(12);
    IRational b = IRational.make(3);
    
    mRational = IRational.sMultiply(a, b);
    assertTrue(mRational.getDouble() == 36);
  }

  @Test
  public void testAdd()
  {
    IRational a = IRational.make(12);
    IRational b = IRational.make(3);
    
    mRational = IRational.sAdd(a, b);
    assertTrue(mRational.getDouble() == 15);
  }  

  @Test
  public void testSubtract()
  {
    IRational a = IRational.make(12);
    IRational b = IRational.make(3);
    
    mRational = IRational.sSubtract(a, b);
    assertTrue(mRational.getDouble() == 9);
  }
  
  @Test
  public void testDivision()
  {
    IRational a = IRational.make(12);
    IRational b = IRational.make(3);
    
    mRational = IRational.sDivide(a, b);
    assertTrue(mRational.getDouble() == 4);
    
    mRational = IRational.sDivide(
        IRational.make(1),
        IRational.make(0));
    assertTrue(Double.isInfinite(mRational.getDouble()));

    mRational = IRational.sDivide(
        IRational.make(0),
        IRational.make(0));
    assertTrue(Double.isNaN(mRational.getDouble()));
  }
  
  @Test
  public void testCreateFromNumAndDen()
  {
    mRational = IRational.make(1, 10);
    assertTrue(mRational.getDouble() == 0.1);
    mRational = IRational.make(2,10);
    assertTrue(mRational.getDouble() == 0.2);
    assertTrue(mRational.getNumerator() == 1);
    assertTrue(mRational.getDenominator() == 5);
    
  }
}
