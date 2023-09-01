FROM debian:11
RUN apt-get -y update \
  && apt-get -y install build-essential \
  && apt-get -y install cmake \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app/MatchingEngine

COPY src src 
copy CMakeLists.txt . 

ARG TESTING 
RUN echo "TESTING="$TESTING
RUN cmake -B _build -H. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -DBUILD_FOR_TESTING=$TESTING && cmake --build _build -- install -j4

ARG CACHEBUST
RUN echo ${CACHEBUST} && cd _build && ctest -VV
