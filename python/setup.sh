#!/bin/bash  -i
# Use: source setup.sh
export PYTHONPATH="$PYTHONPATH:"`dirname $(readlink -f ${BASH_SOURCE[0]})`
export PATH=$PATH:`dirname $(readlink -f ${BASH_SOURCE[0]})`/http
export PATH=$PATH:`dirname $(readlink -f ${BASH_SOURCE[0]})`/messaging
