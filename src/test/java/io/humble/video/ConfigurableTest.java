package io.humble.video;

import static org.junit.Assert.*;

import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ConfigurableTest {
  private final Logger log = LoggerFactory.getLogger(this.getClass());
  private final static String GOOD_OPTION = "use_wallclock_as_timestamps";

  @Test
  public void testGetNumProperties() {
    Source source = Source.make();

    assertTrue(source.getNumProperties() > 10);
  }

  @Test
  public void testGetPropertyMetaDataInt() {

    Source source = Source.make();
    int n = source.getNumProperties();

    Property d;
    for (int i = 0; i < n; i++) {
      d = source.getPropertyMetaData(i);
      assertNotNull(d);
      log.debug("name: {}", d.getName());
    }
    try {
      source.getPropertyMetaData(n);
      fail("should not get here");
    } catch (IllegalArgumentException e) {

    }
    try {
      source.getPropertyMetaData(-1);
      fail("should not get here");
    } catch (IllegalArgumentException e) {

    }

  }

  @Test(expected = PropertyNotFoundException.class)
  public void testGetPropertyMetaDataStringNotFound() {
    Source source = Source.make();
    source.getPropertyMetaData("not-an-option");
  }

  @Test
  public void testSetPropertyStringString() {
    Source source = Source.make();

    // set it.
    source.setProperty(GOOD_OPTION, "66.459");

    assertEquals(66, source.getPropertyAsDouble(GOOD_OPTION),
        0.01);
  }

  @Test(expected = PropertyNotFoundException.class)
  public void testSetPropertyStringStringNotFound() {
    Source source = Source.make();

    source.setProperty("not-an-option", "66.459");
  }

  @Test
  public void testSetPropertyStringDouble() {
    Source source = Source.make();

    // set it.
    source.setProperty(GOOD_OPTION, 66.459);

    assertEquals(66, source.getPropertyAsDouble(GOOD_OPTION),
        0.01);
  }

  @Test(expected = PropertyNotFoundException.class)
  public void testSetPropertyStringDoubleNotFound() {
    Source source = Source.make();

    source.setProperty("not-an-option", 66.459);
  }

  @Test
  public void testSetPropertyStringLong() {
    Source source = Source.make();

    // set it.
    source.setProperty(GOOD_OPTION, 66L);

    assertEquals(66, source.getPropertyAsDouble(GOOD_OPTION),
        0.01);
  }

  @Test(expected = PropertyNotFoundException.class)
  public void testSetPropertyStringLongNotFound() {
    Source source = Source.make();

    source.setProperty("not-an-option", 66L);
  }

  @Test
  public void testSetPropertyStringBoolean() {
    Source source = Source.make();

    // set it.
    source.setProperty(GOOD_OPTION, true);

    assertEquals(true,
        source.getPropertyAsBoolean(GOOD_OPTION));
  }

  @Test(expected = PropertyNotFoundException.class)
  public void testSetPropertyStringBooleanNotFound() {
    Source source = Source.make();

    source.setProperty("not-an-option", true);
  }
  
  @Test
  public void testSetPropertyStringRational() {
    Source source = Source.make();
    Rational r = Rational.make(2, 1);

    // set it.
    source.setProperty(GOOD_OPTION, r);

    assertEquals(2, source.getPropertyAsInt(GOOD_OPTION));
    
    // and make sure this doesn't crash
    try {
      source.setProperty(GOOD_OPTION, (Rational)null);
      fail("should not get here");
    } catch (IllegalArgumentException e) {
      
    }
  }

  @Test(expected = PropertyNotFoundException.class)
  public void testSetPropertyStringRationalNotFound() {
    Source source = Source.make();
    Rational r = Rational.make(2, 1);

    source.setProperty("not-an-option", r);
  }
  
  @Test
  public void testGetPropertyAsString() {
    Source source = Source.make();
    assertEquals("0", source.getPropertyAsString(GOOD_OPTION));
  }

  @Test
  public void testGetPropertyAsDouble() {
    Source source = Source.make();
    assertEquals(0.0, source.getPropertyAsDouble(GOOD_OPTION), 0.01);
  }

  @Test
  public void testGetPropertyAsLong() {
    Source source = Source.make();
    assertEquals(0L, source.getPropertyAsLong(GOOD_OPTION));
  }

  @Test
  public void testGetPropertyAsInt() {
    Source source = Source.make();
    assertEquals(0, source.getPropertyAsInt(GOOD_OPTION));
  }

  @Test
  public void testGetPropertyAsRational() {
    Source source = Source.make();
    assertEquals(0.0, source.getPropertyAsRational(GOOD_OPTION).getValue(), 0.0001);
  }

  @Test
  public void testGetPropertyAsBoolean() {
    Source source = Source.make();
    assertEquals(false, source.getPropertyAsBoolean(GOOD_OPTION));
  }

  @Test
  public void testSetPropertyKeyValueBagKeyValueBag() {
    KeyValueBag in = KeyValueBag.make();
    KeyValueBag out = KeyValueBag.make();
    
    in.setValue(GOOD_OPTION, "2");
    in.setValue("not-an-option", "62");
    
    Source source = Source.make();
    source.setProperty(in, out);
    assertEquals(1,  out.getNumKeys());
    assertEquals(2,  source.getPropertyAsInt(GOOD_OPTION));
    assertEquals("62", out.getValue("not-an-option"));
  }

}
