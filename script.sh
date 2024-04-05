#!/bin/bash
# This script will run 360 times the cpp executable with the input parameter corresponding to the number of the iteration
for i in {0..359}
do
    ./build/engine $i
done
echo "TGA files generated"

# Convert the output to png
mogrify -format png out/*.tga
echo "PNG files generated"

# Remove the tga files
rm out/*.tga
echo "TGA files removed"

# Create a gif
# convert -delay 5 -loop 0 out/*.png out.gif
# echo "GIF file generated"

# Remove the png files
# rm out/*.png
# echo "PNG files removed"