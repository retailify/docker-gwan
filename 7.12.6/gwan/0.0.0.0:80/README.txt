
G-WAN uses the file system directory hierarchy to define which IP address
and port number to LISTEN to.

This LISTENER directory contains the HOSTS directories.

Directory names prefixed with a:

  - '#' are the "root" HOST (the default host)
  - '$' are "virtual" HOSTS (identified by the "Host: xxx" HTTP header)
  
All other files or directories are ignored. This allows you to disable
HOSTS by renaming their directory:

   enabled       disabled
  ----------    -----------
  "#0.0.0.0" => "_#0.0.0.0"

For more information, read the http://gwan.ch/faq or the PDF manual available
at http://gwan.ch/download
