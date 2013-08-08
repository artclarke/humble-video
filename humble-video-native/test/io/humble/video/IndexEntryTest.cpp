#include "IndexEntryTest.h"
#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/IndexEntry.h>
#if 0
#include <io/humble/video/Container.h>
#include <io/humble/video/Stream.h>
#endif

// For getenv()
#include <stdlib.h>

using namespace io::humble::video;
using namespace io::humble::ferry;

IndexEntryTest :: IndexEntryTest()
{

}

IndexEntryTest :: ~IndexEntryTest()
{

}

void
IndexEntryTest :: setUp()
{

}

void
IndexEntryTest :: tearDown()
{

}

void
IndexEntryTest :: testCreation()
{
  int64_t pos = 1;
  int64_t ts = 2;
  int32_t flags = IndexEntry::INDEX_FLAG_KEYFRAME;
  int32_t size = 4;
  int32_t minDistance = 5;
  RefPointer<IndexEntry> entry = IndexEntry::make(
      pos,
      ts,
      flags,
      size,
      minDistance);
  TSM_ASSERT("should exist", entry);
  if (!entry)
    return;
  TSM_ASSERT_EQUALS("", pos, entry->getPosition());
  TSM_ASSERT_EQUALS("", ts, entry->getTimeStamp());
  TSM_ASSERT_EQUALS("", flags, entry->getFlags());
  TSM_ASSERT_EQUALS("", size, entry->getSize());
  TSM_ASSERT_EQUALS("", minDistance, entry->getMinDistance());
}

#if 0
void
IndexEntryTest :: testGetIndexEntry()
{
  RefPointer<IContainer> container = IContainer::make();

  char file[2048];
  const char *fixtureDirectory = getenv("VS_TEST_FIXTUREDIR");
  const char *sample = "testfile_h264_mp4a_tmcd.mov";
  if (fixtureDirectory && *fixtureDirectory)
    snprintf(file, sizeof(file), "%s/%s", fixtureDirectory, sample);
  else
    snprintf(file, sizeof(file), "./%s", sample);

  int retval = container->open(file, IContainer::READ, 0);
  TSM_ASSERT("", retval >= 0);
  RefPointer<IStream> stream = container->getStream(0);
  TSM_ASSERT("", stream);
  if (!stream) return;
  int32_t numEntries = stream->getNumIndexEntries();
  TSM_ASSERT_EQUALS("", 2665, numEntries);
  RefPointer<IIndexEntry> entry = stream->getIndexEntry(numEntries-1);
  TSM_ASSERT("", entry);
  TSM_ASSERT_EQUALS("", 11673146, entry->getPosition());
  TSM_ASSERT_EQUALS("", 332875, entry->getTimeStamp());
  TSM_ASSERT_EQUALS("should be non-keyframe", 0, entry->getFlags());
  TSM_ASSERT_EQUALS("", 50, entry->getSize());
  TSM_ASSERT_EQUALS("", 96, entry->getMinDistance());

  // Bounds checking
  entry = stream->getIndexEntry(-1);
  TSM_ASSERT("", !entry);
  entry = stream->getIndexEntry(numEntries);
  TSM_ASSERT("", !entry);

  // close the container
  container->close();
}

void
IndexEntryTest :: testAddIndexEntry()
{
  RefPointer<IContainer> container = IContainer::make();
  int retval = container->open("IIndexEntry_1.mov", IContainer::WRITE, 0);
  TSM_ASSERT("", retval >= 0);
  RefPointer<IStream> stream = container->addNewStream(ICodec::CODEC_ID_FLV1);
  int64_t pos = 1;
  int64_t ts = 2;
  int32_t flags = IIndexEntry::IINDEX_FLAG_KEYFRAME;
  int32_t size = 4;
  int32_t minDistance = 5;
  RefPointer<IIndexEntry> entry = IIndexEntry::make(
      pos,
      ts,
      flags,
      size,
      minDistance);
  TSM_ASSERT_EQUALS("", 0, stream->getNumIndexEntries());
  TSM_ASSERT("should exist", entry);
  retval = stream->addIndexEntry(entry.value());
  TSM_ASSERT("", retval >= 0);
  TSM_ASSERT_EQUALS("", 1, stream->getNumIndexEntries());

  RefPointer<IIndexEntry> foundEntry = stream->getIndexEntry(0);
  TSM_ASSERT("", foundEntry);
  TSM_ASSERT_EQUALS("", entry->getPosition(), foundEntry->getPosition());
  TSM_ASSERT_EQUALS("", entry->getTimeStamp(), foundEntry->getTimeStamp());
  TSM_ASSERT_EQUALS("", entry->getFlags(), foundEntry->getFlags());
  TSM_ASSERT_EQUALS("", entry->getSize(), foundEntry->getSize());
  TSM_ASSERT_EQUALS("", entry->getMinDistance(), foundEntry->getMinDistance());

  foundEntry = stream->findTimeStampEntryInIndex(ts, 0);
  TSM_ASSERT("", foundEntry);
  TSM_ASSERT_EQUALS("", entry->getPosition(), foundEntry->getPosition());
  TSM_ASSERT_EQUALS("", entry->getTimeStamp(), foundEntry->getTimeStamp());
  TSM_ASSERT_EQUALS("", entry->getFlags(), foundEntry->getFlags());
  TSM_ASSERT_EQUALS("", entry->getSize(), foundEntry->getSize());
  TSM_ASSERT_EQUALS("", entry->getMinDistance(), foundEntry->getMinDistance());

  retval = container->close();
  TSM_ASSERT("", retval >= 0);
}
#endif
