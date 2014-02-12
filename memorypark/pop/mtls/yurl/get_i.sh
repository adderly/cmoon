#!/bin/sh

BGN=0
END=500000

for i in `seq ${BGN} ${END}`; do
    echo ">>>Process $i ..."
    wget --quiet --timeout=5 --tries=1 http://www.yellowurl.cn/${i}.html
    sleep 1
done

echo "################## DONE ##################"
