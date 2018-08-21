QT += network widgets

win32-msvc*{
	QMAKE_CXXFLAGS += /MP
	QMAKE_CXXFLAGS_WARN_ON = /W4
}

HEADERS       = client.h \
				../common/NetworkKeyValueProvider.h \
				../common/KeyValueProvider.h
SOURCES       = client.cpp \
                main.cpp \
				../common/NetworkKeyValueProvider.cpp