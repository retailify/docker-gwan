# docker-gwan

This project contains the complete installation for the current version of [G-WAN](http://gwan.ch).

> G-WAN 7.12.6 64-bit (Feb  8 2016 16:33:28)

#### Build
> docker build -t gwan .

#### Run (static server)
> docker run -d -p 6070:80 -v /my/path/to/static/www:/opt/www gwan

##### Run (app server)
> docker run -d -p 6070:80 -v /my/path/to/static/www:/opt/www -v /my/path/to/dynamic/csp:/opt/csp gwan

##### Attention
The directory structure of G-WAN collides with the volume structure of Docker. So we created a symbolic link to the www directory. Furthermore you need to change your static file ownership to www-data:www-data. If you do not have www-data user and group, you can use the following command inside your volume mount.

> chown -R 33:33 .
