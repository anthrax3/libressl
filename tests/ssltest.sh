#!/bin/sh
export PATH=../apps:$PATH
./testssl server.pem server.pem ca.pem
