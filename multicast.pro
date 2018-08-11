TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    net_utils.c \
    server.c \
    handlers.c \
    servers_container.c

HEADERS += \
    net_utils.h \
    server.h \
    handlers.h \
    servers_container.h
