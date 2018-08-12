# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = lib
DESTDIR = dist/Debug/MinGW-Windows
TARGET = BNCSUtil
VERSION = 
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += dll debug 
PKGCONFIG +=
QT = 
SOURCES += src/bncsutil/bsha1.cpp src/bncsutil/cdkeydecoder.cpp src/bncsutil/checkrevision.cpp src/bncsutil/decodekey.cpp src/bncsutil/file.cpp src/bncsutil/libinfo.cpp src/bncsutil/nls.c src/bncsutil/oldauth.cpp src/bncsutil/pe.c src/bncsutil/sha1.c src/bncsutil/stack.c
HEADERS += src/bncsutil/bncsutil.h src/bncsutil/bsha1.h src/bncsutil/buffer.h src/bncsutil/cdkeydecoder.h src/bncsutil/checkrevision.h src/bncsutil/decodekey.h src/bncsutil/file.h src/bncsutil/gmp.h src/bncsutil/keytables.h src/bncsutil/libinfo.h src/bncsutil/ms_stdint.h src/bncsutil/mutil.h src/bncsutil/mutil_types.h src/bncsutil/nls.h src/bncsutil/oldauth.h src/bncsutil/pe.h src/bncsutil/sha1.h src/bncsutil/stack.h
FORMS +=
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Debug/MinGW-Windows
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += src 
LIBS += src/libgmp.a src/version.lib  
