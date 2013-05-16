INCS = -I../client -I../lib -I/usr/local/include/ClearSilver
LIBS = -L../client -lmoc -lneo_utl

SOURCES = $(wildcard *.c)
BINARY = $(patsubst %.c, %, $(SOURCES))

all: $(BINARY)

%:%.c
	@if [ "$<" = `ls *.c|awk '{print $1}'|sed -n '1p'` ]; then \
		echo "=== "$(CFLAGS); \
		echo "=== "$(INCS); \
		echo "=== "$(LIBS); fi
	@echo "$(CC) -o $@"
	@$(CC) $(CFLAGS) $< -o $@ $(INCS) ${LIBS}

install:

clean:
	rm -f $(BINARY)
