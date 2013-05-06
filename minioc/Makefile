BASEDIR = ../
include $(BASEDIR)Make.env

CFLAGS += -rdynamic

SOURCE1 = apev.c
SOURCES = apev.c main.c mevent_ape_ext.c net.c parse.c udps.c
OBJS = $(patsubst %.c, %.o, $(SOURCES))
DEPEND = .depend

all: moc

$(DEPEND): $(SOURCES)
	@$(CC) $(CFLAGS) -MM $^ $(INC_MOON) > $@

moc: $(OBJS) $(DEPEND)
	$(CC) $(CFLAGS) $(OBJS) -o moc $(LIB_MOON)

include $(DEPEND)
%.o:%.c
	@if [ "$<" = "$(SOURCE1)" ]; then \
		echo "=== "$(CFLAGS); \
		echo "=== "$(INC_MOON) $(LIB_MOON); fi
	@echo "$(CC) -c $<"
	@$(CC) $(CFLAGS) -c $< -o $@ $(INC_MOON)

clean:
	rm -f $(OBJS) moc

backup:
	@$(MAKE) clean
	@$(TAR) zcvf moc.tar.gz *
