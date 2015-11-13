#
# Makefile pour le projet de Logique - Licence Info 2001/2002
# Université d'Angers
#
# Ecrit par Olivier SERVE
# 17/01/2002

APP = testSat

SRCDIR = src
BUILDDIR = build

SRCS    := $(shell find $(SRCDIR) -name '*.cpp')
SRCDIRS := $(shell find . -name '*.cpp' -exec dirname {} \; | uniq)
OBJS    := $(patsubst %.cpp,$(BUILDDIR)/%.o,$(SRCS))

DEBUG = -ggdb
CXXFLAGS = -Wall -pedantic -std=c++11 -c $(DEBUG) -I$(SRCDIR)
LDFLAGS = -llog4c

## GENERAL RULES
.phony: all clean

all: $(BUILDDIR)/$(APP)

$(BUILDDIR)/$(APP): buildrepo $(OBJS)
	@echo "Linking $@..."
	@$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: %.cpp
	@echo "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $< -o $@

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
	$(CXX) -MM	\
		-MF $3	\
		-MP	\
		-MT $2	\
		$(CXXFLAGS) \
		$1
endef