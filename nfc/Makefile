

ZSTDDIR  = lib
PRGDIR   = programs



VOID = /dev/null
EXT =



.PHONY: default
default: zstd-release


#=================================
.PHONY: lib-release
lib-release:
	@$(MAKE) -C $(ZSTDDIR) $@



#=================================
.PHONY: nfc-release
nfc-release:
	@$(MAKE) -C $(PRGDIR) $@
	cp $(PRGDIR)/nfc$(EXT) .
	rm -f $(PRGDIR)/nfc$(EXT)



#=================================
.PHONY: zstd-release
zstd-release:
	@$(MAKE) -C $(PRGDIR) $@
	cp $(PRGDIR)/zstd_test .
	rm -f $(PRGDIR)/zstd_test


#=================================
.PHONY: tools-release
tools-release:
	@$(MAKE) -C $(PRGDIR) $@





.PHONY: clean
clean:
	#@$(MAKE) -C $(ZSTDDIR) $@ > $(VOID)
	@$(MAKE) -C $(PRGDIR) $@ > $(VOID)
	@$(RM) nfc$(EXT) nfcmt$(EXT) tmp* zstd_test
	@echo Cleaning completed














