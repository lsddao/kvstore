CONFIG += console
QT = core network

win32-msvc*{
	QMAKE_CXXFLAGS += /W4 /MP
}

HEADERS       = server.h \
				workerthread.h \
				../common/LocalKeyValueProvider.h \
				../common/PersistentKeyValueStorage.h \
				../common/KeyValuePair.h \
				../common/KeyValuePairQString.h

SOURCES       = server.cpp \
				workerthread.cpp \
				main.cpp \
				../common/LocalKeyValueProvider.cpp \
				../common/PersistentKeyValueStorage.cpp
