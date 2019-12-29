mkdir tmp

printf "\n===[Small Image (320x240)]===\n"
python resize.py --img_name="airplane" --height=240 --width=320 --save_name=tmp/small
realtime1=$(mktemp)
realtime2=$(mktemp)
\time -o "${realtime1}" -f "%e" ./serial_histogram tmp/small.bmp &
\time -o "${realtime2}" -f "%e" ./opencl_histogram tmp/small.bmp &
wait
printf "Serial histogram: %s (sec)\n" $(cat "${realtime1}")
printf "OpenCL histogram: %s (sec)\n" $(cat "${realtime2}")

printf "\n===[Mid Image (1920x1080)]===\n"
python resize.py --img_name="airplane" --height=1080 --width=1920 --save_name=tmp/mid
realtime1=$(mktemp)
realtime2=$(mktemp)
\time -o "${realtime1}" -f "%e" ./serial_histogram tmp/mid.bmp &
\time -o "${realtime2}" -f "%e" ./opencl_histogram tmp/mid.bmp &
wait
printf "Serial histogram: %s (sec)\n" $(cat "${realtime1}")
printf "OpenCL histogram: %s (sec)\n" $(cat "${realtime2}")

printf "\n===[Large Image (7680x4320)]===\n"
python resize.py --img_name="airplane" --height=4320 --width=7680 --save_name=tmp/large
realtime1=$(mktemp)
realtime2=$(mktemp)
\time -o "${realtime1}" -f "%e" ./serial_histogram tmp/large.bmp &
\time -o "${realtime2}" -f "%e" ./opencl_histogram tmp/large.bmp &
wait
printf "Serial histogram: %s (sec)\n" $(cat "${realtime1}")
printf "OpenCL histogram: %s (sec)\n" $(cat "${realtime2}")

printf "\n===[16K Image (15360x8640)]===\n"
python resize.py --img_name="airplane" --height=8640 --width=15360 --save_name=tmp/large
realtime1=$(mktemp)
realtime2=$(mktemp)
\time -o "${realtime1}" -f "%e" ./serial_histogram tmp/large.bmp &
\time -o "${realtime2}" -f "%e" ./opencl_histogram tmp/large.bmp &
wait
printf "Serial histogram: %s (sec)\n" $(cat "${realtime1}")
printf "OpenCL histogram: %s (sec)\n" $(cat "${realtime2}")

rm -rf tmp
