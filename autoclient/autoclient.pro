CONFIG += console
QT = core network testlib

win32-msvc*{
	QMAKE_CXXFLAGS += /MP
	QMAKE_CXXFLAGS_WARN_ON = /W4
}

HEADERS       = autoclient.h \
				../common/NetworkKeyValueProvider.h \
				../common/LocalKeyValueProvider.h \
				../common/KeyValueProvider.h
SOURCES       = autoclient.cpp \
				../common/NetworkKeyValueProvider.cpp \
				../common/LocalKeyValueProvider.cpp