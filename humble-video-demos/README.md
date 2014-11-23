This package containers a variety of demonstration programs using the Humble API.

I recommend going through each one in the following order, as they gradually introduce new
concepts.

| Demo | What You'll Learn |
| ------ | ------------------- |
| [GetContainerInfo](https://github.com/artclarke/humble-video/blob/develop/humble-video-demos/src/main/java/io/humble/video/demos/GetContainerInfo.java) | How to open up a media file (Container) and query the contents of that container. |
| [DecodeAndPlayAudio](https://github.com/artclarke/humble-video/blob/develop/humble-video-demos/src/main/java/io/humble/video/demos/DecodeAndPlayAudio.java) | How to open a media file, find an audio stream, decode/uncompress the audio, and play it back on your speakers. |
| [DecodeAndPlayVideo](https://github.com/artclarke/humble-video/blob/develop/humble-video-demos/src/main/java/io/humble/video/demos/DecodeAndPlayVideo.java) | How to open a media file, find an video stream, decode/uncompress the video, and then use timestamps to determine the right time to display different pictures so the on-screen timing is right. |
| [RecordAndEncodeVideo](https://github.com/artclarke/humble-video/blob/develop/humble-video-demos/src/main/java/io/humble/video/demos/RecordAndEncodeVideo.java) | Recording a screen-capture. This demo shows how to open a media file for writing (Muxer), how to get screenshots from Java, how to add streams to a Muxer, and how to Encode and write video data. |
| [ContainerSegmenter](https://github.com/artclarke/humble-video/blob/develop/humble-video-demos/src/main/java/io/humble/video/demos/ContainerSegmenter.java) | Shows how to re-mux a video file (i.e. just rewrite packets with no re-encoding) into Apple's HTTP Live Streaming format. Also introduces Bit Stream Filters for doing simple rewriting of packets for some cases. |

If you are brand-new to video programming, I recommend you review this short series of videos first: [Overly Simplistic Guide to Internet Video](http://blog.xuggle.com/2009/01/23/overly-simplistic-guide-to-internet-video)
