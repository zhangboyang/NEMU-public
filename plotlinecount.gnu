# this is a gnuplot program


# set range
set yrange [-100:]
set ytics 1000
set mytics 2




#unixtime fail other  nemu nemu-space  kernel kernel-space  game game-space  testcase testcase-space  lib-common lib-common-space
#1        2    3      4    5           6      7             8    9           10       11              12         13

# set offset value
nemu_initial = 3722
nemu_space_initial = 766
kernel_initial = 1277
kernel_space_initial = 261
game_initial = 79910
game_space_initial = 5647





# set graph options
set grid
set xtics nomirror
set autoscale xfix
set autoscale x2fix
set key top left
set xdata time
set timefmt "%s"
set format x "%Y-%m-%d\n%H:%M"

tzoffset=8*60*60
plot lcfile using ($1+tzoffset):($4+$6+$8-nemu_initial-kernel_initial-game_initial) title "total" with linespoints pointtype 1,\
'' using ($1+tzoffset):($5+$7+$9-nemu_space_initial-kernel_space_initial-game_space_initial) title "total-space" with linespoints pointtype 1,\
'' using ($1+tzoffset):($4-nemu_initial) title "nemu-total" with linespoints pointtype 1,\
'' using ($1+tzoffset):($6-kernel_initial) title "kernel-total" with linespoints pointtype 1,\
'' using ($1+tzoffset):($8-game_initial) title "game-total" with linespoints pointtype 1

pause -1
