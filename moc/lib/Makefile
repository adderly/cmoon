#don't use libmevent
MEVENT = 0

BASEDIR = ../../
include $(BASEDIR)Make.env

SOURCE1 = cache.c
SOURCES = cache.c lerr.c packet.c
OBJS = $(patsubst %.c, %.o, $(SOURCES))
DEPEND = .depend

all: libmocpub.a

$(DEPEND): $(SOURCES)
	@$(CC) $(CFLAGS) -MM $^ $(INC_MOON) > $@

libmocpub.a: $(OBJS) $(DEPEND)
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

cflag:
	@echo "=== "$(CFLAGS)
	@echo "=== "$(INC_MOON)
	@echo "=== "$(LIB_MOON)

clean:
	rm -f $(OBJS) *.a

backup:
	@$(MAKE) clean
	@$(TAR) zcvf libmoc.tar.gz *
