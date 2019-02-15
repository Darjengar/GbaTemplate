#
# Template for projects using grit.
# 
# Making gfx into a library in a separate makefile and using that here.
#
# BUGS: This Makefile delete after one execution the processed GFX sources
#		in build directory. You have to execute this file twice if you want
#		the processed gfx sources in build directory for better updating.
# TODO: Bugfixing, Make the GFX Makefile independent

# ---------------------------------------------------------------------
# SETUP
# ---------------------------------------------------------------------

# --- No implicit rules ---
.SUFFIXES:

SHELL	:= /bin/bash	# Do not think about to change this variable

export ROOT  ?= $(CURDIR)

# --- Check for needed environment variables ---

include $(ROOT)/tonc_rules.mk

# --- Main path ---
export PATH	:=	$(DEVKITARM)/bin:$(PATH)

define is-imm-var
  $(eval $(if $($1),,$1 := $2))
endef

# ---------------------------------------------------------------------
# PROJECT DETAILS
# ---------------------------------------------------------------------

# PROJ		: Base project name
# TITLE		: Title for ROM header (12 characters)
# LIBS		: Libraries to use, formatted as list for linker flags
# BUILD		: Directory for build process temporaries. Should NOT be empty!
# APPDIR	: Directory for GBA application
# SRCDIRS	: List of source file directories
# DATADIRS	: List of data file directories
# INCDIRS	: List of header file directories
# LIBDIR	: Directory of project libraries. ONLY FOR LIBS OF BUILD PROCESS!
# LIBDIRS	: List of library directories
# General note: use . for the current dir, don't leave them empty.

export PROJ		?= $(notdir $(CURDIR))
TITLE			:= $(PROJ)
MKR_CODE		:= JN
GAME_VER		:= 0.0

export GFXLIBS	?= $(CURDIR)/lib/libgfx.a
LIBS			:= -ltonc -lgfx

export BUILD	:= bld
APPDIR			:= app
SRCDIRS			:= src asset
DATADIRS		:= data
INCDIRS			:= include
LIBDIR			:= lib
LIBDIRS			:= $(DEVKITPRO)/libtonc 


# --- switches ---

bMB		:= 0	# Multiboot build
bTEMPS	:= 0	# Save gcc temporaries (.i and .s files)
bDEBUG	:= 0	# Generate debug info


# ---------------------------------------------------------------------
# BUILD FLAGS
# ---------------------------------------------------------------------

# This is probably where you can stop editing

# --- Architecture ---

ARCH    := -mthumb-interwork -mthumb
RARCH   := -mthumb-interwork -mthumb
IARCH   := -mthumb-interwork -marm -mlong-calls

# --- Main flags ---

ifeq ($(strip $(bDEBUG)), 2)
  CFLAGS	:= -mcpu=arm7tdmi -mtune=arm7tdmi $(ARCH) -O3
  CFLAGS	+= -Wall
  CFLAGS	+= $(INCLUDE)
  CFLAGS	+=

  CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

  ASFLAGS	:= $(ARCH)
  LDFLAGS := $(ARCH) -Wl,-Map,../$(APPDIR)/$(PROJ)_debug_VBA.map
else ifeq ($(strip $(bDEBUG)), 1)
  CFLAGS	:= -mcpu=arm7tdmi -mtune=arm7tdmi $(ARCH) -O3
  CFLAGS	+= -Wall
  CFLAGS	+= $(INCLUDE)
  CFLAGS	+=

  CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

  ASFLAGS	:= $(ARCH)
  LDFLAGS := $(ARCH) -Wl,-Map,../$(APPDIR)/$(PROJ)_debug_VSC.map
else
  CFLAGS	:= -mcpu=arm7tdmi -mtune=arm7tdmi $(ARCH) -O2
  CFLAGS	+= -Wall
  CFLAGS	+= $(INCLUDE)
  CFLAGS	+= -ffast-math -fno-strict-aliasing

  CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

  ASFLAGS	:= $(ARCH)
  LDFLAGS := $(ARCH) -Wl,-Map,../$(APPDIR)/$(PROJ).map
endif

# --- switched additions ----------------------------------------------

