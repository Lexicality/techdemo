#!/bin/bash
#
# XCode Beheader:
#  This program removes the default XCode header and replaces it with my own

MIT='// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
// Please read the LICENSE file for more information.
//';

for file in $(find . -name "*.cpp" -or -name "*.h")
do
    base=$(basename $file);
    headb=$(head -n4 $file);
    heada="//
//  $base
//  TechDemo
//";
    nicename=${file#./}
    if [[ ! $heada == $headb ]]
    then
        continue;
    fi
    echo "Found a default XCode header in $nicename!";
    beheaded=$(tail +8 $file);
    echo "//
// TechDemo/$nicename
$MIT
$beheaded" > $file;
done
