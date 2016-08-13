FROM ubuntu:16.04
RUN apt-get update && apt-get install pciutils -y && rm -rf /var/lib/apt/lists/*
ADD . /opt
WORKDIR /opt
RUN chown -R www-data:www-data *
WORKDIR /opt/gwan
RUN ln -s "/opt/gwan/0.0.0.0:80/#0.0.0.0/www" /opt/www 
RUN chown -h www-data:www-data /opt/www
CMD [ "/opt/gwan/gwan" ]
