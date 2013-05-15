#don't use libmevent
MEVENT = 0

BASEDIR = ../../
include $(BASEDIR)Make.env

INC_MOON += -I../lib
LIB_MOON += -levent -L../lib -lmocpub

SOURCE1 = internal.c
SOURCES = internal.c moc.c mtrace.c tcp.c
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
