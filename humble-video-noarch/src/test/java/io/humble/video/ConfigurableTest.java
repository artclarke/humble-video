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
    Demuxer source = Demuxer.make();

    assertTrue(source.getNumProperties() > 10);
  }

  @Test
  public void testGetPropertyMetaDataInt() {

    Demuxer source = Demuxer.make();
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
    Demuxer source = Demuxer.make();
    source.getPropertyMetaData("not-an-option");
  }

  @Test
  public void testSetPropertyStringString() {
    Demuxer source = Demuxer.make();

    // set it.
    source.setProperty(GOOD_OPTION, "66.459");

    assertEquals(66, source.getPropertyAsDouble(GOOD_OPTION),
        0.01);
  }

  @Test(expected = PropertyNotFoundException.class)
  public void testSetPropertyStringStringNotFound() {
    Demuxer source = Demuxer.make();

    source.setProperty("not-an-option", "66.459");
  }

  @Test
  public void testSetPropertyStringDouble() {
    Demuxer source = Demuxer.make();

    // set it.
    source.setProperty(GOOD_OPTION, 66.459);

    assertEquals(66, source.getPropertyAsDouble(GOOD_OPTION),
        0.01);
  }

  @Test(expected = PropertyNotFoundException.class)
  public void testSetPropertyStringDoubleNotFound() {
    Demuxer source = Demuxer.make();

    source.setProperty("not-an-option", 66.459);
  }

  @Test
  public void testSetPropertyStringLong() {
    Demuxer source = Demuxer.make();

    // set it.
    source.setProperty(GOOD_OPTION, 66L);

    assertEquals(66, source.getPropertyAsDouble(GOOD_OPTION),
        0.01);
  }

  @Test(expected = PropertyNotFoundException.class)
  public void testSetPropertyStringLongNotFound() {
    Demuxer source = Demuxer.make();

    source.setProperty("not-an-option", 66L);
  }

  @Test
  public void testSetPropertyStringBoolean() {
    Demuxer source = Demuxer.make();

    // set it.
    source.setProperty(GOOD_OPTION, true);

    assertEquals(true,
        source.getPropertyAsBoolean(GOOD_OPTION));
  }

  @Test(expected = PropertyNotFoundException.class)
  public void testSetPropertyStringBooleanNotFound() {
    Demuxer source = Demuxer.make();

    source.setProperty("not-an-option", true);
  }
  
  @Test
  public void testSetPropertyStringRational() {
    Demuxer source = Demuxer.make();
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
    Demuxer source = Demuxer.make();
    Rational r = Rational.make(2, 1);

    source.setProperty("not-an-option", r);
  }
  
  @Test
  public void testGetPropertyAsString() {
    Demuxer source = Demuxer.make();
    assertEquals("0", source.getPropertyAsString(GOOD_OPTION));
  }

  @Test
  public void testGetPropertyAsDouble() {
    Demuxer source = Demuxer.make();
    assertEquals(0.0, source.getPropertyAsDouble(GOOD_OPTION), 0.01);
  }

  @Test
  public void testGetPropertyAsLong() {
    Demuxer source = Demuxer.make();
    assertEquals(0L, source.getPropertyAsLong(GOOD_OPTION));
  }

  @Test
  public void testGetPropertyAsInt() {
    Demuxer source = Demuxer.make();
    assertEquals(0, source.getPropertyAsInt(GOOD_OPTION));
  }

  @Test
  public void testGetPropertyAsRational() {
    Demuxer source = Demuxer.make();
    assertEquals(0.0, source.getPropertyAsRational(GOOD_OPTION).getValue(), 0.0001);
  }

  @Test
  public void testGetPropertyAsBoolean() {
    Demuxer source = Demuxer.make();
    assertEquals(false, source.getPropertyAsBoolean(GOOD_OPTION));
  }

  @Test
  public void testSetPropertyKeyValueBagKeyValueBag() {
    KeyValueBag in = KeyValueBag.make();
    KeyValueBag out = KeyValueBag.make();
    
    in.setValue(GOOD_OPTION, "2");
    in.setValue("not-an-option", "62");
    
    Demuxer source = Demuxer.make();
    source.setProperty(in, out);
    assertEquals(1,  out.getNumKeys());
    assertEquals(2,  source.getPropertyAsInt(GOOD_OPTION));
    assertEquals("62", out.getValue("not-an-option"));
  }

}
