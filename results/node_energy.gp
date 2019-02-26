set term postscript eps enhanced color
set title "Avg. residual energy for 16 nodes network"
set xlabel "Time(s)"
set ylabel "Avg. residual energy"
set xrange[0:200]
set yrange[0:1000]
set output "avg_energy_16.eps"

plot "avg_energy_16.tr" using 1:2 title "Node 10 energy" with linespoints
