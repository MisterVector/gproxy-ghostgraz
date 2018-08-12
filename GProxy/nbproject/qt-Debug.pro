# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Debug/MinGW-Windows
TARGET = GProxy
VERSION = 2.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += debug 
PKGCONFIG +=
QT = core gui network
SOURCES += source/Config.cpp source/ConfigGUI.cpp source/GameListDownloader.cpp source/GhostGrazLogininformationDialog.cpp source/MainGUI.cpp source/Statspage.cpp source/bncsutilinterface.cpp source/bnet.cpp source/bnetprotocol.cpp source/commandpacket.cpp source/data/ColoredMessage.cpp source/data/Friend.cpp source/data/GameListEntry.cpp source/data/Player.cpp source/data/Product.cpp source/data/Slot.cpp source/delegate/ChannellistDelegate.cpp source/delegate/GamelistDelegate.cpp source/gameprotocol.cpp source/gproxy.cpp source/gpsprotocol.cpp source/socket.cpp source/thread/GproxyUpdateThread.cpp source/thread/SleepThread.cpp source/util/ColorUtil.cpp source/util/Util.cpp source/widget/ClickableLineEdit.cpp source/widget/ClickableListWidget.cpp source/widget/MButton.cpp source/widget/MColorDialog.cpp source/widget/MTextEdit.cpp
HEADERS += source/Config.h source/ConfigGUI.h source/GameListDownloader.h source/GhostGrazLogininformationDialog.h source/MainGUI.h source/Statspage.h source/bncsutilinterface.h source/bnet.h source/bnetprotocol.h source/commandpacket.h source/data/ColoredMessage.h source/data/Friend.h source/data/GameListEntry.h source/data/Player.h source/data/Product.h source/data/Slot.h source/delegate/ChannellistDelegate.h source/delegate/GamelistDelegate.h source/gameprotocol.h source/gproxy.h source/gpsprotocol.h source/socket.h source/thread/GproxyUpdateThread.h source/thread/SleepThread.h source/util/ColorUtil.h source/util/Util.h source/util/VariantUtil.h source/widget/ClickableLineEdit.h source/widget/ClickableListWidget.h source/widget/MButton.h source/widget/MColorDialog.h source/widget/MTextEdit.h
FORMS += source/ConfigGUI.ui source/GhostGrazLogininformationDialog.ui source/MainGUI.ui
RESOURCES += source/resources.qrc
TRANSLATIONS +=
OBJECTS_DIR = build/Debug/MinGW-Windows
MOC_DIR = source
RCC_DIR = source
UI_DIR = source
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += source . ../BNCSUtil/src 
LIBS += source/ms_sdk/version.lib source/ms_sdk/ws2_32.lib -L../BNCSUtil/dist/Release/MinGW_1-Windows ../BNCSUtil/dist/Release/MinGW_1-Windows/BNCSUtil.dll  
