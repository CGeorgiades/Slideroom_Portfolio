PROOT=./
BDIR=${PROOT}build/

OBJS=${BDIR}main.o
OUTP=${BDIR}Fourier

CXX=g++
CFLAGS= 
LDFLAGS= -lglut -lGL -lGLU -pthread

all: DIR ${OUTP}

DIR:
	mkdir -p ${BDIR}

${OUTP}: ${OBJS}
	g++ $^ ${LDFLAGS} -o $@ 

${BDIR}%.o: ${PROOT}%.cpp
	$(CXX) -c ${CFLAGS} $< -o $@
	
${PROOT}main.cpp: ${PROOT}fourier.hpp ${PROOT}my_draw.hpp

${PROOT}fourier.hpp: ${PROOT}calculus.hpp

clean:
	rm ${OBJS} ${OUTP}