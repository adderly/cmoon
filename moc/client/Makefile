#don't use libmevent
MEVENT = 0
EVENTLOOP = 1

CFLAGS = -g -Wall -std=c99 -D_XOPEN_SOURCE=600 -fno-strict-aliasing -D_GNU_SOURCE -D_DARWIN_C_SOURCE
INC_MOON = -I/usr/local/include/ClearSilver -I../lib

SOURCE1 = cache.c
SOURCES = cache.c lerr.c packet.c mtrace.c    internal.c moc.c tcp.c
ifeq ($(EVENTLOOP), 1)
CFLAGS += -DEVENTLOOP
SOURCES += eloop.c mscli.c mcbk.c
endif

OBJS = $(patsubst %.c, %.o, $(SOURCES))
DEPEND = .depend

all: libmoc.a

$(DEPEND): $(SOURCES)
	@$(CC) $(CFLAGS) -MM $^ $(INC_MOON) > $@

libmoc.a: $(OBJS) $(DEPEND)
	@rm -rf $@
	$(AR) rcs $@ $(OBJS)

include $(DEPEND)
%.o:%.c
	@if [ "$<" = "$(SOURCE1)" ]; then \
		echo "=== "$(CFLAGS); \
		echo "=== "$(INC_MOON); fi
	@echo "$(CC) -c $<"
	@$(CC) $(CFLAGS) -c $< -o $@ $(INC_MOON)

install:

clean:
	rm -f $(OBJS) *.a

backup:
	@$(MAKE) clean
	@$(TAR) zcvf mocclient.tar.gz *

#LIB_ME = $(LIB_MOON:-lmevent=)
#INC_XX = $(INC_MOON:-I../../lib/mevent=)
#INC_ME = $(INC_XX:-I../../event/plugin=)
