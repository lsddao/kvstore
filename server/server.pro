CONFIG += console
QT += network
QT -= gui

HEADERS       = server.h \
                workerthread.h \
				../common/LocalKeyValueProvider.h
SOURCES       = server.cpp \
                workerthread.cpp \
                main.cpp \
				../common/LocalKeyValueProvider.cpp