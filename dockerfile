FROM ubuntu:22.04

COPY algServer /home/staticanalyzer/algServer
COPY deps /home/staticanalyzer/deps
WORKDIR /home/staticanalyzer

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/staticanalyzer/deps/

CMD ./algServer

EXPOSE 8081
