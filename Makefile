FLAGS=-O3 -march=native -Wno-ignored-attributes
DFLAGS=-ggdb -pg -Wno-ignored-attributes

PP=-DIL_STD -DNDEBUG #-DPROFILE_MRCSP
DPP=-DIL_STD -D_DEBUG -DDEBUG

INC=$(CPATH_EXPANDED) -I./include/
LIB=$(LIBRARY_PATH_EXPANDED) -I./include/

SRC=$(patsubst source/%, %, $(wildcard source/*.cpp))
OBJ=$(patsubst %.cpp, release/%.o, ${SRC})
DOBJ=$(patsubst %.cpp, debug/%.o, ${SRC})

# latticeRCSP
SRCDIR = ./latticeRCSP
INCDIR = ./include
OBJDIR  = ./release/latticeRCSP
OBJINCDIR  = ./release/include
DOBJDIR  = ./debug/latticeRCSP
DOBJINCDIR  = ./debug/include
SRCmrcspDIR = $(SRCDIR)/mrcsp
SRClatticeDIR = $(SRCDIR)/latticeRCSP
# Used to mkdir folders
SRCDIRS := $(shell find ${SRCDIR} -name '*.c*' -exec dirname {} \; | uniq) 
INCDIRS := $(shell find ${INCDIR} -name '*.c*' -exec dirname {} \; | uniq)
OBJDIRS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCDIRS)) $(patsubst $(INCDIR)/%,$(OBJINCDIR)/%,$(INCDIRS))
DOBJDIRS := $(patsubst $(SRCDIR)/%,$(DOBJDIR)/%,$(SRCDIRS)) $(patsubst $(INCDIR)/%,$(DOBJINCDIR)/%,$(INCDIRS))

# Used to build the shared objects (latticeRCSP_v2 but not the main)
SRCSTPP   := $(shell find $(SRClatticeDIR) -name '*.hpp') $(shell find $(SRCmrcspDIR) -name '*.hpp') 
SRCSH   := $(shell find $(SRClatticeDIR) -name '*.h') $(shell find $(SRCmrcspDIR) -name '*.h') 
SRCSCXX   := $(shell find $(SRClatticeDIR) -name '*.cpp') $(shell find $(SRCmrcspDIR) -name '*.cpp') 
SRCSC    := $(shell find $(SRClatticeDIR) -name '*.c') $(shell find $(SRCmrcspDIR) -name '*.c')
INCCXX := $(shell find $(INCDIR) -name '*.cc')
OBJS    := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCSCXX)) $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCSC)) $(patsubst $(INCDIR)/%.cc,$(OBJINCDIR)/%.o,$(INCCXX))
DOBJS    := $(patsubst $(SRCDIR)/%.cpp,$(DOBJDIR)/%.o,$(SRCSCXX)) $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCSC)) $(patsubst $(INCDIR)/%.c,$(DOBJINCDIR)/%.o,$(INCCXX))

all: dir release/evsp

debug: dir-debug debug/evsp

release/evsp: $(OBJ)
	@echo -n "[Linking] $@... "
	@g++ -o release/evsp $(FLAGS) $(OBJ) $(OBJS) $(LIB) -lilocplex -lconcert -lcplex -lm -lpthread -lboost_system -lboost_timer -ldl
	@echo "done!"

debug/evsp: $(DOBJ)
	@echo -n "[Linking debug] $@..."
	@g++ -o debug/evsp $(DFLAGS) $(DOBJ) $(DOBJS) $(LIB) -lilocplex -lconcert -lcplex -lm -lpthread -lboost_system -lboost_timer -ldl
	@echo "done!"

obj: $(OBJ)

dobj: $(DOBJ)

release/MonoidPricing.o: source/MonoidPricing.cpp source/MonoidPricing.h source/Parameters.h source/Parameters.cpp $(OBJS) $(SRCSTPP) $(SRCSH)
	@echo "[Compiling] $@... "
	@g++ -c -o $@ $< $(FLAGS) $(PP) $(INC)

release/%.o: source/%.cpp source/%.h
	@echo "[Compiling] $@... "
	@g++ -c -o $@ $< $(FLAGS) $(PP) $(INC)

release/%.o: source/%.cpp 
	@echo "[Compiling] $@... "
	@g++ -c -o $@ $< $(FLAGS) $(PP) $(INC)

debug/MonoidPricing.o: source/MonoidPricing.cpp source/MonoidPricing.h source/Parameters.h source/Parameters.cpp $(DOBJS) $(SRCSTPP) $(SRCSH)
	@echo "[Compiling debug] $@... "
	@g++ -c -o $@ $< $(DFLAGS) $(DPP) $(INC)

debug/%.o: source/%.cpp
	@echo "[Compiling debug] $@... "
	@g++ -c -o $@ $< $(DFLAGS) $(DPP) $(INC)

clean:
	@echo -n "[Cleaning] release/* debug/*... "
	-@rm -rf release/* debug/* 2>/dev/null || true
	@echo "booom!"

dir:/
	@mkdir -p release

dir-debug:
	@mkdir -p debug

.SECONDEXPANSION:

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRCSTPP) $(SRCSH)
	@$(call make-repo)
	@echo "[Compiling] $@..."
	@g++ $(INC) $(FLAGS) $(PP) -c $< -o $@

$(OBJINCDIR)/%.o: $(INCDIR)/%.cc
	@$(call make-repo)
	@echo "[Compiling] $@..."
	@g++ $(INC) $(FLAGS) $(PP) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@$(call make-repo)
	@echo "[Compiling] $@..."
	@g++ $(INC) $(FLAGS) $(PP) -c $< -o $@

define make-repo
   for dir in $(OBJDIRS); \
   do \
	mkdir -p $$dir; \
   done
endef

$(DOBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRCSTPP) $(SRCSH)
	@$(call make-drepo)
	@echo "[Compiling debug] $@..."
	@g++ $(INC) $(DFLAGS) $(DPP) -c $< -o $@

$(DOBJINCDIR)/%.o: $(INCDIR)/%.cc
	@$(call make-repo)
	@echo "[Compiling debug] $@..."
	@g++ $(INC) $(DFLAGS) $(DPP) -c $< -o $@

$(DOBJDIR)/%.o: $(SRCDIR)/%.c
	@$(call make-drepo)
	@echo "[Compiling debug] $@..."
	@g++ $(INC) $(DFLAGS) $(DPP) -c $< -o $@

define make-drepo
   for dir in $(DOBJDIRS); \
   do \
	mkdir -p $$dir; \
   done
endef

