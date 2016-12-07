DESTDIR=/usr
PREFIX?=/local

CC	?= cc
CFLAGS	+= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -std=gnu11
LIBS    = -lwiringPi

all:		dht

dht:	
	@echo [Link]
	@$(CC) -o $@ dht.c $(CFLAGS) $(LIBS)
	

.PHONY:	install
install:
	@echo "[Install]"
	@install -m 4755 -o root -g root dht	$(DESTDIR)$(PREFIX)/bin

.PHONY:	clean
clean:
	@echo "[Clean]"
	@rm -f $(OBJ) dht *~ core tags *.bak
