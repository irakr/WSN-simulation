set term postscript eps enhanced color
set title "Avg. residual energy for no. of nodes = 40"
set xlabel "Time(s)"
set ylabel "Avg. residual energy"
set xrange[0:500]
set yrange[0:1000]
set output "avg_energy_40.eps"

plot "avg_energy_40.tr" using 1:2 title "Avg. energy" with linespoints
