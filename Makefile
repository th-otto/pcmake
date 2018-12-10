CROSS_PREFIX = m68k-atari-mint-

CC = $(CROSS_PREFIX)gcc
AS = $(CROSS_PREFIX)gcc
LD = $(CC)
WARNINGS = -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes
CFLAGS = -Os -fomit-frame-pointer $(WARNINGS)
LDFLAGS = -s

all: pcmake.ttp


OBJS = \
	pcmake.o \
	prj.o \
	exec.o \
	asopts.o \
	ccopts.o \
	ldopts.o \
	utils.o \
	list.o \
	getopt.o \
	$(empty)

pcmake.ttp: $(OBJS)
	$(LD) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

clean::
	rm -f *.o *.pdb *.ttp
