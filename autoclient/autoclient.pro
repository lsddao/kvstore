CONFIG += console
QT += network testlib

HEADERS       = autoclient.h \
				../common/NetworkKeyValueProvider.h \
				../common/LocalKeyValueProvider.h \
				../common/KeyValueProvider.h
SOURCES       = autoclient.cpp \
				../common/NetworkKeyValueProvider.cpp \
				../common/LocalKeyValueProvider.cpp