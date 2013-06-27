LOCALBASE = ../../
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

FILES = b/base.css

all: preload $(FILES)

preload:
	@mkdir -p b/

b/base.css: reset.css base.css layout.css

%.css:
	$(CSSGLUE) $^ > $@


clean:
	@rm -f $(FILES)
