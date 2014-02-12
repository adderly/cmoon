LOCALBASE = ../../
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

FILES = b/base.css \
        b/client/lcs.css \
        b/client/mic.css \
        b/index.css b/demo.css b/manual.css \
        b/blog.css b/blogindex.css b/blogop.css \
        b/oms.css b/oms/edit.css b/oms/stat.css b/oms/users.css b/oms/faq.css b/oms/bill.css \
        b/okol.css \
        b/zero/zero.css b/zero/detail.css
all: preload $(FILES)

preload:
	@mkdir -p b/client/
	@mkdir -p b/oms/
	@mkdir -p b/zero/

b/base.css: reset.css base.css layout.css overlay.css

b/client/lcs.css: client/lcs.css
b/client/mic.css: client/mic.css

b/index.css: index.css
b/demo.css: demo.css
b/manual.css: manual.css
b/blog.css: blog/blog.css
b/blogindex.css: blog/index.css
b/blogop.css: blog/mku_skin.css blog/mku_set.css blog/op.css

b/oms.css: oms/oms.css oms/scroll.css
b/oms/edit.css: oms/tabs.css oms/edit.css
b/oms/stat.css: okol/datePicker.css oms/sorter.css oms/stat.css
b/oms/users.css: oms/users.css oms/overlay.css
b/oms/faq.css: oms/faq.css
b/oms/bill.css: okol/datePicker.css oms/bill.css
b/okol.css: okol/datePicker.css okol/okol.css
b/zero/zero.css: zero/zero.css zero/overlay.css
b/zero/detail.css: zero/detail.css

%.css:
	$(CSSGLUE) $^ > $@


clean:
	@rm -f $(FILES)
