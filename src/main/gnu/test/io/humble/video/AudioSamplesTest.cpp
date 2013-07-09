
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/LoggerStack.h>
#include <io/humble/video/SourceStream.h>
#include <io/humble/video/Codec.h>
#include <io/humble/video/Global.h>
#include "AudioSamplesTest.h"
#ifdef FULL_TESTS
#include <io/humble/video/StreamCoder.h>
#include "Helper.h"
#endif

using namespace io::humble::ferry;
using namespace io::humble::video;

VS_LOG_SETUP(VS_CPP_PACKAGE);

AudioSamplesTest :: AudioSamplesTest()
{
#ifdef FULL_TESTS
  h = 0;
  hw = 0;
#endif
}

AudioSamplesTest :: ~AudioSamplesTest()
{
  tearDown();
}

void
AudioSamplesTest :: setUp()
{
#ifdef FULL_TESTS
  if (h)
    delete h;
  h = new Helper();
  if (hw)
    delete hw;
  hw = new Helper();
#endif
}

void
AudioSamplesTest :: tearDown()
{
#ifdef FULL_TESTS
  if (h)
    delete h;
  h = 0;
  if (hw)
    delete hw;
  hw = 0;
#endif
}

void
AudioSamplesTest :: testCreationAndDestruction()
{
  RefPointer<AudioSamples> samples = 0;

  samples = AudioSamples::make(1024, 1);
  TSM_ASSERT("got no samples", samples);
  TSM_ASSERT("sample buffer not big enough",
      samples->getMaxBufferSize() >= 1024);
  TSM_ASSERT_EQUALS("there are samples in the buffer?",
      samples->getNumSamples(), (unsigned int)0);
  TSM_ASSERT("unexpected sample size",
      samples->getSampleBitDepth() == sizeof(short)*8);
  samples = AudioSamples::make(0, 1);
  TSM_ASSERT("got some samples where none expected", !samples);
}

#ifdef FULL_TESTS
void
AudioSamplesTest :: testDecodingToBuffer()
{
  RefPointer<AudioSamples> samples = 0;
  int numSamples = 0;
  int numPackets = 0;
  int audioStream = -1;
  int retval = -1;

  samples = AudioSamples::make(1024, h->expected_channels);
  TSM_ASSERT("got no samples", samples);
  TSM_ASSERT("should not be complete", !samples->isComplete());

  h->setupReading(h->SAMPLE_FILE);
  RefPointer<IPacket> packet = IPacket::make();
  for (int i = 0; i < h->num_streams; i++)
  {
    if (h->codecs[i]->getType() == ICodec::CODEC_TYPE_AUDIO)
    {
      // got audio
      audioStream = i;
      retval = h->coders[i]->open();
      TSM_ASSERT("! open codec", retval >= 0);
      break;
    }
  }

  TSM_ASSERT("couldn't find an audio stream", audioStream >= 0);
  int maxSamples = 10 * h->coders[audioStream]->getSampleRate(); // 10 seconds

  while (h->container->readNextPacket(packet.value()) == 0 &&
      numSamples < maxSamples)
  {
    if (packet->getStreamIndex() == audioStream)
    {
      int offset = 0;

      numPackets++;

      while (offset < packet->getSize())
      {
        retval = h->coders[audioStream]->decodeAudio(
            samples.value(),
            packet.value(),
            offset);
        TSM_ASSERT("could not decode any audio",
            retval > 0);
        offset += retval;
        TSM_ASSERT("could not write any samples",
            samples->getNumSamples() > 0);
        numSamples += samples->getNumSamples();

        TSM_ASSERT("did not finish", samples->isComplete());
        TSM_ASSERT_EQUALS("wrong sample rate", samples->getSampleRate(),
            h->expected_sample_rate);
        TSM_ASSERT_EQUALS("wrong channels", samples->getChannels(),
            h->expected_channels);
        TSM_ASSERT_EQUALS("wrong format", samples->getFormat(),
            AudioSamples::SAMPLE_FMT_S16);
      }
    }
  }
  retval = h->coders[audioStream]->close();

  TSM_ASSERT("could not get any audio packets", numPackets > 0);
  TSM_ASSERT("could not decode any audio", numSamples > 0);

}

