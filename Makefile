build_tests:
	g++ -std=c++11 tests_main.cpp -c

test:
	g++ -std=c++11 -o tests tests_main.o src/*.cpp && ./tests