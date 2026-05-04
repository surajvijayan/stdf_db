CXX = g++
CXXFLAGS = -std=c++1y -Wall -I. -O2 -D_GNU_SOURCE -D__IN_LIBSTDF -I/usr/include/mysql
LDFLAGS = -lmysqlcppconn -lstdf -lconfig++ -lcrypto -lpthread

# Common objects shared across binaries
COMMON_OBJS = active_parts.o thread_process.o stdf_cond.o stdf_shared.o

# Main application
MAIN_SRC = main.cpp
MAIN_OBJ = main.o
TARGET = stdf_db

# Simulator utility
SIM_SRC = cond_simulate.cpp
SIM_OBJ = cond_simulate.o
SIM_TARGET = cond_simulate

all: $(TARGET) $(SIM_TARGET)

$(TARGET): $(MAIN_OBJ) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SIM_TARGET): $(SIM_OBJ) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET) $(SIM_TARGET)

.PHONY: all clean