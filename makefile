CXX=gcc
CXXFLAGS= -std=c++11

OFILES = script.cpp

script:	$(OFILES)
		$(CXX) $(OFILES) $(CXXFLAGS)
