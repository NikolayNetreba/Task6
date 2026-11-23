CXX = g++
CXXFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE -Iinclude

all: akinator

akinator: obj/PathStack.o obj/main.o
	$(CXX) $(CXXFLAGS) $^ -o akinator

obj/%.o: src/%.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/PathStack.o: include/PathStack.h
obj/main.o: include/PathStack.h

clear:
	rm -f akinator
	rm -rf obj
