# -----------------------------------------------------------------------------
# Compiler and its Options
CXX=g++
CXXFLAGS=-g -Wall -std=c++0x # -fpermissive


# -----------------------------------------------------------------------------
# Common Include Directories
#
# ILM: find ../ilmbase-1.0.2/ -name "*.h" -exec dirname {} \; |  sort -u
# OPENEXR: find ../openexr-1.7.0/ -name "*.h" -exec dirname {} \; |  sort -u


ILM_INCLUDE_DIR= -I../ilmbase-1.0.2/config -I../ilmbase-1.0.2/config.windows -I../ilmbase-1.0.2/Half -I../ilmbase-1.0.2/HalfTest -I../ilmbase-1.0.2/Iex -I../ilmbase-1.0.2/IexTest -I../ilmbase-1.0.2/IlmThread -I../ilmbase-1.0.2/Imath -I../ilmbase-1.0.2/ImathTest -I../ilmbase-1.0.2/include/OpenEXR -I../ilmbase-1.0.2/vc/createDLL 

OPENEXR_INCLUDE_DIR= -I../openexr-1.7.0/config -I../openexr-1.7.0/config.windows -I../openexr-1.7.0/exrenvmap -I../openexr-1.7.0/exrmakepreview -I../openexr-1.7.0/exrmaketiled -I../openexr-1.7.0/exrmultiview -I../openexr-1.7.0/IlmImf -I../openexr-1.7.0/IlmImfExamples -I../openexr-1.7.0/IlmImfFuzzTest -I../openexr-1.7.0/IlmImfTest

# -----------------------------------------------------------------------------
# Common Library Directories

# find ../ -name "lib*" -exec dirname {} \; |  sort -u

LIBDIR=-L../ilmbase-1.0.2 -L../ilmbase-1.0.2/Half -L../ilmbase-1.0.2/Half/.libs -L../ilmbase-1.0.2/Iex -L../ilmbase-1.0.2/Iex/.libs -L../ilmbase-1.0.2/IlmThread -L../ilmbase-1.0.2/IlmThread/.libs -L../ilmbase-1.0.2/Imath -L../ilmbase-1.0.2/Imath/.libs -L../ilmbase-1.0.2/lib -L../openexr-1.7.0 -L../openexr-1.7.0/IlmImf -L../openexr-1.7.0/IlmImf/.libs

# -----------------------------------------------------------------------------
# Common Library
LIBS=-lIlmImf


# -----------------------------------------------------------------------------
# Compilation

main.out : main.cpp
	$(CXX) $(CXXFLAGS) $(LIBDIR) $(ILM_INCLUDE_DIR) $(OPENEXR_INCLUDE_DIR) $^ $(LIBS) -o main.out


