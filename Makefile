##
## @file Makefile
## mypopen - a simplified version of popen(3)
## Beispiel 2
##
## @author Manuel Seifner	 <ic17b022@technikum-wien.at>
## @author Oliver Safar		 <ic17b077@technikum-wien.at>
## @date 2018/05/4
##
## @version 1.0 $
##
##

##
##

##
## ------------------------------------------------------------- variables --
##

CC=gcc52
CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -Wformat=2 -fno-common -ftrapv -g -O3 -std=gnu11
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

LDLIBS = -lm
OBJ=mypopen.o
HEADER=mypopen.h
TEST=popentest
TESTPIPE=test-pipe

LIB_PATH_TEST=-L./libpopentest
LIB_PATH_PIPE=-L./libtest-pipe
LIB_TEST=-lpopentest
LIB_PIPE=-ltest-pipe

##
## ----------------------------------------------------------------- rules --
##

%.c: %o
	    $(CC) $(CFLAGS) -c

##
## --------------------------------------------------------------- targets --
##

.PHONY: all
all: test test-pipe

# link against the testfolder on annuminas
test: $(OBJ)
	    $(CC) -o$(TEST) $(OBJ) $(LIB_TEST) -ldl #$(LIB_PATH_TEST)
        # to run command directly
        #./popentest
# link against the dynamic test suite test-pipe
test-pipe: $(OBJ)
	$(CC) -o$(TESTPIPE) $(OBJ) $(LIB_PIPE)    # $(LIB_PATH_PIPE)
	    # to run command directly
	    #./test-pipe

.PHONY: clean
clean:
	rm -f *.o mypopen
	rm -f popentest
	rm -f test-pipe

.PHONY: distclean
distclean: clean
	$(RM) -rf doc

doc: html pdf

.PHONY: html
html:
	$(DOXYGEN) doxygen.dcf

pdf: html
	$(CD) doc/pdf && \
	$(MV) refman.tex refman_save.tex && \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex && \
	$(RM) refman_save.tex && \
	make && \
	$(MV) refman.pdf refman.save && \
	$(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile && \
	$(MV) refman.save refman.pdf