#Welcome to the Humble Video uber project.#

Humble-Video allows Java Virtual Machine developers (Java, Scala, Clojure, JRuby, etc.) to decode, analyze/modify and encode audio and video data into 100s of different formats (e.g. H264, AAC, MP3, FLV, etc.).

It uses the <a href="http://ffmpeg.org">FFmpeg</a> project under the covers.

# How To Use In Your Code

Humble is deployed to the [Maven Central](https://repo.maven.apache.org/maven2/io/humble/) Repository. To include it in your Maven project (note: this will include artifacts for all operating systems), add this to your pom.xml:
```xml
<project>
  ...
  <dependencies>
    ...
    <dependency>
      <groupId>io.humble</groupId>
      <artifactId>humble-video</artifactId>
      <version>1.0.0</version>
    </dependency>
    ...
  </dependencies>
  ...
</project>
```

*Important note about dependencies*: Humble-Video is a combination of pure-java and platform-dependent code. The `humble-video.jar` file depends on `humble-video-noarch.jar` which is platform independent, and `humble-video-arch.jar` which has many different jars (with different classifiers) depending on the platform. If you build your own platform dependent jar, then just depend on `humble-video-noarch.jar` and make sure your platform dependent jar is somewhere in your classpath. Humble-Video will then find it automatically.

# Demoes of the API in action.

To see how to use the API, go to the [humble-video-demos](humble-video-demos/) package. The [README.md](humble-video-demos/README.md) there is a good place to start.

# Supported Platforms

Humble Video's [Maven Central](https://repo.maven.apache.org/maven2/io/humble/) artifacts contain native (i.e. non-Java) code and are tested to run on the following platforms:


| Operating System | Architecture |
| ------ | ------------------- |
| Microsoft Windows XP or later | i686 and x86_64 intel processors |
| Apple OS X | x86_64 intel processors |
| Ubuntu 12.04 LTS and later | i686 and x86_64 intel processors |

If you are running on other platforms, the [Maven Central](https://repo.maven.apache.org/maven2/io/humble/) artifacts may not work and you'll have to build your own version. And beware ... building Humble takes a very long time.

# Building Humble Video

## Please read this BEFORE running 'mvn install'.

Humble Video is a mix of Java and native code, and the native code is written in C++, C and Assembly. Most users are **strongly** encouraged to use the maven artifacts in a central repository, as building the system is complicated.  It requires accesss to a Mac OS X development environment (Mountain Sierra or later).

It consists of several sub-projects which are detailed below:

* `humble-video-demos/`: A series of demo programs showing the Humble API in action. It's a great place to start.
* `humble-video-all/`: This creates the `humble-video.jar` jar, which depends on `humble-video-noarch` and `humble-video-arch` jars.
* `humble-video-noarch/`: All platform-independent Java code. This contains all the classes you call.
* `humble-video-arch/`: This builds a jar for each platform we support (using classifiers on the jar name to tell you what platform). These jars contain a native shared-object implemented via JNI -- i.e. all native (non-Java) code. 
* `humble-video-test/`: All integration (very long running) tests for humble-video.
* `humble-video-native/`: This package builds all the Humble Video native (C/C++/Assembly) code.
* `humble-video-captive/`: This package builds all open-source libraries incorporated into humble-video (e.g. ffmpeg, libmp3lame, libvpx, etc.).
* `humble-video-docker/`: A collection of Docker files for building containers that can be used to build Humble-Video for different operating systems.
* `humble-video-parent/`: The parent pom to all projects, where global defaults (like version numbers) are set.

To build, assuming you have all the prerequisites installed for your OS (see below), run:

    mvn install

In general the build system should 'guess' the right platform to build for.

*But warning: Humble Video contains both Java and platform-dependent
code, so usually running `mvn install` will fail if you don't have
all the prerequisites installed. We strongly recommend using our
Maven Central jars.*

# Pre-requisites on build machines

This project builds native code that can be used for Humble Software, and
also generates Java files that can be compiled into Maven libraries
that use the native code. This makes this an unusual Java build process -
it is *not* pure Java.

We try to abstract that away for non Mac-Platforms by using Docker. To see
what's required to build for Linux (Ubuntu) and Windows (on Ubuntu) see
the `humble-video-docker` directory.

## Mac OS X Notes

Mac OS X is the preferred environment for building Humble Video because it allows you to build all supported environments (Mac, Linux and Darwin) if you install Docker.

You must install Docker on your mac.

You must install the JDK on your mac. Any JDK should work, but we require 1.6 support to build this library.

You must install Apple's xCode developer tools.

You should install homebrew. It will make your life much easier for the rest of these dependencies.

The 'brew' utility is your on Mac OS X friend. Use it often.

    brew install pkg-config
    brew install yasm
    brew install nasm
    brew install valgrind

If you're going to be building a lot, I recommend installing ccache

    brew install ccache

This can speed up native code compilation significantly if you install
it to front-end all compiler calls.

To install the Humble-Swig version of Swig:
```bash
git clone git@github.com:artclarke/humble-swig.git
cd humble-swig
./autogen.sh
./configure
make
make install
```

To build mac binaries:
```bash
mvn install
```

## Linux Build Notes
Build a Docker container to build Humble-Video:
```bash
docker build -t humble-video-docker -f humble-video-docker/Dockerfile.ubuntu-12.04 humble-video-docker
```
Set up a Cache directory for CCache, because the build takes a really long time:
```bash
mkdir -p ./humble-video-cache
```
Then we use the container to run the build.
```bash
docker run --name humble-video-docker \
  -v $(pwd)/humble-video-cache:/caches \
  -v $(pwd):/source humble-video-docker:latest \
  mvn install 2>&1 | tee mvn-linux.out
```

## Windows Build Notes
Don't do it. It's not supported. Instead you have to cross-compile for Windows from a Linux box. Sorry.

# Steps Required To Release.

Release is a long process. The full build takes over 12 hours on a 2012 MacBook Pro. You really want to be highly confident before you kick it off.

That said, I've attempted to automate as much of it as possible. The biggest challenge is patience.

All steps should be done from a OS X machine, and we'll build the other binaries in docker containers.

1. Check out a fresh copy of the repo. Do NOT re-use your development branch.

2. Start a git flow release

```bash
git flow release start v<version-number>
```

3. Do a full Mac build and test.

```bash
mvn install 2>&1 | tee mvn.out
```

4. Build and run on all operating systems.

```bash
bash ./build-all.sh 2>&1 | tee mvn.out
```

5. If successful, you now have binaries for all supported OSes staged in the humble-video-arch directory. Well done. Now let's test deploying a snapshot.

```bash
mvn -P\!build,deploy deploy 2>&1 | tee mvn.out
```

6. At this stage you should have successfully deployed a fresh snapshot to Sonatype. Let's check.

    https://oss.sonatype.org/content/repositories/snapshots/io/humble/

7. If successful, it's time to PEG the snapshots to a specific release. Now things get hairy.

Edit humble-video-parent/pom.xml BY HAND to set the humble.version
```bash
cd humble-video-parent && mvn -Pdeploy versions:set -DnewVersion=<version-number>
```
8. Now, rebuild all Java Source

```bash
(cd humble-video-noarch && mvn clean) && (cd humble-video-test && mvn clean) && (cd humble-video-demos && mvn clean)
```

9. Do one last rebuild (you do not need to rebuild native sources) and deploy

```bash
(mvn install && \
docker run --name humble-video-docker \
  -v $(pwd)/humble-video-cache:/caches \
  -v $(pwd):/source humble-video-docker:latest \
  mvn install &&
mvn -P\!build,deploy deploy) 2>&1 | tee mvn.out
```

10. Log into oss.sonatype.org look for your staging repo and approve the builds.

11. Check the OSS snapshot page to see if we got deployed correctly. It can take up to an hour for Sonatype to clear the release.

    https://oss.sonatype.org/content/repositories/releases/io/humble/

12. Check Maven central to make sure it gets copied there. This can take up to 24-hours.

    https://repo.maven.apache.org/maven2/io/humble/humble-video-noarch/

13. Make sure it was done correctly by deleting all your m2 artifacts, and forcing a redownload.

```bash
rm -rf $HOME/.m2/repository/io/humble/humble-video-*
(cd humble-video-tests && mvn test)
(cd humble-video-demos && mvn test)
```

14. Merge your changes back into Develop

```bash
git flow release finish v<version-number>
```

15. Peg your develop tree to the next snapshot.

Edit humble-video-parent/pom.xml BY HAND to set the humble.version
```bash
cd humble-video-parent && mvn -Pdeploy versions:set -DnewVersion=<version-number>-SNAPSHOT
<edit humble-video-parent/pom.xml to set the version property to the same number>
cd humble-video-native/src/main/gnu
<edit configure.ac to update version numbers in an editor of your choice>
<from the linux container, re-run autoreconf for the captive and native directories>
```

16. Done!

Enjoy!

- Art
