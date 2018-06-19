#!/bin/sh -x

BINDER=$(command -v afb-daemon)
CLIENT=$(command -v afb-client-demo)
AFBTEST="$(pkg-config --variable libdir afb-test)/aft.so"
PROCNAME="aft-low-can"
PORT=1234
TOKEN=
LOGPIPE="test.log"
[ "$1" ] && BUILDDIR="$1" || exit 1

[ ! -p $LOGPIPE ] && mkfifo $LOGPIPE

pkill $PROCNAME

${BINDER} --name="${PROCNAME}" \
--port="${PORT}" \
--roothttp=. \
--tracereq=common \
--token=${TOKEN} \
--workdir="${BUILDDIR}/package" \
--binding="lib/afb-low-can.so" \
--binding="$AFBTEST" \
-vvv > ${LOGPIPE} 2>&1 &

while read -r line
do
	[ "$(echo "${line}" | grep 'NOTICE: Browser URL=')" ] && break
done < ${LOGPIPE}

${CLIENT} ws://localhost:${PORT}/api?token=${TOKEN} afTest launch_all_tests
