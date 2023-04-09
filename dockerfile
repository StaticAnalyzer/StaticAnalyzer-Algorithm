FROM hebryan/staticanalyzer-webui-devimage-minimal:v1.4.1

COPY algServer ~/staticanalyzer/algServer
WORKDIR ~/staticanalyzer

CMD ./algServer

EXPOSE 8081
