CONFIG += console
QT = core network

win32-msvc*{
	QMAKE_CXXFLAGS += /MP
	QMAKE_CXXFLAGS_WARN_ON = /W4
}

HEADERS       = server.h \
				workerthread.h \
				../common/LocalKeyValueProvider.h \
				../common/PersistentKeyValueStorage.h \
				utils.h

SOURCES       = server.cpp \
				workerthread.cpp \
				main.cpp \
				../common/LocalKeyValueProvider.cpp \
				../common/PersistentKeyValueStorage.cpp