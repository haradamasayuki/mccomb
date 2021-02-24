#
# In case of building locally, set SKOFL_ROOT variable 
#      setenv SKOFL_ROOT   ... 
#  or directly set in this makefile 
#SKOFL_ROOT = /skofl
#

include $(SKOFL_ROOT)/config.gmk

PWD     := $(shell pwd)

ROOT_INC     = $(shell root-config --cflags)
ROOT_LIBS    = $(shell root-config --libs)

LOCAL_INC	+= -I$(PWD)/include \
        -I$(SKOFL_ROOT)/include -I$(SKOFL_ROOT)/inc/lowe


LOCAL_LIBS	= $(APLIB) $(APLIB) \
	-lsklowe_7.0 -lsollib_4.0 -lwtlib_5.1 -lbonsai_3.3 -lstmu -lska -ltqrealroot\
	-L/home/iida/relic/lib -lrelic  #TreeManager.o skroot_init.o

APLIB =  -lapdrlib -lpolfit -laplib -lringlib  -ltp -ltf  -lringlib  \
         -laplib -lmsfit -lmslib -lseplib -lmsfit -lprtlib -lmuelib \
         -lffit -lodlib -lstmu -laplowe -lpolfit -lexpq  -laplib -lska 


CXXFLAGS += -std=c++11
:q


ALLDEFINES += -DDEBUG 

#
#  Objects
#
SRCS = $(wildcard src/*.cc)
OBJS = $(patsubst src/%.cc, obj/%.o, $(SRCS))
#OBJS   = MergeRealDark.o #initialize.o #TreeManager.o skroot_init.o 


all: bin/mccomb

bin/mccomb : $(OBJS) obj/main.o bin
	@echo "[mccomb] Building mccomb ..."
	@LD_RUN_PATH=$(LIBDIR):$(SKOFL_LIBDIR):$(ROOTSYS)/lib:$(LIBDIR) $(CXX) $(CXXFLAGS) -o $@ obj/main.o $(OBJS)  $(LDLIBS) 
	@echo "[mccomb] Done !!!"

obj/main.o : main.cc obj
	@echo "[mccomb] Building main ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

#@LD_RUN_PATH=$(LIBDIR):$(SKOFL_LIBDIR):$(ROOTSYS)/lib:$(LIBDIR) $(CXX) $(CXXFLAGS) -o $@ $(OBJS)  $(LDLIBS) 
#@echo "[mccomb] Done !!!"

obj/%.o: src/%.cc obj
	@echo "[mccomb] Building $* ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj bin:
	@mkdir $@

.PHONY:  clean 

clean: 
	@$(RM) *.o *~ core fort.* $(OBJS) mccomb
	@echo "[mccomb] Cleaning ..."

#install.exec: 

#exec:: mccomb

#setup includes install.includes depend lib install.lib exec install.exec

