CC = g++

## variables [leave blank if not needed]

CFLAGS = -pipe -std=gnu++17 -Wpedantic -Wall -Wextra -Werror
YFLAGS =
LDLIBS = -lpthread

LEXYACC_PATH =
INCLUDE_PATH = inc
LIB_PATH     = lib
VM_PATH      =

TARGET       = main
FILEXT       = cpp

SRCDIR       = src
OBJDIR       = obj
BINDIR       = bin

PARALLEL     = yes

## performance settings [do not modify]

DEVICE_NUMW  = $(shell expr $(shell nproc) + 1)
NUM_WORKERS  = $(shell echo $(MAKEFLAGS) | grep -oP '(?<=-j)\d+' || echo $(DEVICE_NUMW))
ifneq ($(PARALLEL), yes)
override NUM_WORKERS =
endif

## collections [do not modify]

SRCSUBDIRS  := $(shell find $(SRCDIR) -type d)
INCSUBDIRS  := $(shell find $(INCLUDE_PATH) -type d)

SOURCES     := $(foreach dir, $(SRCSUBDIRS), $(wildcard $(dir)/*.$(FILEXT)))
INCLUDES    := $(foreach dir, $(INCSUBDIRS), $(wildcard $(dir)/*.h|*.hpp))
LIBS        := $(wildcard $(LIB_PATH)/*.h|*.hpp)
OBJECTS0    := $(addprefix $(OBJDIR)/, $(SOURCES:$(SRCDIR)/%.$(FILEXT)=%.o))

LEXSRC      := $(wildcard $(LEXYACC_PATH)/*.l|*.ll)
YACCSRC     := $(wildcard $(LEXYACC_PATH)/*.y|*.yy)
LEXC        := $(LEXSRC:$(LEXYACC_PATH)/%.l=$(SRCDIR)/%.c) $(LEXSRC:$(LEXYACC_PATH)/%.ll=$(SRCDIR)/%.cpp)
YACCC       := $(YACCSRC:$(LEXYACC_PATH)/%.y=$(SRCDIR)/%.c) $(YACCSRC:$(LEXYACC_PATH)/%.yy=$(SRCDIR)/%.cpp)
LEXOBJ      := $(LEXSRC:$(LEXYACC_PATH)/%.l=$(OBJDIR)/%.o) $(LEXSRC:$(LEXYACC_PATH)/%.ll=$(OBJDIR)/%.o)
YACCOBJ     := $(YACCSRC:$(LEXYACC_PATH)/%.y=$(OBJDIR)/%.o) $(YACCSRC:$(LEXYACC_PATH)/%.yy=$(OBJDIR)/%.o)

OBJECTS      = $(filter-out $(LEXOBJ) $(YACCOBJ), $(OBJECTS0))

PATH_TO_EXE  = $(BINDIR)/$(TARGET)
LAUNCH_CMD   = $(PATH_TO_EXE)

## rules [do not modify]

all : debug

.PHONY : docs
docs:
	@echo "\033[95mBuilding documentation...\033[0m"
	@mkdir -p html/assets
	@cp -r assets/* html/assets
	@doxygen ./Doxyfile > /dev/null 2>&1
	@echo "\033[97mDocumentation built!\033[0m"
	@( \
	(  wslview html/index.html \
	|| xdg-open html/index.html \
	|| open html/index.html ) > /dev/null 2>&1 & ) \
	|| echo "\033[91mCould not open documentation in browser.\033[0m"

.PHONY : format
format:
# we exclude the lib directory because it may contain very large files
	@find . -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.${FILEXT}" \) -not -path "./${LIB_PATH}/*" | xargs clang-format -i -style=file
	@echo "\033[92mFormatting complete!\033[0m"

debug: CFLAGS += -Og -DDEBUG -g -ggdb -DYYDEBUG
debug: YFLAGS += -v
debug: __maybe_multi_worker_target
	@echo "\033[93mRunning in debug mode!\033[0m"

release: CFLAGS += -march=native -O2
release: __maybe_multi_worker_target
	@echo "\033[96mRunning in release mode!\033[0m"

generic: CFLAGS += -march=x86-64 -O2
generic: __maybe_multi_worker_target
	@echo "\033[95mRunning in generic mode!\033[0m"

run:
ifneq ("$(wildcard $(PATH_TO_EXE))", "")
	./$(LAUNCH_CMD)
else
	@echo "\033[91mNo executable found!\033[0m"
endif

run-release: release
	./$(LAUNCH_CMD)

run-debug: debug
	valgrind --leak-check=full --show-leak-kinds=all --vgdb=full -s ./$(LAUNCH_CMD)

__maybe_multi_worker_target:
ifneq ($(NUM_WORKERS),)
	@echo "\033[94mBuilding with up to $(NUM_WORKERS) workers...\033[0m"
	@$(MAKE) -j $(NUM_WORKERS) $(PATH_TO_EXE)
else
	@echo "\033[94mBuilding with one worker...\033[0m"
	@$(MAKE) --no-print-directory $(PATH_TO_EXE)
endif

$(LEXC):
	flex -o $@ $(LEXSRC)

$(YACCC):
	bison $(YFLAGS) -do $@ $(YACCSRC)

$(PATH_TO_EXE): $(OBJECTS) $(YACCOBJ) $(LEXOBJ)
	mkdir -p $(BINDIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)
	@echo "\033[92mLinking complete!\033[0m"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.$(FILEXT) $(INCLUDES) $(YACCC) $(LEXC)
	mkdir -p $(OBJDIR) $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) -isystem$(INCLUDE_PATH) -isystem$(LIB_PATH)

$(LEXOBJ): $(OBJDIR)/%.o : $(SRCDIR)/%.$(FILEXT) $(INCLUDES) $(LEXC)
	mkdir -p $(OBJDIR)
	$(CC) -o $@ -c $< $(CFLAGS) -isystem$(INCLUDE_PATH) -isystem$(LIB_PATH)

$(YACCOBJ): $(OBJDIR)/%.o : $(SRCDIR)/%.$(FILEXT) $(INCLUDES) $(YACCC)
	mkdir -p $(OBJDIR)
	$(CC) -o $@ -c $< $(CFLAGS) -isystem$(INCLUDE_PATH) -isystem$(LIB_PATH)


.PHONY: clean
clean:
	rm -rf $(OBJDIR)/*
	rm -f $(PATH_TO_EXE)
	rm -f $(LEXC)
	rm -f $(YACCC) $(YACCC:.c=.h) $(YACCC:.cpp=.h) $(YACCC:.c=.output) $(YACCC:.cpp=.output)
