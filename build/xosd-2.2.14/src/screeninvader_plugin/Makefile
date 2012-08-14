    TARGET  := osd 
    SRCS    := osd.cpp
    OBJS    := ${SRCS:.cpp=.o} 
    DEPS    := ${SRCS:.cpp=.dep} 

    CXXFLAGS = -I../ -O3 -Wall `pkg-config --cflags gtk+-2.0`
    LDFLAGS = -s 
    LIBS    = -lxosd  `pkg-config --libs gtk+-2.0`

    .PHONY: all clean distclean 
    all:: ${TARGET} 

    ifneq (${XDEPS},) 
    include ${XDEPS} 
    endif 

    ${TARGET}: ${OBJS} 
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS} 

    ${OBJS}: %.o: %.cpp %.dep 
	${CXX} ${CXXFLAGS} -o $@ -c $< 

    ${DEPS}: %.dep: %.cpp Makefile 
	${CXX} ${CXXFLAGS} -MM $< > $@ 

    clean:: 
	-rm -f *~ *.o ${TARGET} 

    distclean:: clean

