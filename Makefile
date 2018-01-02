CCX=g++
CXXFLAGS= -g --std=c++11
OBJS= src/gestures/HandGesture.cpp src/mask_edition/MyBGSubtractorColor.cpp src/main.cpp

OPENCV= `pkg-config --cflags opencv` `pkg-config --libs opencv`

all: ${OBJS}
	$(CCX) $(CXXFLAGS) -o mano_recognition ${OBJS} $(OPENCV)
