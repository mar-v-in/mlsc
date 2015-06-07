mlsc
====
`mlsc` is a client for the Mozilla Location Services targeting embedded systems.

The source for the request data (wi-fi, gsm, bluetooth) is decided at compile time.
Currently the only supported backend is OpenWRT's iwinfo.
If no backend is given at compilation, an empty request will be send, resulting in IP-based location lookup.

`mlsc` uses mbed TLS (polarssl) for SSL/TLS connection to the Mozilla server and no external library for HTTP.
The total compiled binary size is only about 8 KB.

Usage
-----

	Usage: mlsc <device> [-d]

The first argument is intended to contain device information and is directly send to the backend.
The iwinfo backend requires this to be the name of the wi-fi device (e.g. `wlan0`)

The second, optional argument `-d` enables debug output on stderr.

`mlsc` runs silently. The result will be given in JSON, reflecting the output of the Mozilla service:

	{"location": {"lat": 0, "lng": 0}, "accuracy": 0}

Compilation
-----------
The git repo comes with two `Makefile`s in it.
The outer `Makefile` is for the OpenWRT build system.
The inner `Makefile` is the actual compilation process.

### OpenWRT
If you want to build `mlsc` for OpenWRT, you need to set-up the OpenWRT build environment (e.g. using the SDK from their download servers)
Add the git repository to the packages sub-directory and compile it using `make packages/mlsc/compile`.
Remember to add polarssl and iwinfo to the OpenWRT build environment if you don't have them already. 
`mlsc` was tested with OpenWRT 14.07 Barrier Breaker only.

### Custom
You can compile `mlsc` for your system, by using `make` on the inner `Makefile`.
The resulting binary does not contain any backend. If you want to add the iwinfo backend, run `make -DUSE_IWINFO`

License
-------
This project is licensed under the terms of the GNU General Public License Version 2, see LICENSE.
