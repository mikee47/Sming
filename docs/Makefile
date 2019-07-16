# Makefile for Sphinx documentation
#

# Don't bother with implicit checks
.SUFFIXES:

SMINGDIR := ..
override SMING_HOME := $(SMINGDIR)/Sming

# You can set these variables from the command line, and also from the environment for the first two.
SPHINXOPTS		?=
SPHINXBUILD		?= sphinx-build
SOURCEDIR		= source
BUILDDIR		= build

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q			:=
vecho		:= @true
else
Q			:= @
vecho		:= @echo
endif

define Sphinx
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
endef

# Put it first so that "make" without argument is like "make help".
.PHONY: help
help:
	$(call Sphinx)

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
.PHONY: html singlehtml latex
html singlehtml latex:
	$(call Sphinx)

.PHONY: latexpdf
latexpdf: latex
	@echo "Running LaTeX files through pdflatex..."
	$(Q) $(MAKE) -C $(BUILDDIR)/latex all-pdf

# List of documentation files to be included from source tree
SOURCE_FILES		:= CONTRIBUTING.md

# List immediate sub-directories for a list of root directories
# Results are sorted and without trailing path separator
# $1 -> Root paths
define ListSubDirs
$(sort $(foreach d,$(dir $(wildcard $1/*/.)),$(d:/=)))
endef

SRC_INCDIR			:= $(SOURCEDIR)/_inc

# Each sample, library and Component MUST have a README.rst or README.md file
# This is a list of directories where such files may exist
# If a component.mk file exists, then it is parsed and any dependencies handled
SOURCE_DIRS := \
	$(SMING_HOME)/Arch \
	$(SMING_HOME)/Arch/*/Components \
	$(SMING_HOME)/Components \
	$(SMING_HOME)/Libraries \
	$(SMINGDIR)/samples

SOURCE_DIRS := $(sort $(SMING_HOME) $(foreach d,$(wildcard $(SOURCE_DIRS)),$(call ListSubDirs,$d)))

# This macro sets the default component variables before including the (optional) component.mk file.
# $1 -> Component path
define ParseComponent
$(if $V,$(info -- Parsing $1))
# Process any component.mk file (optional)
ifneq (,$(wildcard $1/component.mk))
COMPONENT_RULE			:= __no_build__
COMPONENT_NAME			:= $(notdir $1)
COMPONENT_PATH			:= $1
COMPONENT_VARS			:=
CONFIG_VARS				:=
RELINK_VARS				:=
DEBUG_VARS				:=
COMPONENT_SUBMODULES	:=
COMPONENT_DOCFILES		:=
COMPONENT_DEPENDS		:=
ENABLE_ESPCONN			:=
ENABLE_CUSTOM_LWIP		:=
include $1/component.mk
CMP_$1_DEPENDS			:= $$(COMPONENT_DEPENDS)
CMP_$1_VARS				:= $$(sort $$(COMPONENT_VARS))
SOURCE_DIRS				+= $$(addprefix $$(COMPONENT_PATH)/,$$(COMPONENT_SUBMODULES))
SOURCE_FILES			+= $$(addprefix $$(COMPONENT_PATH)/,$$(COMPONENT_DOCFILES))
endif
endef # ParseComponent

$(foreach d,$(SOURCE_DIRS),$(eval $(call ParseComponent,$d)))

SOURCE_FILES			+= $(foreach d,$(SOURCE_DIRS),$(wildcard $d/*.rst $d/README.md $d/*.svg $d/*.png $d/*.jpg))
SOURCE_FILES			:= $(patsubst $(SMINGDIR)/%,%,$(SOURCE_FILES))
SOURCE_DIRS				:= $(patsubst $(SMINGDIR)/%,%,$(SOURCE_DIRS))

SOURCE_FILE_DIRS		:= $(dir $(SOURCE_FILES))
MISSING_README_DIRS		:= $(filter-out $(SOURCE_FILE_DIRS:/=),$(SOURCE_DIRS))

SOURCE_DIRS				:= $(patsubst %,$(SRC_INCDIR)/%,$(SOURCE_DIRS))

# Display variable and list values, e.g. $(call PrintVariable,LIBS)
# $1 -> Name of variable containing values
define PrintVariable
	$(info $1)
	$(foreach item,$($1),$(info - $(item)))
endef

ifeq ($V,1)
$(eval $(call PrintVariable,SOURCE_DIRS))
$(eval $(call PrintVariable,SOURCE_FILES))
endif

.PHONY: clean
clean:
	$(call Sphinx)
	$(Q) rm -rf $(SRC_INCDIR) api

.PHONY: setup
setup: $(addprefix $(SRC_INCDIR)/,$(SOURCE_FILES))
	$(call PrintVariable,MISSING_README_DIRS)

# $1 -> source file (in incdir)
define CopyFileTarget
$(SRC_INCDIR)/$1: $(SMINGDIR)/$1 | $(patsubst %/,%,$(dir $(SRC_INCDIR)/$1))
	$(Q) cp $$< $$@
endef

$(foreach f,$(SOURCE_FILES),$(eval $(call CopyFileTarget,$f)))

$(SRC_INCDIR) $(SOURCE_DIRS):
	$(Q) mkdir -p $@

.PHONY: api
api: api/xml/index.xml

#
DOXYGEN := $(shell command -v doxygen 2> /dev/null)

api/error.log:
ifndef DOXYGEN
	$(error doxygen not found - not building API docs)
endif
	@echo Generating Doxygen information
	$(Q) mkdir -p $(@D)
	$(Q) doxygen 2>$@ 1>$(@D)/doxygen.log	

api/xml/index.xml: api/error.log
	@echo "Undocumented compounds: $(shell grep "Compound.*is not documented" $< | wc -l)"
	@echo "Undocumented members:  $(shell grep "Member.*is not documented" $< | wc -l) "
	@echo "Undocumented parameters: $(shell grep "The following parameters of .* are not documented" $< | wc -l) "
