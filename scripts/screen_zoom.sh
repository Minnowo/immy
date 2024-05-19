#!/bin/sh

# find the screen width and height
read width height <<< $(xdpyinfo | awk '/dimensions:/ {split($2, dims, "x"); print dims[1], dims[2]}')

# # this also works;
# width_height=$(xdpyinfo | grep dimensions | awk '{print $2}')
# width=$(echo "$width_height" | awk -Fx '{print $1}')
# height=$(echo "$width_height" | awk -Fx '{print $2}')

# where we will save the screenshot
# bmp was the fastest from my testing
PICTURE=/tmp/doko_screen.bmp

# take the screenshot
# -Z 1   if png; pretty fast and fair enough compression
scrot -z -o -q 100 -Z 1 "$PICTURE"

# run doko so that it covers all monitors and starts at 0 0
doko -x 0 -y 0 -w "$width" -h "$height" -d -C -B -t "$PICTURE" "$PICTURE" 

# delete the image
rm "$PICTURE"

