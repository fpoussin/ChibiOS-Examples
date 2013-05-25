
DSPBASE = .

include BasicMathFunctions/files.mk
include CommonTables/files.mk
include ComplexMathFunctions/files.mk
include ControllerFunctions/files.mk
include FastMathFunctions/files.mk
include FilteringFunctions/files.mk
include MatrixFunctions/files.mk
include StatisticsFunctions/files.mk
include SupportFunctions/files.mk
include TransformFunctions/files.mk

CROSS_COMPILE = arm-none-eabi-
AR = $(CROSS_COMPILE)ar

rm=rm -f
MKDIR_P = mkdir -p

OBJDIR = ../build/obj

DSPOBJ  := $(DSPSRC:%.c=%.o)
OBJECTS := $(foreach file,${DSPOBJ},$(OBJDIR)/$(notdir ${file}))

default: all
all: libmath_cm4f.a

libmath_cm4f.a: $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)
	
$(OBJECTS):
	$(error $@ is missing, please compile the project without the static lib first)
	
showobj:
	@echo $(OBJECTS)

clean:
	@$(rm) libmath_cm4f.a
	@echo "Cleanup complete!"
