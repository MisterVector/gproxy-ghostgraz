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
CC=mingw32-gcc.exe
CCC=mingw32-g++.exe
CXX=mingw32-g++.exe
FC=gfortran
AS=as.exe
QMAKE=qmake.exe

# Macros
CND_PLATFORM=Qt_MinGW-Windows
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES=


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-Lsource/gmp/lib -Lsource/ms_sdk -Lsource/mysql/lib -Lsource/pdcurses source/ms_sdk/version.lib source/ms_sdk/winmm.lib source/ms_sdk/ws2_32.lib ../BNCSUtil/dist/Release/Qt_MinGW-Windows/BNCSUtil.dll

nbproject/qt-${CND_CONF}.mk: nbproject/qt-${CND_CONF}.pro FORCE
	${QMAKE} VPATH=. -spec win32-g++ -o qttmp-${CND_CONF}.mk nbproject/qt-${CND_CONF}.pro
	mv -f qttmp-${CND_CONF}.mk nbproject/qt-${CND_CONF}.mk
	@sed -e 's/\/qt\/bin/\/qt\/bin\//g' nbproject/qt-${CND_CONF}.mk >nbproject/qt-${CND_CONF}.tmp
	@mv -f nbproject/qt-${CND_CONF}.tmp nbproject/qt-${CND_CONF}.mk

FORCE:

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} nbproject/qt-${CND_CONF}.mk
	"${MAKE}" -f nbproject/qt-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/GProxy.exe

${CND_BUILDDIR}/Release/%.o: nbproject/qt-${CND_CONF}.mk
	$(MAKE) -f nbproject/qt-${CND_CONF}.mk "$@"

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS} nbproject/qt-${CND_CONF}.mk
	$(MAKE) -f nbproject/qt-${CND_CONF}.mk distclean

# Subprojects
.clean-subprojects: