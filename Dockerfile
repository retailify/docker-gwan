FROM ubuntu:16.04
RUN apt-get update && apt-get install pciutils build-essential -y && rm -rf /var/lib/apt/lists/*
ADD . /opt
WORKDIR /opt
RUN chown -R www-data:www-data *
WORKDIR /opt/gwan
RUN ln -s "/opt/gwan/0.0.0.0:80/#0.0.0.0/www" /opt/www
RUN ln -s "/opt/gwan/0.0.0.0:80/#0.0.0.0/csp" /opt/csp
RUN chown -h www-data:www-data /opt/www \
    && chown -h www-data:www-data /opt/csp
CMD [ "/opt/gwan/gwan" ]
