CFLAGS = -DTEST
OBJS = clock.o
BINARY = sismografo

LDSCRIPT = ../stm32f429i-discovery.ld

include ../../Makefile.include