void
AudioSamplesTest :: testEncodingToBuffer()
{
  RefPointer<AudioSamples> samples = 0;
  int numSamples = 0;
  int numPackets = 0;
  int retval = -1;

  samples = AudioSamples::make(1024*128*4, h->expected_channels);
  TSM_ASSERT("got no samples", samples);

  h->setupReading(h->SAMPLE_FILE);

  RefPointer<IPacket> packet = IPacket::make();

  hw->setupWriting("AudioSamplesTest_3_output.flv", 0, "libmp3lame", "flv");
  int outStream = hw->first_output_audio_stream;
  TSM_ASSERT("Could not find an audio stream in the output", outStream >= 0);
  int inStream = h->first_input_audio_stream;
  TSM_ASSERT("Could not find an audio stream in the input", inStream >= 0);

  RefPointer<IStreamCoder> ic = h->coders[inStream];
  RefPointer<IStreamCoder> oc = hw->coders[outStream];
  RefPointer<IPacket> opacket = IPacket::make();
  TSM_ASSERT("! opacket", opacket);

  // Set the output coder correctly.
  oc->setSampleRate(ic->getSampleRate());
  oc->setChannels(ic->getChannels());
  oc->setBitRate(ic->getBitRate());

  int maxSamples = 10 * ic->getSampleRate(); // 10 seconds

  retval = ic->open();
  TSM_ASSERT("Could not open input coder", retval >= 0);
  retval = oc->open();
  TSM_ASSERT("Could not open output coder", retval >= 0);

  // write header
  retval = hw->container->writeHeader();
  TSM_ASSERT("could not write header", retval >= 0);

  while (h->container->readNextPacket(packet.value()) == 0
      && numSamples < maxSamples)
  {
    if (packet->getStreamIndex() == inStream)
    {
      int offset = 0;

      numPackets++;

      while (offset < packet->getSize())
      {
        retval = ic->decodeAudio(
            samples.value(),
            packet.value(),
            offset);
        TSM_ASSERT("could not decode any audio",
            retval > 0);
        offset += retval;
        TSM_ASSERT("could not write any samples",
            samples->getNumSamples() > 0);
        numSamples += samples->getNumSamples();

        // now, write out the packets.
        unsigned int numSamplesConsumed = 0;
        do {
          retval = oc->encodeAudio(opacket.value(), samples.value(),
              numSamplesConsumed);
          TSM_ASSERT("Could not encode any audio", retval >= 0);
          numSamplesConsumed += retval;

          //TSM_ASSERT("could not encode audio", opacket->getSize() > 0);

          RefPointer<IBuffer> encodedBuffer = opacket->getData();
          TSM_ASSERT("no encoded data", encodedBuffer);
          TSM_ASSERT("less data than there should be",
              encodedBuffer->getBufferSize() >=
              opacket->getSize());

          // now, write the packet to disk.
          if (opacket->isComplete())
          {
            retval = hw->container->writePacket(opacket.value());
            TSM_ASSERT("could not write packet", retval >= 0);
          }
        } while (numSamplesConsumed < samples->getNumSamples());
      }
    }
  }
  // sigh; it turns out that to flush the encoding buffers you need to
  // ask the encoder to encode a NULL set of samples.  So, let's do that.
  retval = oc->encodeAudio(opacket.value(), 0, 0);
  TSM_ASSERT("Could not encode any audio", retval >= 0);
  if (retval > 0)
  {
    retval = hw->container->writePacket(opacket.value());
    TSM_ASSERT("could not write packet", retval >= 0);
  }

  retval = hw->container->writeTrailer();
  TSM_ASSERT("! writeTrailer", retval >= 0);

  retval = ic->close();
  TSM_ASSERT("! close", retval >= 0);
  retval = oc->close();
  TSM_ASSERT("! close", retval >= 0);

  TSM_ASSERT("could not get any audio packets", numPackets > 0);
  TSM_ASSERT("could not decode any audio", numSamples > 0);


}
#endif

