LOCALBASE = ../../
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

FILES = b/base.js \
        b/index.js \
        b/member/regist.js b/member/reset.js

all: preload $(FILES)

preload:
	@mkdir -p b/
	@mkdir -p b/member/

b/base.js: pub/jquery-1.7.1.js pub/jquery.cookie.js pub/jquery.md5.js \
           pub/jquery.bmoon.js pub/jquery.hotkeys.js pub/jquery.mninpval.js \
           pub/jquerytools/src/toolbox/toolbox.expose.js \
           pub/jquerytools/src/overlay/overlay.js \
           pub/noty/js/jquery.noty.js pub/jquery.mnblink.js \
           pub/bmoon.js bmoon.sora.js

b/index.js: index.js

b/member/regist.js: member/regist.js
b/member/reset.js: member/reset.js

%.js:
	@echo -n > $@
	$(JSGLUE) $^ > $@


clean:
	@rm -f $(FILES)
