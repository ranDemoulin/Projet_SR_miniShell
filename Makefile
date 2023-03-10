.PHONY: all, clean

# Disable implicit rules
.SUFFIXES:

# Variable pour compilation
CC=gcc
CFLAGS= -g -Wall
CPPFLAGS = -I$(HDRPATH)

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions 
#LIBS += -lsocket -lnsl -lrt
LDFLAGS+=-lpthread

# Variable pour chemin
BINPATH=bin
SRCPATH=source
HDRPATH=header
OBJPATH=objet

# Variable pour les cibles et la récupération des fichiers
SRCS=$(wildcard $(SRCPATH)/*.c)
OBJS=$(SRCS:$(SRCPATH)/%.c=$(OBJPATH)/%.o)
INCLUDE = $(wildcard $(HDRPATH)/*.c)
EXEC=$(BINPATH)/shell

all: $(EXEC)

$(OBJPATH)/%.o: $(SRCPATH)/%.c
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(EXEC): $(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(EXEC) $(OBJPATH)/*.o

