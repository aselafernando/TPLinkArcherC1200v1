
exclude_dirs := include bin

dirs:=$(shell find . -maxdepth 1 -type d)
dirs := $(basename $(patsubst ./%,%,$(dirs)))
dirs := $(filter-out $(exclude_dirs),$(dirs))
SUBDIRS := $(dirs)
CLEAN_DIRS := $(addprefix _clean_,$(SUBDIRS))

.PHONY: all $(SUBDIRS) clean

$(SUBDIRS):
	@echo "$(SUBDIRS)"
	$(MAKE) CFLAGS=$(CFLAGS) LDFLAGS=$(LDFLAGS) CC=$(CC) -C $@ all

all: $(SUBDIRS)

$(CLEAN_DIRS):
	$(MAKE) -C $(patsubst _clean_%,%,$@) clean

clean: $(CLEAN_DIRS) 
