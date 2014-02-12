BASEDIR = ../../../../
include $(BASEDIR)Make.env

FILES = Build/JSON.js Build/apeCore.js Build/apeClient.js

DIR_JSF = $(BASEDIR)../APE_JSF/
DIR_CORE = $(DIR_JSF)Source/
DIR_CLIENT = $(DIR_JSF)Clients/

all: $(FILES)

Build/JSON.js: $(DIR_CORE)Core/JSON.js

Build/apeCore.js: $(DIR_CORE)mootools-core.js \
		$(DIR_CORE)Core/APE.js $(DIR_CORE)Core/Events.js $(DIR_CORE)Core/Core.js $(DIR_CORE)Core/JSON.js $(DIR_CORE)Core/Session.js $(DIR_CORE)Core/Utility.js \
		$(DIR_CORE)Pipe/Pipe.js $(DIR_CORE)Pipe/PipeProxy.js $(DIR_CORE)Pipe/PipeMulti.js $(DIR_CORE)Pipe/PipeSingle.js \
		$(DIR_CORE)Request/Request.js $(DIR_CORE)Request/Request.Stack.js $(DIR_CORE)Request/Request.CycledStack.js \
		$(DIR_CORE)Transport/Transport.longPolling.js $(DIR_CORE)Transport/Transport.SSE.js \
		$(DIR_CORE)Transport/Transport.XHRStreaming.js $(DIR_CORE)Transport/Transport.JSONP.js $(DIR_CORE)Transport/Transport.WebSocket.js

Build/apeClient.js: $(DIR_CLIENT)mootools-core.js $(DIR_CLIENT)MooTools.js config.js

%.js:
	@echo -n > $@
	$(JSGLUE) $^ > $@


clean:
	@rm -f $(FILES)
