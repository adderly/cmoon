#!/bin/sh


if [ -z "$1" ] ; then
  echo "Usage: $0 data <title>"
  exit 1
fi

datafile=${1}
dataname=`echo ${datafile}|sed -e 's/\.dat$//'`

if [ ! -f ${datafile} ] ; then
   echo  "could not find ${datafile}"
   exit 1;
fi


title=${dataname}
if [ ! -z "$2" ] ; then
  title=$2
fi

cat - > tmp.gnu << EOF

set terminal  png
set output "${dataname}.png"

set title "$title"

set xlabel "#try"
set ylabel "time [second]"
pl '${datafile}' u 1:2 w l title '4KB page' , '${datafile}' u 1:3 w l title '4MB page'

repl
EOF

gnuplot tmp.gnu
rm -f tmp.gnu
