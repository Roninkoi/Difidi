#!/bin/sh

sed -i 's/.*rhopath diode.out/#rhopath diode.out/g' diode.in
sed -i 's/VG.*/VG 0.4/g' diode.in
./difidi diode.in diode.out > out.log

sed -i 's/.*rhopath diode.out/rhopath diode.out/g' diode.in

i=0
vg="0.5"
while [ $i -lt 20 ]; do
    echo "VG=${vg}"
    sed -i "s/VG.*/VG ${vg}/g" diode.in
    vg=$(echo $vg | awk '{print $1+0.05}')
    ./difidi diode.in diode.out > out.log || exit
    i=$((i+1))
done

