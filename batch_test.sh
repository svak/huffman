#!/bin/sh

if [ -t 1 ]
then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    NC='\033[0m'
fi

if [ ! -e encode ] || [ ! -e decode ];
then
    printf "Tools not found."
    exit 1
fi

result=0

totalSize=0
compressedSize=0

for file in ./*
do
    origmd5=($(md5sum -b $file))
    origSize=$(wc -c < "$file")

    totalSize=$(($totalSize + $origSize))

    ./encode $file /tmp/encoded
    encodedSize=$(wc -c < "/tmp/encoded")

    compressedSize=$(($compressedSize + $encodedSize))

    ./decode /tmp/encoded /tmp/decoded
    newmd5=($(md5sum -b /tmp/decoded))

    if (($encodedSize > $origSize )); then
        face="[-]"
    else
        face="[+]"
    fi

    info="$origSize - $encodedSize = $(($origSize - $encodedSize))"

    if [ $origmd5 != $newmd5 ];
    then
        printf "${RED}FAIL${NC} $face $origmd5 $newmd5 $file ($info)\n"
        result=1
    else
        printf "${GREEN}PASS${NC} $face $file ($info)\n"
    fi
done

ratio=$((100 - $compressedSize * 100 / $totalSize))

printf "== TOTAL\n"
printf "   ORIGINAL: $totalSize\n"
printf " COMPRESSED: $compressedSize\n"
printf "       DIFF: $(($totalSize - $compressedSize)) ($ratio%%)\n"

exit $result

