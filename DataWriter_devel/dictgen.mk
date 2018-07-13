src/dict_src.cc: $(DICTH_src)
	@echo "Generating dictionary $@..."
	@$(ROOTCINT) -f $@ -c -p $(filter -I% -D%,$(CXXFLAGS)) $^
src/reflexdict_src.hh: $(REFLEXH_src)
	@echo "Generating reflex header $@..."
	@echo '#pragma once' > $@
	@for i in $^; do echo '#include "'$${i}'"' >> $@; done
src/reflexdict_src.cc: src/reflexdict_src.hh $(SELECTIONXML_src) ${DEPDIR}/src/reflexdict_src.gendict.d
	@echo "Generating reflex dictionary $@..."
	@$(GENREFLEX) $< --fail_on_warnings --quiet -s $(SELECTIONXML_src) -o $@ $(filter -I% -D%,$(CXXFLAGS))
interface/dict_interface.cc: $(DICTH_interface)
	@echo "Generating dictionary $@..."
	@$(ROOTCINT) -f $@ -c -p $(filter -I% -D%,$(CXXFLAGS)) $^
interface/reflexdict_interface.hh: $(REFLEXH_interface)
	@echo "Generating reflex header $@..."
	@echo '#pragma once' > $@
	@for i in $^; do echo '#include "'$${i}'"' >> $@; done
interface/reflexdict_interface.cc: interface/reflexdict_interface.hh $(SELECTIONXML_interface) ${DEPDIR}/interface/reflexdict_interface.gendict.d
	@echo "Generating reflex dictionary $@..."
	@$(GENREFLEX) $< --fail_on_warnings --quiet -s $(SELECTIONXML_interface) -o $@ $(filter -I% -D%,$(CXXFLAGS))
