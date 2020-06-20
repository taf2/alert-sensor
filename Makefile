all: play.c
	gcc -Wall play.c -o play -lwiringPi -lmad -lpulse -lpulse-simple
clean:
	rm -f play
