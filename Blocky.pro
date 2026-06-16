TARGET = harbour-blocky

CONFIG += sailfishapp

QT += network

SOURCES += \
    src/blockyclient.cpp \
    src/blockymanager.cpp \
    src/main.cpp

OTHER_FILES += \
    qml/cover/CoverPage.qml \
    qml/pages/MainPage.qml \
    qml/pages/SettingsPage.qml \
    qml/Blocky.qml \
    harbour-blocky.desktop \
    rpm/harbour-blocky.changes \
    rpm/harbour-blocky.spec \
    rpm/blocky.service \
    rpm/blocky.yaml \
    rpm/connman.override.conf \
    translations/*.ts

HEADERS += \
    src/blockyclient.h \
    src/blockymanager.h

CONFIG += sailfishapp_i18n

TRANSLATIONS += \
    translations/harbour-blocky-it.ts \
    translations/harbour-blocky-sv.ts
