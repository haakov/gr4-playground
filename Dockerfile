FROM ghcr.io/mormj/gr4-oot-env-incubator:latest

COPY ./backend/ /code/

RUN mkdir -p build
WORKDIR /code/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Debug
RUN make -j2
