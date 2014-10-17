DIRS	= assembler vm_translator

.PHONY: prod clean tar open $(DIRS)

prod clean: $(DIRS)

$(DIRS):
	$(MAKE) $(MAKECMDGOALS) --directory=$@

tar:
	tar -cvf TEOCS_Compiler.tar .

open:
	vi -p Makefile $(patsubst %, %/Makefile, $(DIRS))
