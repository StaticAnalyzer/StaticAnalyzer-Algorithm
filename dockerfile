FROM ubuntu:22.04

COPY algServer /home/staticanalyzer/algServer
COPY deps /home/staticanalyzer/deps
WORKDIR /home/staticanalyzer

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/staticanalyzer/deps/
RUN sed -i 's/archive.ubuntu.com/mirrors.nju.edu.cn/g' /etc/apt/sources.list && \
    sed -i 's/security.ubuntu.com/mirrors.nju.edu.cn/g' /etc/apt/sources.list && \
    apt update && apt install build-essential

CMD ./algServer

EXPOSE 8081
