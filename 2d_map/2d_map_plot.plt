set terminal svg
set output "2d_map_plot.bmp"
set title "Carte 2D (plage max : 0-57°, ~50-8000mm)"
set xlabel "Angle (°)"
set ylabel "Distance (mm)"
plot "./2d_map_plot.dat" using 1:2 title ""
