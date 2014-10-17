PROJECTNAME	= assembler
CC			= /usr/bin/gcc
CPPFLAGS	= -I include
CFLAGS 		= -W -Wall -std=gnu99 $(MODEFLAG)
COMPILE		= $(CC) $(CFLAGS) $(CPPFLAGS)
CDIR		= source
ODIR		= object
HDIR		= include
EXE			= assembler
OBJ			:= $(patsubst $(CDIR)/%.c, %.o, $(wildcard $(CDIR)/*.c))
OBJPATHS	:= $(addprefix $(ODIR)/, $(OBJ))

vpath %.c $(CDIR)
vpath %.o $(ODIR)
vpath %.h $(HDIR)

.PHONY: prod showpp open tar clean_all clean clean_hackfiles
.DEFAULT_GOAL := $(EXE)

# debug / production version differentiation
ifeq ($(MAKECMDGOALS), prod)
empty =
$(info )
$(info $(empty) ===> Building optimised production version of $(EXE)!)
$(info $(empty) ===> Please make sure you deleted all debug object files!)
$(info )
MODEFLAG = -O3
prod: $(EXE)
else
MODEFLAG = -g3
$(OBJ): debug.h
endif

# link
$(EXE): $(OBJ)
	@printf "\n ===> final linking to create executable: $@\n"
	$(CC) $(CFLAGS) $(OBJPATHS) -o $@

# compile
%.o: %.c
	@printf "\n ===> compiling object file: $@\n"
	$(COMPILE) -c $< -o $(ODIR)/$@

# header dependencies
main.o: strlib.h parser.h code.h symbolTable.h defs.h
parser.o: parser.h strlib.h dyn_input.h defs.h
code.o: code.h defs.h
symbolTable.o: symbolTable.h defs.h
dyn_input.o: dyn_input.h
strlib.o: strlib.h

# helpers
showpp:
	$(COMPILE) $(CDIR)/*.c -E 
open:
	vi -p $(CDIR)/*.c $(HDIR)/*.h Makefile
tar:
	tar -cvf $(PROJECTNAME).tar .

clean_all: clean clean_hackfiles
clean:
	rm -f $(EXE) $(ODIR)/*.o $(PROJECTNAME).tar
clean_hackfiles:
	rm -f programs/*.hack
