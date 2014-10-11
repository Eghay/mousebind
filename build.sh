files=$(find . -name "*.c" | tr '\n' ' ')
gcc $files -o mousebind -lX11 -lm -lXtst -lXinerama -I xdotool
