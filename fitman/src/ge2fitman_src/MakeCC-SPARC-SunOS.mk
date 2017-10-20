## -*- Makefile -*-
##
## User: ckiss
## Time: May 17, 2005 1:58:42 PM
## Makefile created by the Native Languages Module.
##
## This file is generated automatically -- Changes will be lost if regenerated
##


#### Compiler and tool definitions shared by all build targets #####
CCC = CC
CXX = CC
BASICOPTS = -g -Bstatic
CCFLAGS = $(BASICOPTS)
CXXFLAGS = $(BASICOPTS)
CCADMIN = CCadmin -clean


# Define the target directories.
TARGETDIR_ge2fitman=./SPARC-SunOS-run-CC


all: $(TARGETDIR_ge2fitman)/ge2fitman

## Target: ge2fitman
OBJS_ge2fitman =  \
	$(TARGETDIR_ge2fitman)/ge2fitman_com_line.o \
	$(TARGETDIR_ge2fitman)/ge2fitman_fmtext_o.o \
	$(TARGETDIR_ge2fitman)/ge2fitman_preproc.o \
	$(TARGETDIR_ge2fitman)/ge2fitman_read_procpar.o \
	$(TARGETDIR_ge2fitman)/ge2fitman_sup.o \
	$(TARGETDIR_ge2fitman)/ge2fitman_error.o \
	$(TARGETDIR_ge2fitman)/ge2fitman.o


# Link or archive
$(TARGETDIR_ge2fitman)/ge2fitman: $(TARGETDIR_ge2fitman) $(OBJS_ge2fitman)
	$(LINK.cc) $(CCFLAGS_ge2fitman) $(CPPFLAGS_ge2fitman) -o $@ $(OBJS_ge2fitman) $(LDLIBS_ge2fitman)


# Compile source files into .o files
$(TARGETDIR_ge2fitman)/ge2fitman_com_line.o: $(TARGETDIR_ge2fitman) ge2fitman_com_line.cpp
	$(COMPILE.cc) $(CCFLAGS_ge2fitman) $(CPPFLAGS_ge2fitman) -o $@ ge2fitman_com_line.cpp

$(TARGETDIR_ge2fitman)/ge2fitman_fmtext_o.o: $(TARGETDIR_ge2fitman) ge2fitman_fmtext_o.cpp
	$(COMPILE.cc) $(CCFLAGS_ge2fitman) $(CPPFLAGS_ge2fitman) -o $@ ge2fitman_fmtext_o.cpp

$(TARGETDIR_ge2fitman)/ge2fitman_preproc.o: $(TARGETDIR_ge2fitman) ge2fitman_preproc.cpp
	$(COMPILE.cc) $(CCFLAGS_ge2fitman) $(CPPFLAGS_ge2fitman) -o $@ ge2fitman_preproc.cpp

$(TARGETDIR_ge2fitman)/ge2fitman_read_procpar.o: $(TARGETDIR_ge2fitman) ge2fitman_read_procpar.cpp
	$(COMPILE.cc) $(CCFLAGS_ge2fitman) $(CPPFLAGS_ge2fitman) -o $@ ge2fitman_read_procpar.cpp

$(TARGETDIR_ge2fitman)/ge2fitman_sup.o: $(TARGETDIR_ge2fitman) ge2fitman_sup.cpp
	$(COMPILE.cc) $(CCFLAGS_ge2fitman) $(CPPFLAGS_ge2fitman) -o $@ ge2fitman_sup.cpp

$(TARGETDIR_ge2fitman)/ge2fitman_error.o: $(TARGETDIR_ge2fitman) ge2fitman_error.cpp
	$(COMPILE.cc) $(CCFLAGS_ge2fitman) $(CPPFLAGS_ge2fitman) -o $@ ge2fitman_error.cpp

$(TARGETDIR_ge2fitman)/ge2fitman.o: $(TARGETDIR_ge2fitman) ge2fitman.cpp
	$(COMPILE.cc) $(CCFLAGS_ge2fitman) $(CPPFLAGS_ge2fitman) -o $@ ge2fitman.cpp



#### Clean target deletes all generated files ####
clean:
	rm -f \
		$(TARGETDIR_ge2fitman)/ge2fitman \
		$(TARGETDIR_ge2fitman)/ge2fitman_com_line.o \
		$(TARGETDIR_ge2fitman)/ge2fitman_fmtext_o.o \
		$(TARGETDIR_ge2fitman)/ge2fitman_preproc.o \
		$(TARGETDIR_ge2fitman)/ge2fitman_read_procpar.o \
		$(TARGETDIR_ge2fitman)/ge2fitman_sup.o \
		$(TARGETDIR_ge2fitman)/ge2fitman_error.o \
		$(TARGETDIR_ge2fitman)/ge2fitman.o
	$(CCADMIN)
	rm -f -r $(TARGETDIR_ge2fitman)


# Create the target directory (if needed)
$(TARGETDIR_ge2fitman):
	mkdir -p $(TARGETDIR_ge2fitman)


# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.Sun-sparc-Solaris
