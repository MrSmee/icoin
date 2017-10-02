#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/icoin.png
ICON_DST=../../src/qt/res/icons/icoin.ico
convert ${ICON_SRC} -resize 16x16 icoin-16.png
convert ${ICON_SRC} -resize 32x32 icoin-32.png
convert ${ICON_SRC} -resize 48x48 icoin-48.png
convert icoin-16.png icoin-32.png icoin-48.png ${ICON_DST}

