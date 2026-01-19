## The following lines are mandatory, please don't change them.
where_am_I := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include $(E3_REQUIRE_TOOLS)/driver.makefile
include $(E3_REQUIRE_CONFIG)/DECOUPLE_FLAGS


ifneq ($(strip $(ASYN_DEP_VERSION)),)
asyn_VERSION=$(ASYN_DEP_VERSION)
endif

ifneq ($(strip $(MODBUS_DEP_VERSION)),)
modbus_VERSION=$(MODBUS_DEP_VERSION)
endif

APP:=myrrha_ssaApp
APPDB:=$(APP)/Db
APPSRC:=$(APP)/src

USR_CXXFLAGS += --std=c++11

TEMPLATES += $(wildcard $(APPDB)/*.db)
TEMPLATES += $(wildcard $(APPDB)/*.proto)
TEMPLATES += $(wildcard $(APPDB)/*.template)

SOURCES += $(APPSRC)/dummy.c
SOURCES += $(APPSRC)/buffer.cpp
SOURCES += $(APPSRC)/endianness.cpp
SOURCES += $(DBDINC_SRCS)

DBDS += $(APPSRC)/buffer.dbd
DBDS += $(APPSRC)/endianness.dbd


$(DBDINC_DEPS): $(DBDINC_HDRS)

.dbd.h:
	$(DBTORECORDTYPEH)  $(USR_DBDFLAGS) -o $@ $<

.PHONY: $(DBDINC_DEPS) .dbd.h


SCRIPTS += $(wildcard ../iocsh/*.iocsh)
STARTUP += $(wildcard ../startup/*.cmd)


db: 

.PHONY: db 


vlibs:

.PHONY: vlibs


# Install IOCFactory-compatible snippets. The build system is run several times,
# ensure that we define things in appropriate runs.
ifndef SNIPPETS

SNIPPETS := $(if ${STARTUP},$(filter-out -none-,${STARTUP}),$(wildcard *.cmd))
SNIPPETS := $(addprefix ${where_am_I},${SNIPPETS})
export SNIPPETS

endif

ifdef INSTALL_REV

INSTALL_SNIPPETS = $(addprefix ${INSTALL_REV}/startup/,$(notdir ${SNIPPETS}))

${INSTALL_SNIPPETS}: ${SNIPPETS}
	@echo "Installing startup snippets $^ to $(@D)"
	$(INSTALL) -d -m555 $^ $(@D)

install: ${INSTALL_SNIPPETS}

endif # SNIPPETS
