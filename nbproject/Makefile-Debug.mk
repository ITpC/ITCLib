#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/ITCError.o \
	${OBJECTDIR}/src/Runnable.o \
	${OBJECTDIR}/src/Thread.o \
	${OBJECTDIR}/src/bzError.o \
	${OBJECTDIR}/src/globals.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-g -pipe -pthread -D_REENTRANT -D_THREAD_SAFE -O2 -fPIC -march=native -mtune=native -fomit-frame-pointer -mfpmath=sse -ftree-vectorize -funroll-loops -mno-tls-direct-seg-refs -DBZ_NO_STDIO -DLOG_DEBUG -DMAX_BUFF_SIZE=256 -DTSAFE_LOG=1 -std=c++11
CXXFLAGS=-g -pipe -pthread -D_REENTRANT -D_THREAD_SAFE -O2 -fPIC -march=native -mtune=native -fomit-frame-pointer -mfpmath=sse -ftree-vectorize -funroll-loops -mno-tls-direct-seg-refs -DBZ_NO_STDIO -DLOG_DEBUG -DMAX_BUFF_SIZE=256 -DTSAFE_LOG=1 -std=c++11

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libitclib.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libitclib.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libitclib.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libitclib.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libitclib.a

${OBJECTDIR}/src/ITCError.o: src/ITCError.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../utils/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ITCError.o src/ITCError.cpp

${OBJECTDIR}/src/Runnable.o: src/Runnable.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../utils/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Runnable.o src/Runnable.cpp

${OBJECTDIR}/src/Thread.o: src/Thread.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../utils/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Thread.o src/Thread.cpp

${OBJECTDIR}/src/bzError.o: src/bzError.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../utils/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/bzError.o src/bzError.cpp

${OBJECTDIR}/src/globals.o: src/globals.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Iinclude -I../utils/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/globals.o src/globals.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libitclib.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