void
AudioSamplesTest :: testSetSampleEdgeCases()
{
  RefPointer<AudioSamples> samples = 0;

  // turn down logging for this test
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_WARN, false);

  samples = AudioSamples::make(1024, 1);
  TSM_ASSERT("got no samples", samples);
  int retval = 0;

  retval = samples->setSample(samples->getMaxBufferSize(), 1, AudioSamples::SAMPLE_FMT_S16, 0);
  TSM_ASSERT("sampleIndex should be too large", retval < 0);

  retval = samples->setSample(3, 2, AudioSamples::SAMPLE_FMT_S16, 0);
  TSM_ASSERT("channels is too large", retval < 0);

  retval = samples->setSample(3, -1, AudioSamples::SAMPLE_FMT_S16, 0);
  TSM_ASSERT("channels is too small", retval < 0);

  retval = samples->setSample(3, 1, AudioSamples::SAMPLE_FMT_NONE, 0);
  TSM_ASSERT("format is not supported", retval < 0);
}

void
AudioSamplesTest :: testSetSampleSunnyDayScenarios()
{
  RefPointer<AudioSamples> samples = 0;

  samples = AudioSamples::make(1024, 2);
  TSM_ASSERT("got no samples", samples);
  int retval = 0;

  samples->setComplete(true, 512, 22050, 2, AudioSamples::SAMPLE_FMT_S16, 0);

  unsigned int sampleIndex = 3;
  short channel0Sample = 8;
  short channel1Sample = 16;
  retval = samples->setSample(sampleIndex, 0, AudioSamples::SAMPLE_FMT_S16, channel0Sample);
  TSM_ASSERT("should set sample without error", retval >= 0);
  retval = samples->setSample(sampleIndex, 1, AudioSamples::SAMPLE_FMT_S16, channel1Sample);
  TSM_ASSERT("should set sample without error", retval >= 0);

  TSM_ASSERT("should be equal to set value",
      samples->getSample(sampleIndex, 0, AudioSamples::SAMPLE_FMT_S16) == channel0Sample);
  TSM_ASSERT("should be equal to set value",
      samples->getSample(sampleIndex, 1, AudioSamples::SAMPLE_FMT_S16) == channel1Sample);
}

void
AudioSamplesTest :: testGetSampleRainyDayScenarios()
{
  // turn down logging for this test
  LoggerStack stack;
  stack.setGlobalLevel(Logger::LEVEL_WARN, false);

  RefPointer<AudioSamples> samples = 0;

  samples = AudioSamples::make(1024, 1);
  TSM_ASSERT("got no samples", samples);
  int retval = 0;

  short channel0Sample = 8;
  samples->setSample(0, 0, AudioSamples::SAMPLE_FMT_S16, channel0Sample);
  samples->setComplete(true, 1, 22050, 1, AudioSamples::SAMPLE_FMT_S16, 0);

  retval = samples->getSample(0, 0, AudioSamples::SAMPLE_FMT_S16);
  TSM_ASSERT_EQUALS("should be set value", channel0Sample, retval);

  retval = samples->getSample(samples->getNumSamples(), 0, AudioSamples::SAMPLE_FMT_S16);
  TSM_ASSERT_EQUALS("should be too large a sampleIndex", 0, retval);

  retval = samples->getSample(0, 1, AudioSamples::SAMPLE_FMT_S16);
  TSM_ASSERT_EQUALS("should be too large a channel", 0, retval);

  retval = samples->getSample(0, -1, AudioSamples::SAMPLE_FMT_S16);
  TSM_ASSERT_EQUALS("should be too small a channel", 0, retval);
}
