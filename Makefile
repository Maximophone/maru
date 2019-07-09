build_tests:
	g++ -std=c++14 tests_main.cpp -c

test:
	g++ -std=c++14 -o tests tests_main.o src/*.cpp src/tests/*.cpp src/maru_std/*.cpp src/maru_std/c_libraries/*.cpp && ./tests

test_no_run:
	g++ -std=c++14 -o tests tests_main.o src/*.cpp src/tests/*.cpp src/maru_std/*.cpp src/maru_std/c_libraries/*.cpp

main:
	g++ -std=c++14 -o maru main.cpp src/*.cpp src/maru_std/*.cpp src/maru_std/c_libraries/*.cpp