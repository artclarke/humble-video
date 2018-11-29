## Task List
* [x] Saner build systems for eventual CI
    * [x] File Issue for Docker-based build system
    * [x] Make Ubuntu:12.04 docker container to build Ubuntu & Windows binaries
    * [x] Change os-designation to come from Maven rather than intelligently inside the GNU folders.
    * [x] Change debug vs not designation to come from Maven
    * [x] Change LGPL vs GPL for dependencies to come from Maven
    * [x] Change memcheck designation to come from Maven
    * [x] Create script to invoke all supported build matrix combinations from Maven
    * [x] Remove `humble-video-chef` and update README to require Docker

* [ ] Upgrade to FFMpeg 4.1 with current dependencies and get all Humble tests to pass
    * [x] File Issue for FFmpeg 4.1 support
    * [ ] Make Ubuntu:18.04 LTS docker container to build Ubuntu & Windows binaries
    * [ ] Get captive FFmpeg 4.1 to build in that container and on Mac
    * [ ] Get native to build with FFmpeg 4.1 and all native tests to pass
    * [ ] Get memcheck to pass
        * This is NOT worth doing on the 2.8 set.
    * [ ] Get java to build with FFmpeg 4.1 and all tests to pass
    * [ ] Add additional libraries
        * [ ] openh264 (cisco)
            * [ ] make --enable-gpl work
            * [ ] add test to native tests
        * [ ] libx265
            * [ ] add test to native tests
        * [ ] opencl?
            * [ ] add test to native tests
        * [ ] subtitle libraries?
            * [ ] add test?
        * [ ] others?
    * [ ] Add demo to show what's possible in FFmpeg 4.1
    * [ ] Close the ticket
* [ ] Other Potential Ideas
    * [ ] (stretch) Create CentOS Docker file and get tests to pass

## Fixes

* [ ] Muxer::write(...) should be rewritten so that force interleave is a Muxer level option, and ::send(...) works like everything else.

    
 # What is the processor algorithm
 
 Add a sink to a pump
 Connect a sink to a source
 
 Pipe<T1, T2> -> Source<T1> & Sink<T2>
 
 Pipe.Connect(Sink<T>, Pipe<A, T>)
 Pipe.Connect(Source<T>, Pipe<T, B>)
 Pipe.receive()
 
 Pipe.Connect(
 
 Processor.Pump(Source, Sink)
  - send()
  - receive()
  
  // Continually pumps a connected pipe
  addConnection(source, sink)
    - should check for cycles, and exception (not insert) if one found
  
    pump->receive(dep)
      -- starts with ordered list of predecessors
      (head, tail[]) = ordered_dependencies
      label:
      r = head->receive() from head
      if (r != awaiting_data || tail == nil)
        return r;
      } else {
        r = pump->receive(tail) // recurse, tail
        if (r == success)
          r = head->send()
          (goto label) return pump->receive(dep) // recurse in place; can loop
        else if (r == eof) {
          // enter flushing mode
          r = head->send(0)
           (goto label) return pump->receive(dep) // recurse in place; can loop
        } // awaiting
        else
          return r;
      }

We can imagine that we have a system `S` that consumes `N` sources and produces `M` outputs. In this diagram `[name]` is a Processor [`P`] and `->` is a Pipe [`p`]. The simplest `S` is `P`, and the next simplest is `P1 -> P2`.

A simple case of `S` that consumes 1 input and produces 1 output is (Example1: media transcoder):
 
                                |                                                           |
     [source:file] -> [demuxer] | -> [decoder:audio] ->                     [encoder:audio] | -> [muxer] -> [sink:file]
                                | -> [decoder:video] -> [filter:picture] -> [encoder:video] |
 
 A more complex case of `S` could be imaged as (Example2: video mixer):
 
     [source1:video1] |              | [video1 overlay video2]
     [source2:video2] |-> [renderer] | 
     [source3:video3] |              | [video3 overlay video2]
     
Systems should be composable. So imagine (Example1) where the middle decode/filter/encode steps are taken out into a sub system `S2`

                                |                                                           |
     [source:file] -> [demuxer] | ->                        [S2]                            | -> [muxer] -> [sink:file]
                                | ->                                                        |

Or:

                                |                                                           |
     [source:file] ->   [S1]    | ->                        [S2]                            | ->   [S3] -> [sink:file]
                                | ->                                                        |

So by this definition a `System` and a `Processor` have the same interface except a `Processor` has the limitation that it can only operate on one thing at a time -- it is serial by definition. The `System` can have different `Processors` running concurrently on different threads/machines, and it is the `Pipes` responsibility to transfer data between the systems. In this way, a `Processor` and an `Actor` are similar, and a `Pipe` and a `Mailbox` in the actor model are aligned.

`Pipes` send `Messages` between end-points, and all communication must be asynchronous. A `Processor` should advertise if it blocks to enable `Systems` that compose themselves with it to configure it with independent threads of execution. If a `System` chooses to configure a `Processor` on a seperate thread, it may advertise itself as a non-blocking `Processor`.

`Messages` can be of two basic types, `MsgControl` and `MsgMedia`. 

`MsgMedia` messages hold `Media` objects, in the `humble-video` sense. A `Processor` consuming a `MsgMedia` message should either consume the media in-place (i.e. not copy or addRef it) or addRef the media if it plans to keep it outside of a message handler.

  `Media`
    - `MediaEncoded`
       -`MediaPacket`
    - `MediaRaw`
       - `MediaSubtitle`
       - `MediaSampled`
         - `MediaAudio`
         - `MediaPicture`
    

`Pipes` will have a `Leak` policy for `MsgMedia` messages and a `Leak` policy for `MsgControl` messages. -- no Pipe can be unbounded for `MsgMedia` messages -- it will lead to a crash because those are so large. In reality `MsgControl` objects should not leak.

`Pipes` will only handle one type of `Media` message -- for example, only MediaPacket data, or only MediaAudio data. In addition, a `Pipe` may impose sub-constraints on the messages (e.g. only 44.1khz, 2 channel, raw audio data).

 
