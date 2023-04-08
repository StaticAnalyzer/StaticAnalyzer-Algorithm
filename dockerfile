FROM hebryan/staticanalyzer-webui-devimage

COPY algServer ~/staticanalyzer/algServer
WORKDIR ~/staticanalyzer

CMD ./algServer

EXPOSE 8081
