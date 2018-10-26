CXX=g++
CXXFLAGS= -std=c++11 -o tojsdl -g

OFILES = script.cpp

script:	$(OFILES)
		$(CXX) $(OFILES) $(CXXFLAGS)
clean:
		rm -f tojsdl
