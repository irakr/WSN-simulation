set terminal png size 800,600
set title "Avg. residual energy vs Time for different total nodes"
set xlabel "Time(s)"
set ylabel "Avg. residual energy"
set xrange[0:500]
set yrange[0:1000]
set output "avg_energy.png"

plot "avg_energy_16_refined.tr" using 1:2 title "Nodes = 16" with linespoints, \
	"avg_energy_40_refined.tr" using 1:2 title "Nodes = 40" with linespoints, \
	"avg_energy_64_refined.tr" using 1:2 title "Nodes = 64" with linespoints, \
	"avg_energy_94_refined.tr" using 1:2 title "Nodes = 94" with linespoints
