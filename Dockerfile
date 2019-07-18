FROM ubuntu:18.04
MAINTAINER cequenser "cequenser@gmail.com"

RUN apt-get update && apt-get install -y software-properties-common
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get update && apt-get install -y rbenv git gcc-8 g++-8 libboost-dev wget clang-8 clang-tidy-8 clang-format-6.0 ninja-build
RUN wget http://github.com/Kitware/CMake/releases/download/v3.13.1/cmake-3.13.1.tar.gz && tar xzf cmake-3.13.1.tar.gz && cd cmake-3.13.1 && ./bootstrap && make && make install
RUN wget http://ftp.de.debian.org/debian/pool/main/l/lcov/lcov_1.14-1_all.deb && dpkg -i lcov_1.14-1_all.deb
RUN gem install coveralls-lcov

WORKDIR /home
