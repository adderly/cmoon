#don't use libmevent
MEVENT = 0
EVENTLOOP = 1

BASEDIR = ../../
include $(BASEDIR)Make.env

CFLAGS = -std=c99 -D_GNU_SOURCE
INC_MOON += -I../lib

SOURCE1 = cache.c
SOURCES = cache.c lerr.c packet.c mtrace.c    internal.c moc.c tcp.c
ifeq ($(EVENTLOOP), 1)
CFLAGS += -DEVENTLOOP
SOURCES += eloop.c
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
