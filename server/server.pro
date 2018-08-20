CONFIG += console
QT += network
QT -= gui

HEADERS       = server.h \
				workerthread.h \
				../common/LocalKeyValueProvider.h \
				../common/PersistentKeyValueStorage.h \
				../common/KeyValuePair.h \
				../common/KeyValuePairQString.h \
				utils.h

SOURCES       = server.cpp \
				workerthread.cpp \
				main.cpp \
				../common/LocalKeyValueProvider.cpp \
				../common/PersistentKeyValueStorage.cpp
