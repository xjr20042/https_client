#CROSS = arm-linux-gnueabihf-

CC = $(CROSS)gcc
LD = $(CROSS)ld
STRIP = $(CROSS)strip

CFLAGS = -fPIC -DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -O2
LDFLAGS =

INCLUDES = -I../mbedtls-2.2.1/include
LIBS = ../mbedtls-2.2.1/library/libmbedx509.a ../mbedtls-2.2.1/library/libmbedtls.a ../mbedtls-2.2.1/library/libmbedcrypto.a

SOURCES = main.c https.c

OBJS = $(SOURCES:.c=.o)

.SUFFIXES:.c .o

.c.o:
	@echo Compiling: $< ....
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

all: https_client

https_client: $(OBJS)
	@echo Linking: $@ ....
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(STRIP) -s $@

clean:
	rm -f https_client *.o
