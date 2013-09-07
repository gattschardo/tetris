./tetris
mkdir -p graphs
DATE=`date +%Y.%m.%d-%H:%M`
gnuplot plot > graphs/g-$DATE.png
cp log graphs/log-$DATE
