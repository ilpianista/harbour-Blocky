TARGET = harbour-blocky

CONFIG += sailfishapp

SOURCES += \
    src/blockymanager.cpp \
    src/main.cpp

OTHER_FILES += \
    qml/cover/CoverPage.qml \
    qml/pages/MainPage.qml \
    qml/Blocky.qml \
    harbour-blocky.desktop \
    rpm/harbour-blocky.changes \
    rpm/harbour-blocky.spec \
    rpm/blocky.service \
    rpm/blocky.yaml \
    rpm/connman.override.conf \
    translations/*.ts

HEADERS += \
    src/blockymanager.h

CONFIG += sailfishapp_i18n

TRANSLATIONS += \
    translations/harbour-blocky-it.ts
