set terminal png size 800,600
set title "No. of dead nodes with time"
set xlabel "Time(s)"
set ylabel "No. of dead nodes"
set xrange[0:500]
set yrange[0:100]
set output "dead_nodes.png"

plot "dead_nodes_16.tr" using 1:2 title "Nodes = 16" with linespoints, \
	"dead_nodes_40.tr" using 1:2 title "Nodes = 40" with linespoints, \
	"dead_nodes_64.tr" using 1:2 title "Nodes = 64" with linespoints, \
	"dead_nodes_94.tr" using 1:2 title "Nodes = 94" with linespoints
