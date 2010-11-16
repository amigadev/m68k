TARGET   = m68k
SUBDIRS  =
CFLAGS	 = -O2 -pipe -fomit-frame-pointer -Wall

MAKETARGET=$(firstword $(MAKECMDGOALS))
ifneq ($(MAKETARGET),$(MAKECMDGOALS))
%:
	@echo "ERROR: More than one make-target specified!"
	@echo "Targets available: all clean distclean"
	@false
else

DEPENDFILE=.depend
DISTDIR=$(PACKAGE)-$(VERSION)
SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

.PHONY: all clean distclean tar $(TARGET) $(SUBDIRS)

all: $(TARGET)

fastbuild: ${SOURCES}
	find *.c -printf "#include \"%p\"\n" > .fastbuild.c
	gcc -o ${TARGET} ${CFLAGS} ${LDFLAGS} .fastbuild.c

$(TARGET): $(OBJECTS)
	gcc -o $@ $(OBJECTS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	make -C $@ $(MAKETARGET)

# clean intermediate files
clean: subdirs
	-rm -f *.o $(TARGET) $(OBJECTS) .fastbuild.c

# clean all unneeded files to simplify re-distribution
distclean: subdirs
	-rm -f *~ DEADJOE $(DEPENDFILE)
	-rm -f *.o $(TARGET) $(OBJECTS) .fastbuild.c

# automatic dependency updates
$(DEPENDFILE): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -MM $(SOURCES) > $(DEPENDFILE)
-include $(DEPENDFILE)

endif
