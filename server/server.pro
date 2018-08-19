QT += core network
CONFIG += console

HEADERS       = server.h \
                workerthread.h \
				LocalKeyValueProvider.h
SOURCES       = server.cpp \
                workerthread.cpp \
                main.cpp \
				LocalKeyValueProvider.cpp