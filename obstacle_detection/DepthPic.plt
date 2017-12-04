set terminal png
set output "DepthPic.png"

unset key
unset tics
unset border
unset colorbox
#set autoscale fix

set pm3d map
splot "DepthPic.txt" matrix
