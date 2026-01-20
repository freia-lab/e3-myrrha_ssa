# Copyright (C) 2025  FREIA Laboratory

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

#QUIET = 
# The following lines are required
where_am_I := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include $(E3_REQUIRE_TOOLS)/driver.makefile

# Most modules only need to be built for x86_64
EXCLUDE_ARCHS += linux-ppc64e6500
EXCLUDE_ARCHS += linux-corei7-poky

# Since this file (myrrha_ssa.Makefile) is copied into
# the module directory at build-time, these paths have to be relative
# to that path
APP := myrrha_ssaApp
APPDB := $(APP)/Db
APPSRC := $(APP)/src

# If you have files to include, you will generally want to include these, e.g.
#
#     SOURCES += $(APPSRC)/myrrha_ssaMain.cpp
#     SOURCES += $(APPSRC)/library.c
#     HEADERS += $(APPSRC)/library.h
#     USR_INCLUDES += -I$(where_am_I)$(APPSRC)

#SOURCES +=  $(wildcard $(APPSRC)/*.cpp)
SOURCES += $(APPSRC)/dummy.c
SOURCES += $(APPSRC)/buffer.cpp
SOURCES += $(APPSRC)/endianness.cpp
SOURCES += $(DBDINC_SRCS)
HEADERS +=  $(wildcard $(APPSRC)/*.h)


TEMPLATES += $(wildcard $(APPDB)/*.db)
TEMPLATES += $(wildcard $(APPDB)/*.proto)
TEMPLATES += $(wildcard $(APPDB)/*.template)

############################################################################
#
# Add any .dbd files that should be included (e.g. from user-defined functions, etc.)
#
############################################################################

DBDS += $(APPSRC)/buffer.dbd
DBDS += $(APPSRC)/endianness.dbd


SCRIPTS += $(wildcard ../iocsh/*.iocsh)
STARTUP += $(wildcard ../startup/*.cmd)

# Note that architecture-specific source files can be specified:
#
#     SOURCES_linux-x86_64 += ...
#     SOURCES_linux
#
# These are also valid for many of the compile flags specified by e.g.
#     CFLAGS CXXFLAGS CPPFLAGS
# i.e.
#     USR_CFLAGS_linux-ppc64e6500 += ...


#OP_SYS_CXXFLAGS += -std=c++17
#ARCH_DEP_CXXFLAGS += -std=c++17
USR_CXXFLAGS += --std=c++11

# Same as with any source or header files, you can also use $SUBS and $TMPS to define
# database files to be inflated (using MSI), e.g.
#
#     SUBS = $(wildcard $(APPDB)/*.substitutions)
#     TMPS = $(wildcard $(APPDB)/*.template)

#SUBS = $(wildcard $(APPDB)/*.substitutions)
#TMPS = $(wildcard $(APPDB)/*.template)

USR_DBFLAGS += -I . -I ..
USR_DBFLAGS += -I $(EPICS_BASE)/db
USR_DBFLAGS += -I $(APPDB)

# Inflate netStat.db from netStat.substitutions. I couldn't figure out how to force to inflate
# netStat first because netStat.db is used to create other db files from substitution files.
# This target creates $(APPDB)/netStat.db


.PHONY: vlibs
vlibs:
#--------------------------

#$(DBDINC_DEPS): $(DBDINC_HDRS)

#.dbd.h:
#	$(DBTORECORDTYPEH)  $(USR_DBDFLAGS) -o $@ $<

#.PHONY: $(DBDINC_DEPS) .dbd.h



#db: 

#.PHONY: db 