# --- Multiboot ? ---
ifeq ($(strip $(bMB)), 1)
  TARGET	:= $(PROJ).mb
else
  ifeq ($(strip $(bDEBUG)), 2)
    TARGET	:= $(PROJ)_debug_VBA
  else ifeq ($(strip $(bDEBUG)), 1)
    TARGET	:= $(PROJ)_debug_VSC
  else
    TARGET	:= $(PROJ)
  endif
endif
	
# --- Save temporary files ? ---
ifeq ($(strip $(bTEMPS)), 1)
	CFLAGS		+= -save-temps
	CXXFLAGS	+= -save-temps
endif

# --- Debug info ? ---

ifeq ($(strip $(bDEBUG)), 2)
  CFLAGS	+= -DNDEBUG
  CXXFLAGS	+= -DNDEBUG
  ASFLAGS	+= -DNDEBUG
else ifeq ($(strip $(bDEBUG)), 1)
  CFLAGS	+= -DDEBUG -g
  CXXFLAGS	+= -DDEBUG -g
  ASFLAGS	+= -DDEBUG -g
  LDFLAGS	+= -g
else
  CFLAGS	+= -DNDEBUG
  CXXFLAGS	+= -DNDEBUG
  ASFLAGS	+= -DNDEBUG
endif

# ---------------------------------------------------------------------
# BUILD PROCEDURE
# ---------------------------------------------------------------------

ifneq ($(BUILD),$(notdir $(CURDIR)))

# Still in main dir: 
# * Define/export some extra variables
# * Invoke this file again from the build dir
# PONDER: what happens if BUILD == "" ?

export APP		:=	$(CURDIR)/$(APPDIR)/$(TARGET)
export VPATH	:=	$(CURDIR)

export DEPSDIR	:=	$(CURDIR)/$(BUILD)


# --- List source and data files ---
export SOURCES	:=	$(shell find $(SRCDIRS) -type f)
src_subdirs 	:=	$(dir $(SOURCES))
bld_dirs 		:=	$(addprefix $(BUILD)/,$(src_subdirs))
bld_dirs		+=	$(APPDIR) $(LIBDIR)

# Create build directories only if the goal is not clean
ifneq "$(MAKECMDGOALS)" "clean"
  create-bld-dirs :=	$(shell	for dir in $(bld_dirs);					\
								do 										\
									[ -d $$dir ] || mkdir -p $$dir;		\
								done)
endif

CFILES		:=	$(filter src/%.c,$(SOURCES))
CPPFILES	:=	$(filter src/%.cpp,$(SOURCES))
SFILES		:=	$(filter src/%.s,$(SOURCES))

# --- Set linker depending on C++ file existence ---
ifeq ($(strip $(CPPFILES)),)
	export LD	:= $(CC)
else
	export LD	:= $(CXX)
endif

# --- Define object file list ---
export OFILES	:=									\
	$(addsuffix .o, $(BINFILES))					\
	$(CFILES:.c=.o) $(CPPFILES:.cpp=.o)				\
	$(SFILES:.s=.o)

# --- Create include and library search paths ---
export INCLUDE	:=									\
	$(foreach dir,$(INCDIRS),-I$(CURDIR)/$(dir))	\
	$(foreach dir,$(LIBDIRS),-I$(dir)/include)		\
	-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=									\
	-L$(CURDIR)/$(LIBDIR) 							\
	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)


# --- More targets ----------------------------------------------------

.PHONY: $(BUILD) clean

# --- Create $(BUILD) if necessary, and run this makefile from there ---

$(BUILD):
	@make --no-print-directory -f asset/gfx/Makefile
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

all	: $(BUILD)

clean:
	@echo clean ...
	@rm -fvr $(BUILD) $(APPDIR) $(LIBDIR)


else		# If we're here, we should be in the BUILD dir

DEPENDS	:=	$(OFILES:.o=.d)

# --- Main targets ----

$(APP).gba	:	$(APP).elf

$(APP).elf	:	$(OFILES) $(GFXLIBS) include/gfx.h


-include $(DEPENDS)


endif		# End BUILD switch

# EOF
