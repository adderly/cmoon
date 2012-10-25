LOCALBASE = ../../
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

# prefix for installing the binaries
PREFIX=/usr/local

CFLAGS += -shared -fPIC -rdynamic

CFLAGS += -I$(BASEDIR)event/daemon/ $(INC_MOON)
LIBS = -levent $(LIB_MOON) -liconv

SOURCES = $(wildcard *.c)
SOURCES_DM = ../daemon/log.c ../daemon/common.c ../daemon/reply.c ../daemon/syscmd.c
LIB_PLUGIN = $(patsubst %.c, %.so, $(SOURCES))

all: $(LIB_PLUGIN)

mevent_%.so: mevent_%.c
	@echo "$(CC) -o $@"
    if [ "$<" == mevent_public.c ]; then \
		@$(CC) $(CFLAGS) -o $@ $< $(SOURCES_DM) $(LIBS) \
    else \
		@$(CC) $(CFLAGS) -o $@ $< $(SOURCES_DM) mevent_public.c $(LIBS)

install:
	install -d $(PREFIX)/lib
	install -m 0644 $(LIB_PLUGIN) $(PREFIX)/lib/

uninstall:
	rm -f $(PREFIX)/lib/$(LIB_PLUGIN)

clean:
	rm -f $(LIB_PLUGIN)
