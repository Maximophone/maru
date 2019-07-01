build_tests:
	g++ -std=c++14 tests_main.cpp -c

test:
	g++ -std=c++14 -o tests tests_main.o src/*.cpp src/tests/*.cpp && ./tests

test_no_run:
	g++ -std=c++14 -o tests tests_main.o src/*.cpp src/tests/*.cpp

main:
	g++ -std=c++14 -o maru main.cpp src/*.cpp