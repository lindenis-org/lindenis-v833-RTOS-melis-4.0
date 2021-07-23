
cc = gcc

ccflag-s = -I./

all:
	@$(cc) $(ccflag-s) -o config.o -c config.c
	@$(cc) $(ccflag-s) -o config_private.o -c config_private.c

clean:
	@rm -rf *.o
