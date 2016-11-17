idy003 : idy003.cpp
	g++  -Wall -Wextra -Wconversion -g idy003.cpp -o idy003
Debug : idy003
	gdb --silent idy003
Fightme : idy003
	python3 ./main.py ./idy003 ./idy003
Fight : idy003
	python3 ./main.py ./idy003 ./idy003.old
Show :
	./Demon ./result.json
