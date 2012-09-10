Unified flash remoting server
=====================

This is a Drupal 6 module for providing an unified services 3 server interface for exposing resources to various AMF capable clients.

**Backend libraries that can be used are:*

> AMFPHP 1.9beta2 from http://sourceforge.net/projects/amfphp/files/amfphp/amfphp%201.9%20beta2/

> SabreAMF from https://github.com/evert/SabreAMF

> ZendAMF 1.12 from http://packages.zendframework.com/releases/ZendAMF-1.12.0/ZendAMF-1.12.0.zip

Notes:
- favoring SabreAMF
- ZendAMF error handling for missing resources/methods is missing (error output is not AMF encoded)