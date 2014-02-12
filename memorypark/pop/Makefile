LOCALBASE = ./
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

SUBDIR = xport pub walk run pager mtls fly

all: $(SUBDIR)
	@$(MULTIMAKE) $(SUBDIR)

clean:
	@$(MULTIMAKE) -m clean $(SUBDIR)

install:
	@$(MULTIMAKE) -m install $(SUBDIR)

backup:
	@$(BACKUPDIR) $(SUBDIR)
