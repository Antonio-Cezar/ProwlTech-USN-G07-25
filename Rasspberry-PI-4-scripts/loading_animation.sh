#!/bin/bash

loading_animasjon() {
    local i=0
    local symboler=('|' '/' '-' '\\')
    echo -n "Laster  "
    while [ $i -lt 4 ]; do
        for s in "${symboler[@]}"; do
            echo -ne "\rLaster $s"
            sleep 0.125
        done
        ((i++))
    done
    echo -e "\rModul klar!         "
}

loading_animasjon
