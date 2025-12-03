FROM ghcr.io/mormj/gr4-oot-env-incubator:latest

COPY ./backend/ /code/

RUN mkdir build
RUN cmake -S . -B build
WORKDIR /code/build
RUN make -j3 baseline
RUN make -j3 all
