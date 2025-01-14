#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
MD5_TO_DSMA=$TOOLS/md5_to_dsma/md5_to_dsma.py
IMG2DS=$TOOLS/img2ds/img2ds.py

mkdir -p nitrofiles

python3 $MD5_TO_DSMA \
    --model $ASSETS/robot/Robot.md5mesh \
    --name robot \
    --output nitrofiles \
    --texture 256 256 \
    --anim $ASSETS/robot/Wave.md5anim \
    --skip-frames 1 \
    --blender-fix

python3 $IMG2DS \
    --input $ASSETS/teapot.png \
    --name texture \
    --output nitrofiles \
    --format A1RGB5
