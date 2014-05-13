#!/bin/sh
./update.sh
mkdir -p m4
autoreconf -i -f
(cd libottery; ./autogen.sh)
