# docker-gwan

This project contains the complete installation for the current version of gwan.

> G-WAN 7.12.6 64-bit (Feb  8 2016 16:33:28)

#### Build
> docker build -t gwan .

#### Run
> docker run -d -p 6070:80 -v /my/path/to/static/www:/opt/www gwan
