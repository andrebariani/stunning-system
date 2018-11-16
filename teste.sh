##!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

if [ ! -f casos/$1.in ]; then
    echo "Caso de teste $1.in não encontrado!"
    exit
fi

echo -n "Executando caso " && cat casos/descricao.txt | grep ^$1.in

timeout 0.1s ./main < casos/$1.in > $1.out

if [[ $? -eq 124 ]]; then
    echo -e "${RED}Timeout!${NC}"
    rm $1.out
    exit
fi

diff=$(colordiff --side-by-side casos/$1.out $1.out)

if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}Yay!${NC}\n\n$diff" | less -R
else
    echo -e "${RED}Nay...${NC}\n\n$diff" | less -R
fi

rm $1.out
