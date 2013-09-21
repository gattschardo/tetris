DATE=`date +%Y.%m.%d-%H:%M`
TETRIS='gt'
./$TETRIS
mkdir -p graphs
gnuplot plot > graphs/g-$DATE.png
cp log graphs/log-$DATE
