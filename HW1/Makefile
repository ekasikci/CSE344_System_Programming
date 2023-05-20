
all: appendMeMore

appendMeMore: main.o hw1q2.o hw1q3.o
	g++ -o appendMeMore main.o hw1q2.o hw1q3.o

main.o: hw1.c
	g++ -c hw1.c -o main.o
	
q2.o: hw1q2.c
	g++ -c hw1q2.c -o hw1q2.o
	
q3.o: hw1q3.c
	g++ -c hw1q3.c -o hw1q3.o

clean:
	rm -rf *o appendMeMore
