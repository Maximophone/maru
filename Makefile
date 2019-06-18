build_tests:
	g++ -std=c++14 tests_main.cpp -c

test:
	g++ -std=c++14 -o tests tests_main.o src/*.cpp && ./tests