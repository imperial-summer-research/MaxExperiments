
####################################################
#       Makefile ------- Thomas Chau 2013          #
#       Tested for MaxCompiler 2013.2.2            #
####################################################
# 1)    For SLiC interface only
#       If you are using MaxCompilerRT,
#       use maxfilecompile instead of sliccompile
#       and change LFLAGS -lslic to -lmaxcompilerrt
# 2)    This makefile uses common settings and
#       compilation targets defined in the root folder
# 3)    Usage
#       -----
#       Build and run simulation:  make runsim
#       Kill failed simulation:    make stopsim
#       Debug simulation:          make debug
#       Build DFE:                 make build
#       Run DFE:                   make run
#       Clean binaries:            make clean
#       Clean all generated files: make distclean


# ---- Project settings ----

WHEREISROOT=../..

# Assuming Kernel and Manager class names follow the pattern below:
PRJ=MultiRomRead
KERNEL=$(PRJ)Kernel
MANAGER=$(PRJ)Manager

#TREE_WIDTH?=002
#STREAM_FREQUENCY?=100
#BUILD_PARAMS:=treeWidth=$(TREE_WIDTH) streamFrequency=$(STREAM_FREQUENCY)
#BUILD_NAME_OPTION:=_treeWidth$(TREE_WIDTH)_streamFrequency$(STREAM_FREQUENCY)

NUM_PORTS?=002
NUM_PIPES?=002
BUILD_PARAMS:=numPorts=$(NUM_PORTS) numPipes=$(NUM_PIPES)
BUILD_NAME_OPTION:=_NumPorts$(NUM_PORTS)_NumPipes$(NUM_PIPES)

# Using known hardware settings
include $(WHEREISROOT)/Makefile.Max3.hardware


# Using common compillation targets and compiler settings
include $(WHEREISROOT)/Makefile.common

# Using utilities Makefile
# include Makefile.utils
