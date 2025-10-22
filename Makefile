SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=build/%.o)
LD = clang++
CXX = clang++
CXXFLAGS = -Wall -std=c++23
OUT = bin/pldec

build/%.o: %.cpp
	$(shell echo 1>&2 -e "Compiling \e[1m\e[32m$<\e[0m")

	@$(CXX) $(CXXFLAGS) -c -ggdb -o $@ $<

.PHONY: pldec
pldec: $(OUT)
$(OUT): $(OBJS)
	@$(LD) $(LDFLAGS) $(OBJS) -o $(OUT)

.PHONY: clean
clean:
	@rm -rf build/
	@rm -rf bin/
	@mkdir build/
	@mkdir bin/
