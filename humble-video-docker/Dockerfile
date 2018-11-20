# Yes, we're deliberately using a VERY old Ubuntu
FROM ubuntu:12.04

# Turn off some warnings

RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections
# Let's get all those pre-reqs installed
RUN apt-get update
RUN apt-get install -y apt-utils=0.8.16~exp12ubuntu10.27

#RUN apt-get install -y software-properties-common=0.82.7.7
#RUN apt-get install -y python-software-properties=0.82.7.7
#RUN add-apt-repository http://ubuntu.mirror.cambrium.nl/ubuntu/

# C++ Pre-Reqs
#RUN apt-get install -y build-essential=11.5ubuntu2.1
RUN apt-get install -y coreutils=8.13-3ubuntu3.3
RUN apt-get install -y autoconf=2.68-1ubuntu2
RUN apt-get install -y libtool=2.4.2-1ubuntu1
RUN apt-get install -y pkg-config=0.26-1ubuntu1
RUN apt-get install -y gcc-multilib=4:4.6.3-1ubuntu5
RUN apt-get install -y g++-multilib=4:4.6.3-1ubuntu5
RUN apt-get install -y mingw-w64=2.0.1-1
RUN apt-get install -y g++-mingw-w64=4.6.3-1ubuntu5+5ubuntu1
RUN apt-get install -y gcc-mingw-w64=4.6.3-1ubuntu5+5ubuntu1
RUN apt-get install -y binutils-mingw-w64=2.22-2ubuntu1+1
RUN apt-get install -y nasm=2.09.10-1
RUN apt-get install -y yasm=1.1.0-1
RUN apt-get install -y doxygen=1.7.6.1-2ubuntu1
RUN apt-get install -y valgrind=1:3.7.0-0ubuntu3.1
RUN apt-get install -y graphviz=2.26.3-10ubuntu1.2
RUN apt-get install -y libpcre3-dev=8.12-4ubuntu0.2
RUN apt-get install -y ccache=3.1.6-1
RUN apt-get install -y git=1:1.7.9.5-1ubuntu0.3
RUN apt-get install -y make=3.81-8.1ubuntu1.1
RUN apt-get install -y build-essential=11.5ubuntu2.1
RUN apt-get install -y bison=1:2.5.dfsg-2.1
RUN apt-get install -y flex=2.5.35-10ubuntu3
# Java Pre-Reqs
RUN apt-get install -y openjdk-6-jdk=6b41-1.13.13-0ubuntu0.12.04.1
RUN apt-get install -y maven=3.0.4-2

RUN git clone https://github.com/artclarke/humble-swig.git && \
    cd humble-swig && \
    ./autogen.sh && \
    ./configure && \
    make && make install && \
    cd .. && rm -rf humble-swig

RUN apt-get clean -y

# Let's mount a volume for Maven caches and for CC Caches
VOLUME /caches
ENV HOME /caches
ENV PATH /usr/lib/ccache:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

# Now, let's mount the current directory because that's what
# we stage to.
VOLUME /source

# In theory, we are now GTG
WORKDIR /source
CMD ["bash"]
