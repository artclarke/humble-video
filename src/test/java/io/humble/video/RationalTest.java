package io.humble.video;

import org.junit.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.humble.video.Rational;

import junit.framework.TestCase;

public class RationalTest extends TestCase
{
  private final Logger log = LoggerFactory.getLogger(this.getClass());

  private Rational mRational=null;

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
    mRational = Rational.make();    
    assertTrue(mRational.getNumerator() == 0);
    assertTrue(mRational.getDenominator() == 1);
    mRational.delete();
    mRational = null;
    
    mRational = Rational.make(6.0);
    assertTrue(mRational.getNumerator() == 6);
    assertTrue(mRational.getDenominator() == 1);
    mRational.delete();
    mRational = null;

    mRational = Rational.make(6.1);
    assertTrue(mRational.getNumerator() == 61);
    assertTrue(mRational.getDenominator() == 10);
    mRational.delete();
    mRational = null;
  }
  
  @Test
  public void testReduction()
  {
    int retval = -1;
    mRational = Rational.make(2.2);
    
    assertTrue(mRational.getNumerator() == 11);
    assertTrue(mRational.getDenominator() == 5);

    retval = mRational.reduce(mRational.getNumerator()*5,
        mRational.getDenominator()*10, 100);
    assertTrue("not exact reduction", retval ==1);
    assertTrue(mRational.getNumerator()==11);
    assertTrue(mRational.getDenominator()==10);
    
    retval = Rational.sReduce(mRational, 33, 32, 10);
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
    Rational otherRational = Rational.make(5);
    mRational = Rational.make(4);

    retval = mRational.compareTo(otherRational);
    assertTrue(retval <0);
    retval = otherRational.compareTo(mRational);
    assertTrue(retval >0);
    retval = mRational.compareTo(mRational);
    assertTrue(retval == 0);
    retval = Rational.sCompareTo(mRational, otherRational);
    assertTrue(retval < 0);
  }
  
  @Test
  public void testGetDouble()
  {
    double retval = -1;
    mRational = Rational.make();
    retval = mRational.getDouble();
    assertTrue(retval == 0);
    
    // Now, let's create an invalid value and see what happens.
    mRational = Rational.make();
    mRational.reduce(1,0, 10);
    retval = mRational.getDouble();
    assertTrue("double is valid: " + retval, Double.isInfinite(retval));
  }
  
  @Test
  public void testMultiply()
  {
    Rational a = Rational.make(12);
    Rational b = Rational.make(3);
    
    mRational = Rational.sMultiply(a, b);
    assertTrue(mRational.getDouble() == 36);
  }

  @Test
  public void testAdd()
  {
    Rational a = Rational.make(12);
    Rational b = Rational.make(3);
    
    mRational = Rational.sAdd(a, b);
    assertTrue(mRational.getDouble() == 15);
  }  

  @Test
  public void testSubtract()
  {
    Rational a = Rational.make(12);
    Rational b = Rational.make(3);
    
    mRational = Rational.sSubtract(a, b);
    assertTrue(mRational.getDouble() == 9);
  }
  
  @Test
  public void testDivision()
  {
    Rational a = Rational.make(12);
    Rational b = Rational.make(3);
    
    mRational = Rational.sDivide(a, b);
    assertTrue(mRational.getDouble() == 4);
    
    mRational = Rational.sDivide(
        Rational.make(1),
        Rational.make(0));
    assertTrue(Double.isInfinite(mRational.getDouble()));

    mRational = Rational.sDivide(
        Rational.make(0),
        Rational.make(0));
    assertTrue(Double.isNaN(mRational.getDouble()));
  }
  
  @Test
  public void testCreateFromNumAndDen()
  {
    mRational = Rational.make(1, 10);
    assertTrue(mRational.getDouble() == 0.1);
    mRational = Rational.make(2,10);
    assertTrue(mRational.getDouble() == 0.2);
    assertTrue(mRational.getNumerator() == 1);
    assertTrue(mRational.getDenominator() == 5);
    
  }
}
