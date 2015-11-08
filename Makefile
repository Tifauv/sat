#
# Makefile pour le projet de Logique - Licence Info 2001/2002
# Université d'Angers
#
# Ecrit par Olivier SERVE
# 17/01/2002

APP = testSat

SRCDIR = src
BUILDDIR = build

SRCS    := $(shell find $(SRCDIR) -name '*.c')
SRCDIRS := $(shell find . -name '*.c' -exec dirname {} \; | uniq)
OBJS    := $(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS))

DEBUG = -ggdb
CFLAGS = -Wall -pedantic -std=gnu99 -c $(DEBUG) -I$(SRCDIR)

## GENERAL RULES
.phony: all clean

all: $(BUILDDIR)/$(APP)

$(BUILDDIR)/$(APP): buildrepo $(OBJS)
	@echo "Linking $@..."
	@$(CC) $(OBJS) $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: %.c
	@echo "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) -r $(BUILDDIR)

buildrepo:
	@$(call make-repo)

## Functions
define make-repo
	for dir in $(SRCDIRS); \
	do \
		mkdir -p $(BUILDDIR)/$$dir; \
	done
endef

##
# Usage: $(call make-depend, source-file, object-file, depend-file)
define make-depend
	$(CC) -MM	\
		-MF $3	\
		-MP	\
		-MT $2	\
		$(CFLAGS) \
		$1
endef