CXX = g++
AM_CXXFLAGS = -std=c++1y -Wall -I. -O2 -D_GNU_SOURCE -D__IN_LIBSTDF
AM_LDFLAGS =

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

bin_PROGRAMS = stdf_db cond_simulate
stdf_db_SOURCES = $(MAIN_SRC) active_parts.cpp thread_process.cpp stdf_cond.cpp stdf_shared.cpp
cond_simulate_SOURCES = $(SIM_SRC) active_parts.cpp thread_process.cpp stdf_cond.cpp stdf_shared.cpp

stdf_db_LDADD = -lmysqlcppconn -lstdf -lconfig++ -lcrypto -lpthread
cond_simulate_LDADD = -lmysqlcppconn -lstdf -lconfig++ -lcrypto -lpthread