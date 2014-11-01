all: work

%: %.cc
	g++ -std=c++11 $< -o $@

