# -----------------------------------------------------------------------------
# Compiler and its Options
CXX=g++
CXXFLAGS=-g -Wall -std=c++0x # -fpermissive

# -----------------------------------------------------------------------------
# Common Include Directories
#
# ILM: find ../ilmbase-1.0.2/ -name "*.h" -exec dirname {} \; |  sort -u
# OPENEXR: find ../openexr-1.7.0/ -name "*.h" -exec dirname {} \; |  sort -u
#

ILM_INCLUDE_DIR= -Iilmbase-1.0.2/config -Iilmbase-1.0.2/config.windows -Iilmbase-1.0.2/Half -Iilmbase-1.0.2/HalfTest -Iilmbase-1.0.2/Iex -Iilmbase-1.0.2/IexTest -Iilmbase-1.0.2/IlmThread -Iilmbase-1.0.2/Imath -Iilmbase-1.0.2/ImathTest -Iilmbase-1.0.2/include/OpenEXR -Iilmbase-1.0.2/vc/createDLL 

OPENEXR_INCLUDE_DIR= -Iopenexr-1.7.0/config -Iopenexr-1.7.0/config.windows -Iopenexr-1.7.0/exrenvmap -Iopenexr-1.7.0/exrmakepreview -Iopenexr-1.7.0/exrmaketiled -Iopenexr-1.7.0/exrmultiview -Iopenexr-1.7.0/IlmImf -Iopenexr-1.7.0/IlmImfExamples -Iopenexr-1.7.0/IlmImfFuzzTest -Iopenexr-1.7.0/IlmImfTest

# -----------------------------------------------------------------------------
# Common Library Directories
# 
# find ../ -name "lib*" -exec dirname {} \; |  sort -u
# 

LIBDIR=-Lilmbase-1.0.2 -Lilmbase-1.0.2/Half -Lilmbase-1.0.2/Half/.libs -Lilmbase-1.0.2/Iex -Lilmbase-1.0.2/Iex/.libs -Lilmbase-1.0.2/IlmThread -Lilmbase-1.0.2/IlmThread/.libs -Lilmbase-1.0.2/Imath -Lilmbase-1.0.2/Imath/.libs -Lilmbase-1.0.2/lib -Lopenexr-1.7.0 -Lopenexr-1.7.0/IlmImf -Lopenexr-1.7.0/IlmImf/.libs


# -----------------------------------------------------------------------------
# Common Library
#

LIBS=-lIlmImf


# -----------------------------------------------------------------------------
# Main Compilation
#

bin/main.out : src/main.cpp ILM_BASE OpenEXR
	$(CXX) $(CXXFLAGS) $(LIBDIR) $(ILM_INCLUDE_DIR) $(OPENEXR_INCLUDE_DIR) $^ $(LIBS) -o bin/main.out


# -----------------------------------------------------------------------------
# External Library Compilation
# 

# IlmBase

ILM_BASE_DIR=$(shell readlink -f ilmbase-1.0.2)

ILM_BASE : 
	cd $(ILM_BASE_DIR) && ./configure --prefix `pwd` && make && make install


# OpenExr
# This must be configured after ILM_BASE

OPENEXR_DIR=$(shell readlink -f openexr-1.7.0)

OpenEXR : 
	cd $(OPENEXR_DIR) && ./configure --prefix=`pwd` --with-ilmbase-prefix=$(ILM_BASE_DIR) && make && make install


# -----------------------------------------------------------------------------
# Misc
# 

CLEAN_ILM :
	cd $(ILM_BASE_DIR) && make clean

CLEAN_OPENEXR : 
	cd $(OPENEXR_DIR) && make clean

clean : CLEAN_ILM CLEAN_OPENEXR
	rm bin/main.out

