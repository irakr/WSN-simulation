set term postscript eps enhanced color
set title "Avg. residual energy for no. of nodes = 64"
set xlabel "Time(s)"
set ylabel "Avg. residual energy"
set xrange[0:500]
set yrange[0:1000]
set output "avg_energy_64.eps"

plot "avg_energy_64.tr" using 1:2 title "Avg. energy" with linespoints
