#==============================================================================
SHELL = bash
#------------------------------------------------------------------------------
NO_COLOR=\x1b[0m
OK_COLOR=\x1b[32m
ERROR_COLOR=\x1b[31m
WARN_COLOR=\x1b[33m
OK_STRING=$(OK_COLOR)✓$(NO_COLOR)
ERROR_STRING=$(ERROR_COLOR)⨯$(NO_COLOR)
WARN_STRING=$(WARN_COLOR)problems$(NO_COLOR)
#------------------------------------------------------------------------------
CC      = gcc
CFLAGS  = -O2 
INCLDS  = -I $(INC_DIR)
#------------------------------------------------------------------------------
BIN_DIR = bin
BLD_DIR = build
DOC_DIR = docs
INC_DIR = include
SRC_DIR = src
#------------------------------------------------------------------------------
PERF_STATS  = perf stat -r 3 -e L1-dcache-load-misses -M cpi
#------------------------------------------------------------------------------
TRASH   = $(BIN_DIR) $(BLD_DIR) 
#------------------------------------------------------------------------------
SRC     = $(wildcard $(SRC_DIR)/*.c)
OBJS    = $(patsubst $(SRC_DIR)/%.c,$(BLD_DIR)/%.o,$(SRC))
DEPS    = $(patsubst $(BLD_DIR)/%.o,$(BLD_DIR)/%.d,$(OBJS))
#------------------------------------------------------------------------------
PROGRAM = k_means
#==============================================================================

vpath %.c $(SRC_DIR)

.DEFAULT_GOAL = build

define show
@./$(UTI_DIR)/fmt.sh --color $(1) --type $(2) $(3)
endef

$(BLD_DIR)/%.d: %.c
	@echo -n "Generating  $(shell basenameter $@) ... "
	@$(CC) -M $(INCLDS) $(CFLAGS) $< -o $@
	@echo -e "$(OK_STRING)"

$(BLD_DIR)/%.o: %.c
	@echo -n "Building $(shell basename $@) ... "
	@$(CC) -c $(INCLDS)  $(CFLAGS) $< -o $@
	@echo -e "$(OK_STRING)"

$(BIN_DIR)/$(PROGRAM): $(DEPS) $(OBJS)
	@echo -n "Compiling ... "
	@$(CC) $(INCLDS)  $(CFLAGS) -o $@ $(OBJS)
	@echo -e "$(OK_STRING)"


build: setup $(BIN_DIR)/$(PROGRAM)

run: build
	@srun --partition=cpar $(PERF_STATS) ./$(BIN_DIR)/$(PROGRAM) 

# POR CORRIGIR
report: build
	@srun --partition=cpar perf record ./$(BIN_DIR)/$(PROGRAM) && perf report --stdio --dsos=k_means[pg50402@search7edu2 TP1]

setup:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(BLD_DIR)

clean:
	@echo -n "Cleaning ... "
	@-rm -rf $(TRASH)
	@echo -e "$(OK_STRING)"
