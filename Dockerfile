FROM ghcr.io/mormj/gr4-oot-env-incubator:latest

COPY ./backend/ /code/

RUN mkdir build
RUN cmake -S . -B build
RUN cmake --build build -j2
