.PHONY = all clean

CXXFLAGS += -Wall -pedantic --std=c++17 -g

all: t_gui

gui.cpp: gui.h
	touch $@

t_gui.cpp: gui.h
	touch $@

t_gui: t_gui.cpp gui.cpp

clean:
	rm -f t_gui
