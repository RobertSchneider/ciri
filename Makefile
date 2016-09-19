cfiles = main.c command.c globals.c cmdMgr.c base64.c sha1.c ws_protocol.c server.c
libs = -lpthread

CFLAGS += -I/usr/include/json-c
LDFLAGS += -L/usr/lib -ljson-c

all:
	cd ciri;\
	gcc $(cfiles) -std=gnu99 -o ciri $(CFLAGS) -I/usr/include -L/usr/lib $(libs) $(LDFLAGS)
