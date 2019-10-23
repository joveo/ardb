#!/bin/bash

start() {
   echo "starting dynomite... "
   cd /home/ubuntu/ardb
   sudo ./src/ardb-server ardb.conf
}

stop() {
   echo "stop"
   PID=`pgrep ardb`
   if [[ "" !=  "$PID" ]]; then
      echo "killing $PID"
      sudo kill -9 $PID
   fi
}

case "$1" in start)
  start
;;
  stop)
  stop
;;
*)

echo $"Usage: $0 {start|stop}"
RETVAL=1
esac
exit 0