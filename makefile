# just compile
all:
	@g++ main.cpp -c -lncursesw
	@g++ main.cpp -o main -lncursesw

# compile and run
run:
	@g++ main.cpp -c -lncursesw
	@g++ main.cpp -o main -lncursesw
	@./main
