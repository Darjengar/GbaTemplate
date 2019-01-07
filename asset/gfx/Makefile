#
# Making a gfx library from grit output
#
# For use on data-arrays only. NO CODE!!!
# ONLY ACCESSIBLE FROM TOPLEVEL MAKEFILE
# TODO: Independent from toplevel Makefile
#

# ---------------------------------------------------------------------
# SETUP
# ---------------------------------------------------------------------

export PATH	:=	$(DEVKITARM)/bin:$(PATH)

GRIT		:= grit

.SUFFIXES:

include $(DEVKITARM)/gba_rules

# ---------------------------------------------------------------------
# PROJECT DETAILS
# ---------------------------------------------------------------------

# GFXTITLE	: Graphics library name
# BUILD		: Directory for build process temporaries. Should NOT be empty!
# GFXDIRS	: List of graphics directories
# GFXEXTS	: Graphics extensions.
# General note: use . for the current dir, don't leave them empty.

BUILD			?= bld
GFXDIRS			:= asset/gfx
GFXLIB			?= lib/libgfx.a
export GFXHDR	?= $(CURDIR)/include/gfx.h

GFXEXTS			:= png bmp

# --- Exceptions ---
# Add files/file-variables for special rules here. Put the rules 
# At the bottom of the makefile. Be careful with directories, as
# we'll be in $(BUILD) when converting.
# GFXSPECIALS	: removed from GFXFILES
# OSPECIALS		: added to OFILES

# Key exception variables
export GFXSPECIALS	:=
OSPECIALS			:= 


# ---------------------------------------------------------------------
# BUILD FLAGS
# ---------------------------------------------------------------------

# Since there's no code to compile, we won't need optimizations, 
# architectures etc.

CFLAGS		:= 
CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS		:= 


# ---------------------------------------------------------------------
# BUILD PROCEDURE
# ---------------------------------------------------------------------

ifneq ($(BUILD),$(notdir $(CURDIR)))

# still in main directory.

export TARGET	:=	$(CURDIR)/$(GFXLIB)
export VPATH	:=	$(CURDIR)
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

GFXFILES		:= $(filter %.png,$(SOURCES))
export OFILES	:= $(patsubst %.png,%.o,$(GFXFILES))


# --- More targets ----------------------------------------------------

.PHONY: $(BUILD) clean

# --- Create BUILD if necessary, and run this makefile from there ---

$(BUILD):
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/asset/gfx/Makefile

all	: $(BUILD)

clean:
	@echo clean ...
	@rm -fvr $(BUILD) $(TARGET) $(GFXHDR)

	
# ---------------------------------------------------------------------

else

DEPENDS	:=	$(OFILES:.o=.d)

.PHONY : all 

all : $(TARGET) $(GFXHDR)

$(TARGET) : $(OFILES)
	@echo Archiving into $(notdir $@)
	-@rm -fv $@
	@$(AR) -crs $@ $(OFILES)


$(GFXHDR) : $(OFILES)
	@echo "Creating master header: $@"
	@$(call create-master-header,$@,$(subst .o,.h,$^),0)


# ---------------------------------------------------------------------
# BASE CONVERSION RULES
# ---------------------------------------------------------------------

# --- With separate .grit file ---

%.s %.h	: %.png %.grit
	@$(GRIT) $< -o$* -fts

%.s %.h	: %.bmp %.grit
	@$(GRIT) $< -o$* -fts

%.s %.h	: %.pcx %.grit
	@$(GRIT) $< -o$* -fts

%.s %.h	: %.jpg %.grit
	@$(GRIT) $< -o$* -fts


# --- Without .grit file ; uses dirname/dirname.grit for options ---

%.s %.h	: %.png
	$(GRIT) $< -o$* -fts -ff $(<D)/$(notdir $(<D)).grit

%.s %.h	: %.bmp
	$(GRIT) $< -o$* -fts -ff $(<D)/$(notdir $(<D)).grit

%.s %.h	: %.pcx
	$(GRIT) $< -o$* -fts -ff $(<D)/$(notdir $(<D)).grit

%.s %.h	: %.jpg
	$(GRIT) $< -o$* -fts -ff $(<D)/$(notdir $(<D)).grit


# ---------------------------------------------------------------------
# SPECIAL RULES 
# ---------------------------------------------------------------------


# ---------------------------------------------------------------------
# FUNCTIONS
# ---------------------------------------------------------------------

# ! type: shell script only use in recipe
# call create-master-header,file_name,header_list,remove?
define create-master-header
	printf "//\n// $(notdir $1)\n//\n\n		\
  			#ifdef __cplusplus\nextern \"C\" {\n#endif" > $1
  	cat $2 >> $1
  	printf "\n#ifdef __cplusplus\n};\n#endif // $(notdir $1)" >> $1
	if [ $3 -eq 1 ];	\
	then				\
		rm -fvr $2;		\
	fi
endef

## if you just want to include the separate headers, use this instead of cat:
#	for hdr in $2 ;	\
#		do echo -e "#include \"$$hdr\"" >> $1 ; done;

# --- odds and ends ---

## Get the title-part of filename.
define title		# $1: filepath
	$(basename $(notdir $1))
endef

## Get a valid C identifier for a name.
define cident		# $1: name
	`echo $1 | sed -e 's|^\([0-9]\)|_\1| ; s|[./\\-]|_|g'`
endef

## Create a header file for a bin2s converted binary.
define bin-header		# $1: path, $2: identifier
	echo "extern const u32 $(strip $2)_size;" >  $1
	echo "extern const u8 $(strip $2)[];"     >> $1	
endef

# ---------------------------------------------------------------------
# DEPENDENCIES
# ---------------------------------------------------------------------

-include $(DEPENDS)

endif
