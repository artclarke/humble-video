/*
 * ProcessorTestHelper.h
 *
 *  Created on: Nov 28, 2018
 *      Author: aclarke
 */

#ifndef SRC_MAIN_GNU_TEST_IO_HUMBLE_VIDEO_PROCESSORTESTHELPER_H_
#define SRC_MAIN_GNU_TEST_IO_HUMBLE_VIDEO_PROCESSORTESTHELPER_H_

#include <Processor.h>

namespace io { namespace humble { namespace video {

  /**
   * Creates a tester for ProcessorSinks and Sources
   */
  class ProcessorTestHelper: public virtual ProcessorSink, public virtual PrcessorSource
  {
  public:
    ProcessorTestHelper ();
    virtual
    ~ProcessorTestHelper ();
  };

 }}}

#endif /* SRC_MAIN_GNU_TEST_IO_HUMBLE_VIDEO_PROCESSORTESTHELPER_H_ */
