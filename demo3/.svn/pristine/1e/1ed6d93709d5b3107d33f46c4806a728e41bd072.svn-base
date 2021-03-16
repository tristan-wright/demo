CFLAGS = -Wall -pedantic -std=gnu99 -g -pthread

all:mapper2310 control2310 roc2310

# Dealer executable.
mapper2310:mapper.o server.o
		gcc $(CFLAGS) mapper.o server.o -o mapper2310

# Dealer executable.
control2310:control.o server.o
		gcc $(CFLAGS) control.o server.o -o control2310

# Roc executable.
roc2310:roc.o server.o
		gcc $(CFLAGS) roc.o server.o -o roc2310

# Mapper file.
mapper.o:mapper.c
		gcc $(CFLAGS) -c mapper.c -o mapper.o

# Control file.
control.o:control.c
		gcc $(CFLAGS) -c control.c -o control.o

# Roc file.
roc.o:roc.c
		gcc $(CFLAGS) -c roc.c -o roc.o

# Server file
server.o:server.c
		gcc $(CFLAGS) -c server.c -o server.o

clean:
		rm mapper.o server.o control.o roc.o
		rm mapper2310 control2310 roc2310
