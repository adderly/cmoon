LOCALBASE = ../../
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

FILES = b/base.css \
        b/index.css \
        b/member/regist.css b/member/reset.css

all: preload $(FILES)

preload:
	@mkdir -p b/
	@mkdir -p b/member/

b/base.css: reset.css base.css layout.css modules/overlay/overlay.css \
            modules/indicator/indicator.css \
            pub/noty/css/jquery.noty.css pub/noty/css/noty_theme_mitgux.css

b/index.css: index.css

b/member/regist.css: member/regist.css
b/member/reset.css: member/reset.css

%.css:
	$(CSSGLUE) $^ > $@


clean:
	@rm -f $(FILES)
