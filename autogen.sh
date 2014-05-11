#!/bin/sh
./update.sh
mkdir -p m4
autoreconf -i -f
(cd libottery; chmod 755 autogen.sh; ./autogen.sh)
