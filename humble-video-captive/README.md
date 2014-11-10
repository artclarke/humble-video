The `humble-video-captive` directory contains all the native libraries that Humble Video depends on. 

In humble-video, we compile versions of every library as a static library, and then the
`humble-video-native` directory creates a shared library that links against those static libraries. The process involved copying (or git subtreeing) the relevant source code into the captive directory, and then 'incarcerating' them with Humble-specific build scripts.

The dependency tree is complicated, but looks like this:
```

  libz
    |
 openssl                                                    ogg
    |                                                        |
  -----                                             -------------------
  |   |                                            |      |     |      |
  | rtmp vo-aacenc opencore-amr mp3lame x264 vpx speex vorbis theora   |
  |   |      |          |          |     |    |    |      |     |      |
  --------------------------------------------------------------------- 
                                   |
                               ffmpeg
                                   |
                             humble-video
```
