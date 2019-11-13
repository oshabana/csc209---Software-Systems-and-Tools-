#!/bin/bash

`cd `
#grade=`tail -n 1 feedback.txt |  tr -d [:blank:]`
totala=0
totalt=0
maxa=0
maxt=0
 
for feedback in a1 a2 a3 a4; do
	for dir in */; do
		`cd $dir`
        	if [ -e /$feedback ]; then
			`cd /$feedback`
			if [ -e feedback.txt ]; then
				grade=`tail -n 1 /$feedback/feedback.txt | tr -d [:blank:]`
				IFS='/' read -ra grArray <<< "$grade"
				score=${grArray[0]}
				max=${grArray[1]}
				echo "$feedback: $score / $max"
				totala=$(($totala + $score))
				maxa=$($maxa + max)
				echo "$totala"
			else
				echo "$feedback: - / -"

			fi
		
		fi
		`cd  `
	done
done
echo
`cd `
for feedback in t01 t02 t03 t04 t05 t06 t07 t08 t09 t10 t11; do
        if [ -e /$feedback ]; then
		`cd /$feedback/`
		if [ -e $feedback/feedback.txt ]; then
			grade=`tail -n 1 /$feedback/feedback.txt | tr -d [:blank:]`
			IFS='/' read -ra grArray <<< "$grade"
			score=${grArray[0]}
			max=${grArray[1]}
			echo "$feedback: $score / $max"
			totalt=$(($totalt + $score))
			maxa=$($maxt + max)

		else

			echo "$feedback: - / -"

		fi
	else 
		echo "$feedback: - / -"

	fi
	`cd ..`
done
echo
echo "Assignment Total: $totala / $maxa"
echo "Tutorial Total: $totalt / $maxt"
#grade=`tail -n 1 feedback.txt`
#IFS='/' read -ra grArray <<< "$grade"
#score=${grArray[0]}
#max=${grArray[1]}
#IFS='/' read -ra grArray <<< "$grade"
#score=${grArray[0]}
#max=${grArray[1]}
#echo $score / $max


