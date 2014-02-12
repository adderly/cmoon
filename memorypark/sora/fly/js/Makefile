LOCALBASE = ../../
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

FILES = b/base.js

all: preload $(FILES)

preload:
	@mkdir -p b/

b/base.js: pub/jquery-1.7.1.js pub/jquery.cookie.js \
           pub/jquery.bmoon.js pub/jquery.hotkeys.js pub/jquery.mninpval.js \
           bmoon.sora.js

%.js:
	@echo -n > $@
	$(JSGLUE) $^ > $@


clean:
	@rm -f $(FILES)
