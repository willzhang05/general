CXX=g++
CXXFLAGS= -std=c++11 -o tojsdl

OFILES = script.cpp

script:	$(OFILES)
		$(CXX) $(OFILES) $(CXXFLAGS)
