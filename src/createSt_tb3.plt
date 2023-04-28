set size ratio -1
set nokey
set xrange[-5:105]
set yrange[-5:105]
set object 1 rect from 0,0 to 100,100fc rgb "grey" behind
set object circle at first 10,73 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 10,73 radius char 0.15 fillstyle solid fc rgb "red" front
set object circle at first 62,9 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 62,9 radius char 0.15 fillstyle solid fc rgb "red" front
set object circle at first 74,61 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 74,61 radius char 0.15 fillstyle solid fc rgb "red" front
set object circle at first 57,61 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 57,61 radius char 0.15 fillstyle solid fc rgb "red" front
set object circle at first 15,16 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 15,16 radius char 0.15 fillstyle solid fc rgb "red" front
set arrow 2 from 57,61 to 57,61 nohead lc rgb "red" lw 1.5 back
set arrow 3 from 57,61 to 74,61 nohead lc rgb "red" lw 1.5 back
set arrow 4 from 62,9 to 62,9 nohead lc rgb "orange" lw 1.5 back
set arrow 5 from 50,97 to 50,97 nohead lc rgb "red" lw 1.5 back
set arrow 6 from 34,35 to 34,42 nohead lc rgb "orange" lw 1.5 back
set arrow 7 from 26,35 to 34,35 nohead lc rgb "orange" lw 1.5 back
set arrow 8 from 34,42 to 50,42 nohead lc rgb "orange" lw 1.5 back
set arrow 9 from 50,34 to 50,42 nohead lc rgb "orange" lw 1.5 back
set arrow 10 from 50,34 to 88,34 nohead lc rgb "orange" lw 1.5 back
set arrow 11 from 84,52 to 84,58 nohead lc rgb "red" lw 1.5 back
set arrow 12 from 84,58 to 95,58 nohead lc rgb "red" lw 1.5 back
set arrow 13 from 51,81 to 51,81 nohead lc rgb "orange" lw 1.5 back
set arrow 14 from 28,81 to 51,81 nohead lc rgb "orange" lw 1.5 back
set arrow 15 from 28,80 to 28,81 nohead lc rgb "orange" lw 1.5 back
set arrow 16 from 55,79 to 55,89 nohead lc rgb "red" lw 1.5 back
set arrow 17 from 55,79 to 80,79 nohead lc rgb "red" lw 1.5 back
set arrow 18 from 80,60 to 80,79 nohead lc rgb "red" lw 1.5 back
set arrow 19 from 80,60 to 94,60 nohead lc rgb "red" lw 1.5 back
set arrow 20 from 21,29 to 21,29 nohead lc rgb "orange" lw 1.5 back
set object circle at first 88,34 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 88,34 radius char 0.15 fillstyle solid fc rgb "orange" front
set object circle at first 41,4 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 41,4 radius char 0.15 fillstyle solid fc rgb "orange" front
set object circle at first 50,97 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 50,97 radius char 0.15 fillstyle solid fc rgb "orange" front
set object circle at first 34,42 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 34,42 radius char 0.15 fillstyle solid fc rgb "orange" front
set object circle at first 26,35 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 26,35 radius char 0.15 fillstyle solid fc rgb "orange" front
set object circle at first 95,58 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 95,58 radius char 0.15 fillstyle solid fc rgb "yellow" front
set object circle at first 14,59 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 14,59 radius char 0.15 fillstyle solid fc rgb "yellow" front
set object circle at first 51,81 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 51,81 radius char 0.15 fillstyle solid fc rgb "yellow" front
set object circle at first 84,52 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 84,52 radius char 0.15 fillstyle solid fc rgb "yellow" front
set object circle at first 28,80 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 28,80 radius char 0.15 fillstyle solid fc rgb "yellow" front
set object circle at first 94,60 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 94,60 radius char 0.15 fillstyle solid fc rgb "green" front
set object circle at first 21,29 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 21,29 radius char 0.15 fillstyle solid fc rgb "green" front
set object circle at first 8,37 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 8,37 radius char 0.15 fillstyle solid fc rgb "green" front
set object circle at first 80,79 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 80,79 radius char 0.15 fillstyle solid fc rgb "green" front
set object circle at first 55,89 radius char 0.3 fillstyle solid fc rgb "black" front
set object circle at first 55,89 radius char 0.15 fillstyle solid fc rgb "green" front
plot 1000000000
pause -1 'Press any key'
