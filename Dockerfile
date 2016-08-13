FROM ubuntu:16.04
RUN apt-get update && apt-get install pciutils -y && rm -rf /var/lib/apt/lists/*
ADD . /opt
WORKDIR /opt
RUN chown -R www-data:www-data *
WORKDIR /opt/gwan
CMD [ "/opt/gwan/gwan" ]
