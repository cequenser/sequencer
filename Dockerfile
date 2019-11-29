FROM ubuntu:18.04 as build-env-gcc
LABEL description "build environment"
ENV CTEST_OUTPUT_ON_FAILURE=1
RUN apt-get update && apt-get install -y ninja-build g++-8 libboost-dev libalsa-ocaml-dev wget build-essential libssl-dev
RUN wget http://github.com/Kitware/CMake/releases/download/v3.16.0/cmake-3.16.0.tar.gz && tar xzf cmake-3.16.0.tar.gz && cd cmake-3.16.0 && ./bootstrap && make && make install

FROM build-env-gcc as build-env-release
LABEL description="release build environment"
RUN apt-get update && apt-get install -y clang-8 lld

FROM build-env-gcc as build-coverage
LABEL description="gcc debug build"
ENV coveralls_token "dummy"
ADD . /usr/local/src/sequencer
WORKDIR /usr/local/src/sequencer
RUN cmake -S . -B build -GNinja -DSEQUENCER_BuildTests=ON -DSEQUENCER_BuildExamples=ON -DSEQUENCER_Coverage=ON -DSEQUENCER_DisableAssert=ON -DCMAKE_BUILD_TYPE=Debug
RUN cmake --build build
RUN cmake --build build --target test
RUN apt-get update && apt-get install -y rbenv lcov
RUN gem install coveralls-lcov
RUN lcov --gcov-tool gcov --capture --no-external --directory build --base-directory include -rc lcov_branch_coverage=1 --output-file coverage.info 
CMD coveralls-lcov --repo-token ${coveralls_token} coverage.info

FROM build-env-release as build-clang-release
LABEL description="clang release build"
ADD . /usr/local/src/sequencer
WORKDIR /usr/local/src/sequencer
RUN cmake -S . -B build -GNinja -DCMAKE_CXX_COMPILER=clang++-8 -DCMAKE_C_COMPILER=clang-8 -DSEQUENCER_BuildTests=ON -DSEQUENCER_BuildExamples=ON -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build
RUN cmake --build build --target test
RUN cmake --build build --target install

FROM build-clang-release as test-installation-clang-release
LABEL description="test installation of clang release build"
ENV CTEST_OUTPUT_ON_FAILURE=1
COPY --from=build-clang-release /usr/local/include /usr/local/include
COPY --from=build-clang-release /usr/local/lib /usr/local/lib
COPY --from=build-clang-release /usr/local/lib/cmake/sequencer /usr/local/lib/cmake/sequencer
COPY --from=build-clang-release /usr/local/src/sequencer/tests /usr/local/tests
WORKDIR /usr/local/tests
RUN cmake -S . -B build -GNinja -DCMAKE_CXX_COMPILER=clang++-8 -DCMAKE_C_COMPILER=clang-8 -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build
RUN cmake --build build --target test

FROM build-clang-release as test-clang-tidy
LABEL description="clang tidy environment"
ADD . /usr/local/src/sequencer
WORKDIR /usr/local/src/sequencer
RUN apt-get update && apt-get install -y clang-tidy-8
RUN cmake -S . -B build -GNinja -DCMAKE_CXX_COMPILER=clang++-8 -DCMAKE_C_COMPILER=clang-8 -DSEQUENCER_BuildTests=ON -DSEQUENCER_BuildExamples=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cd build && run-clang-tidy-8.py -header-filter='(include/sequencer|tests).*'
