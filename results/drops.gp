set term png size 800,600
set title "Packet drop ratio"
set xlabel "No. of nodes"
set ylabel "Packet drop (percentage)"
set xrange[0:100]
set yrange[0:100]
set output "pdr_all.png"

plot "pdr_all.tr" using 1:2 title "Packet drop (%)" with linespoints
