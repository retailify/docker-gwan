---------------------------------------------------------------
G-WAN/Linux (32-bit and 64-bit)                      readme.txt
---------------------------------------------------------------
Program: G-WAN
Website: http://gwan.ch
Authors: http://trustleap.ch
License: freeware, free for commercial and non-profit users
Release: first published in June 2009, maintained by TrustLeap

G-WAN is a Web App. server with scripts in 16 languages (ASM,
C/C++, C#, Java, D, Objective-C, Objective-C++, Go, Javascript,
Lua, Perl, PHP, Python, Ruby, Scala) and a TCP reverse-proxy
with elastic load-balancing.

Despite a small-footprint (a single file < 200 KB) and a zero
-configuration design (no configuration file) G-WAN is faster,
uses less resources and is more scalable than the other servers
written for any of G-WAN's supported programming languages.

G-WAN also offers a 'wait-free' a KV store, in-memory GIF I/O,
fast bar/dot/line/pie/ring charts and 2D frame-buffer drawing,
compression, encryption, etc.

In daemon mode (sudo ./gwan -d:www-data) G-WAN automatically
minifies HTML, CSS and Javascript files to reduce latency. It
even turns small CSS images into Data URIs to save connections.

G-WAN lets you focus on your applications rather than resolve
concurrency challenges or learn obscure configuration details.

Feel free to distribute it around you and to spread the word!

---------------------------------------------------------------
Archive Contents
---------------------------------------------------------------
- gwan ............ G-WAN web application server executable
- hello.c ......... G-WAN-unrelated (general-purpose) example
- init.c_ ......... G-WAN initialization script example
- main.c_ ......... G-WAN maintenance script example
- /logs ........... G-WAN's log file
- /.../csp ........ G-WAN C/C++ and Objective-C/C++ examples
- /.../www ........ G-WAN app. server 'root' directory
- /.../handlers ... G-WAN app. server handler examples
- /.../logs ....... G-WAN app. server access/error log files
- readme.txt ...... the document you are reading
- license.txt ..... the license agreement (for humans)

In some of the directories listed above, you will find further 
README.txt files to guide you in case of doubt.

---------------------------------------------------------------
Installation
---------------------------------------------------------------
1) decompress the gwan_xxx.tar.bz2 archive under '/opt/gwan'
2) run G-WAN with 'cd /opt/gwan' and then './gwan'
3) start your Internet browser and enter: http://127.0.0.1:8080
4) test the many examples provided in the archive
5) read the G-WAN manual to use all G-WAN's features

---------------------------------------------------------------
Removal
---------------------------------------------------------------
G-WAN does not alter the system, does not add shared libraries, 
does not create dependencies or configuration files that could
interfere with other programs so you can 'uninstall' it by just
stopping the gwan process and deleting its directory.

---------------------------------------------------------------
Use is subject to license terms, read the attached license.txt.
---------------------------------------------------------------

