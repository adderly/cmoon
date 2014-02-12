LOCALBASE = ../
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

SUBDIR = js css

all: $(SUBDIR)
	@$(MULTIMAKE) $(SUBDIR)

install:
	@mkdir -p -m 775 $(PATH_DOC)
	@cp -rf * $(PATH_DOC)/

clean:
	@$(MULTIMAKE) -m clean $(SUBDIR)

backup:
	@$(BACKUPDIR) $(SUBDIR)
