### General config ###

TARGET          = MPMBase
VERSION         = 0.0.0
TEMPLATE        = app
#ICON            = resources/icon.icns
QT              += sql widgets printsupport
CONFIG          += c++11
DEFINES         += APP_VERSION=\\\"$$VERSION\\\"
QMAKE_CXXFLAGS   = -Wall \#-Weverything \
                   -Wno-c++98-compat \
                   -Wno-padded  \
                   -Wno-undefined-reinterpret-cast  \
                   -Wno-pragmas  \
                   -Wno-unknown-warning-option


### Info.plist ###

QMAKE_INFO_PLIST    =  res/Info.plist
OTHER_FILES         += res/Info.plist \
    res/updater/sparkle_appcast.template.xml \
    res/updater/sparkle_dsa_priv.pem \
    res/updater/sparkle_dsa_pub.pem

### MPMBaseModel ###

INCLUDEPATH     += ../lib/MPMBaseModel/include ../lib/ContactsModel/include ../lib/ContactsModel
LIBS            += -L../lib/MPMBaseModel -lMPMBaseModel
POST_TARGETDEPS += ../lib/MPMBaseModel/libMPMBaseModel.a


### QPersistence ###

QPERSISTENCE_PATH = ../lib/QPersistence
include($$QPERSISTENCE_PATH/QPersistence.pri)

INCLUDEPATH     += $$QPERSISTENCE_INCLUDEPATH
LIBS            += $$QPERSISTENCE_LIBS
POST_TARGETDEPS += $$QPERSISTENCE_POST_TARGETDEPS


### libGUI ###

INCLUDEPATH     += ../lib/libGUI/include
LIBS            += -L../lib/libGUI/src -lGUI
POST_TARGETDEPS += ../lib/libGUI/src/libGUI.a


### Documents ###

INCLUDEPATH     += ../lib/libDocuments/src
LIBS            += -L../lib/libDocuments/src -lDocuments
POST_TARGETDEPS += ../lib/libDocuments/src/libDocuments.a


### MPMDocuments ###

INCLUDEPATH     += ../lib/libMPMDocuments/include
LIBS            += -L../lib/libMPMDocuments/src -lMPMDocuments
POST_TARGETDEPS += ../lib/libMPMDocuments/src/libMPMDocuments.a


### libUpdate ###

LIBUPDATE_PATH = ../lib/libUpdate
include($$LIBUPDATE_PATH/libUpdate.pri)

INCLUDEPATH         += $$LIBUPDATE_INCLUDEPATH
LIBS                += $$LIBUPDATE_LIBS
PRE_TARGETDEPS      += $$LIBUPDATE_PRE_TARGETDEPS
QMAKE_BUNDLE_DATA   += $$LIBUPDATE_BUNDLE_DATA


### QtAwesome ###

include(../lib/QtAwesome/QtAwesome/QtAwesome.pri)




### Deployment ###

## Target: app ##

TARGET_APP.target    = app
TARGET_APP.commands  = $$[QT_INSTALL_BINS]/macdeployqt $${OUT_PWD}/$${TARGET}.app
TARGET_APP.commands  += ;/usr/libexec/PlistBuddy -c \"Set :CFBundleVersion $${VERSION}\" $${OUT_PWD}/$${TARGET}.app/Contents/Info.plist
TARGET_APP.commands  += ;rm -R $${OUT_PWD}/$${TARGET}.app/Contents/PlugIns/accessible
TARGET_APP.commands  += ;rm $${OUT_PWD}/$${TARGET}.app/Contents/PlugIns/imageformats/libqmng.dylib
TARGET_APP.commands  += ;rm $${OUT_PWD}/$${TARGET}.app/Contents/PlugIns/imageformats/libqtga.dylib
TARGET_APP.commands  += ;rm $${OUT_PWD}/$${TARGET}.app/Contents/PlugIns/imageformats/libqtiff.dylib
TARGET_APP.commands  += ;rm $${OUT_PWD}/$${TARGET}.app/Contents/PlugIns/imageformats/libqwbmp.dylib
TARGET_APP.commands  += ;rm $${OUT_PWD}/$${TARGET}.app/Contents/PlugIns/sqldrivers/libqsqlite.dylib
TARGET_APP.commands  += ;rm $${OUT_PWD}/$${TARGET}.app/Contents/PlugIns/sqldrivers/libqsqlodbc.dylib
TARGET_APP.commands  += ;rm $${OUT_PWD}/$${TARGET}.app/Contents/PlugIns/sqldrivers/libqsqlpsql.dylib
QMAKE_EXTRA_TARGETS += TARGET_APP

## Target: zip ##

ZIPFILE = \"$${OUT_PWD}/$${TARGET}-$${VERSION}.zip\"
TARGET_ZIP.depends   = app
TARGET_ZIP.target    = zip
TARGET_ZIP.commands  = cd $${OUT_PWD}
TARGET_ZIP.commands += ;if [ -e $$ZIPFILE ] ; then rm $$ZIPFILE; fi
TARGET_ZIP.commands += ;/usr/bin/zip -q -y $$ZIPFILE -r $${TARGET}.app
TARGET_ZIP.commands += ;cd -
QMAKE_EXTRA_TARGETS += TARGET_ZIP

## Target: appcast ##

SPARKLE_PRIVATE_KEY         = $$PWD/res/updater/sparkle_dsa_priv.pem
SPARKLE_PUBLIC_KEY          = $$PWD/res/updater/sparkle_dsa_pub.pem
SPARKLE_APPCAST_TEMPLATE    = $$PWD/res/updater/sparkle_appcast.template.xml
SPARKLE_UPLOAD_URL          = \"http://someUrl.de\"
DEFINES += SPARKLE_UPLOAD_URL=\\\"$$SPARKLE_UPLOAD_URL\\\"

SPARKLE_SIGNATURE.files = $$SPARKLE_PUBLIC_KEY
SPARKLE_SIGNATURE.path  = Contents/Resources
QMAKE_BUNDLE_DATA += SPARKLE_SIGNATURE

TARGET_APPCAST.depends   = zip
TARGET_APPCAST.target    = appcast
TARGET_APPCAST.commands += $$SPARKLE_CREATE_APPCAST     # creating script
TARGET_APPCAST.commands += $$ZIPFILE                    # input zipfile
TARGET_APPCAST.commands += $$VERSION                    # version
TARGET_APPCAST.commands += \"Version $$VERSION\"        # version string
TARGET_APPCAST.commands += $$SPARKLE_PRIVATE_KEY        # sparkle private key for signatures
TARGET_APPCAST.commands += $$SPARKLE_UPLOAD_URL         # The URL. Upload the Zipfile, sparkle_appcast.xml and releasenotes_mac.html to this folder!
TARGET_APPCAST.commands += $$SPARKLE_APPCAST_TEMPLATE   # Appcast template
TARGET_APPCAST.commands += > $$OUT_PWD/sparkle_appcast.xml # Output appcast
QMAKE_EXTRA_TARGETS += TARGET_APPCAST

## Target: deploy ##

TARGET_DEPLOY.depends   = appcast
TARGET_DEPLOY.target    = deploy
QMAKE_EXTRA_TARGETS += TARGET_DEPLOY


### Files ###

SOURCES += \
    main.cpp \
    ui/contacts/artistsPage.cpp \
    ui/contacts/clientsPage.cpp \
    ui/contacts/contact/contactWidget.cpp \
    ui/contacts/contact/details/categoryComboBox.cpp \
    ui/contacts/contact/details/contactDetailsWidget.cpp \
    ui/contacts/contact/details/contactInfo/addressWidget.cpp \
    ui/contacts/contact/details/contactInfo/contactInfoCategoryWidget.cpp \
    ui/contacts/contact/details/contactInfo/contactInfosWidget.cpp \
    ui/contacts/contact/details/contactInfo/contactInfoWidget.cpp \
    ui/contacts/contact/details/contactInfo/contactPersonsWidget.cpp \
    ui/contacts/contact/details/contactInfo/contactPersonWidget.cpp \
    ui/contacts/contact/details/contactInfo/emailWidget.cpp \
    ui/contacts/contact/details/contactInfo/facebookAccountWidget.cpp \
    ui/contacts/contact/details/contactInfo/faxNumberWidget.cpp \
    ui/contacts/contact/details/contactInfo/phonenumberwidget.cpp \
    ui/contacts/contact/details/contactInfo/skypeAccountWidget.cpp \
    ui/contacts/contact/details/contactInfo/websiteWidget.cpp \
    ui/contacts/contact/details/contractWidget.cpp \
    ui/contacts/contact/events/callEditor.cpp \
    ui/contacts/contact/events/contactEventsModel.cpp \
    ui/contacts/contact/events/contactEventsWidget.cpp \
    ui/contacts/contact/events/contactInfoButton.cpp \
    ui/contacts/contact/events/emailEditor.cpp \
    ui/contacts/contact/events/eventDelegate.cpp \
    ui/contacts/contact/events/eventEditor.cpp \
    ui/contacts/contact/events/eventList.cpp \
    ui/contacts/contact/events/noteEditor.cpp \
    ui/contacts/contactDelegate.cpp \
    ui/contacts/contactsModel.cpp \
    ui/contacts/contactsPage.cpp \
    ui/contacts/placesPage.cpp \
    ui/contacts/staffPage.cpp \
    ui/dialogs/createStaffDialog.cpp \
    ui/dialogs/loginDialog.cpp \
    ui/dialogs/preferences/countriesWidget.cpp \
    ui/dialogs/preferences/preferencesWindow.cpp \
    ui/mainWindow.cpp \
    ui/menuBar.cpp \
    ui/widgets/actionButton.cpp \
    ui/widgets/contactLabel.cpp \
    ui/widgets/contactsComboBox.cpp \
    ui/offers/offersPage.cpp \
    ui/offers/offersModel.cpp \
    ui/offers/creation/offerCreationWidget.cpp \
    ui/offers/creation/dateCreationWidget.cpp \
    ui/offers/options/optionWidget.cpp \
    ui/offers/offerDelegate.cpp \
    ui/widgets/contactCompleter.cpp \
    ui/offers/offerWidget.cpp \
    ui/widgets/contactInfoCompleter.cpp \
    ui/qtawesomesidebariconpainter.cpp \
    ui/widgets/contactLineEdit.cpp \
    ui/widgets/contactValidator.cpp \
    ui/calendar/calendarPage.cpp \
    ui/calendar/eventsCalendar.cpp \
    ui/calendar/dayOverviewWidget.cpp \
    ui/calendar/calendarModel.cpp \
    ui/calendar/dateDelegate.cpp \
    ui/calendar/bookingWidget.cpp \
    ui/contract/contractEditorWidget.cpp \
    ui/contract/contractEditorWindow.cpp \
    ui/contacts/contact/details/contactInfo/contactInfoLabelComboBox.cpp \
    ui/icons.cpp

RESOURCES += \
    res/images/common.qrc \
    res/images/contactswidget.qrc

FORMS += \
    ui/contacts/contact/contactWidget.ui \
    ui/contacts/contact/details/contactDetailsWidget.ui \
    ui/contacts/contact/details/contactInfo/addressWidget.ui \
    ui/contacts/contact/details/contactInfo/contactInfosWidget.ui \
    ui/contacts/contact/details/contactInfo/contactPersonsWidget.ui \
    ui/contacts/contact/details/contactInfo/contactPersonWidget.ui \
    ui/contacts/contact/details/contactInfo/emailWidget.ui \
    ui/contacts/contact/details/contactInfo/facebookAccountWidget.ui \
    ui/contacts/contact/details/contactInfo/faxNumberWidget.ui \
    ui/contacts/contact/details/contactInfo/phonenumberwidget.ui \
    ui/contacts/contact/details/contactInfo/skypeAccountWidget.ui \
    ui/contacts/contact/details/contactInfo/websiteWidget.ui \
    ui/contacts/contact/details/contractWidget.ui \
    ui/contacts/contact/events/callEditor.ui \
    ui/contacts/contact/events/contactEventsWidget.ui \
    ui/contacts/contact/events/emailEditor.ui \
    ui/contacts/contact/events/noteEditor.ui \
    ui/contacts/contactsPage.ui \
    ui/dialogs/createEventDialog.ui \
    ui/dialogs/createStaffDialog.ui \
    ui/dialogs/loginDialog.ui \
    ui/dialogs/preferences/countriesWidget.ui \
    ui/dialogs/preferences/preferencesWindow.ui \
    ui/mainWindow.ui \
    ui/offers/offersPage.ui \
    ui/offers/creation/dateCreationWidget.ui \
    ui/offers/options/optionWidget.ui \
    ui/offers/creation/offerCreationWidget.ui \
    ui/offers/offerWidget.ui \
    ui/calendar/calendarPage.ui \
    ui/calendar/dayOverviewWidget.ui \
    ui/calendar/bookingWidget.ui \
    ui/contract/contractEditorWidget.ui \
    ui/contract/contractEditorWindow.ui

HEADERS += \
    ui/contacts/artistsPage.h \
    ui/contacts/clientsPage.h \
    ui/contacts/contact/contactWidget.h \
    ui/contacts/contact/details/categoryComboBox.h \
    ui/contacts/contact/details/contactDetailsWidget.h \
    ui/contacts/contact/details/contactInfo/addressWidget.h \
    ui/contacts/contact/details/contactInfo/contactInfoCategoryWidget.h \
    ui/contacts/contact/details/contactInfo/contactInfosWidget.h \
    ui/contacts/contact/details/contactInfo/contactInfoWidget.h \
    ui/contacts/contact/details/contactInfo/contactPersonsWidget.h \
    ui/contacts/contact/details/contactInfo/contactPersonWidget.h \
    ui/contacts/contact/details/contactInfo/emailWidget.h \
    ui/contacts/contact/details/contactInfo/facebookAccountWidget.h \
    ui/contacts/contact/details/contactInfo/faxNumberWidget.h \
    ui/contacts/contact/details/contactInfo/phonenumberwidget.h \
    ui/contacts/contact/details/contactInfo/skypeAccountWidget.h \
    ui/contacts/contact/details/contactInfo/websiteWidget.h \
    ui/contacts/contact/details/contractWidget.h \
    ui/contacts/contact/events/callEditor.h \
    ui/contacts/contact/events/contactEventsModel.h \
    ui/contacts/contact/events/contactEventsWidget.h \
    ui/contacts/contact/events/contactInfoButton.h \
    ui/contacts/contact/events/emailEditor.h \
    ui/contacts/contact/events/eventDelegate.h \
    ui/contacts/contact/events/eventEditor.h \
    ui/contacts/contact/events/eventList.h \
    ui/contacts/contact/events/noteEditor.h \
    ui/contacts/contactDelegate.h \
    ui/contacts/contactsModel.h \
    ui/contacts/contactsPage.h \
    ui/contacts/placesPage.h \
    ui/contacts/staffPage.h \
    ui/dialogs/createStaffDialog.h \
    ui/dialogs/loginDialog.h \
    ui/dialogs/preferences/countriesWidget.h \
    ui/dialogs/preferences/preferencesWindow.h \
    ui/mainWindow.h \
    ui/menuBar.h \
    ui/widgets/actionButton.h \
    ui/widgets/contactLabel.h \
    ui/widgets/contactsComboBox.h \
    ui/offers/offersPage.h \
    ui/offers/offersModel.h \
    ui/offers/creation/offerCreationWidget.h \
    ui/offers/creation/dateCreationWidget.h \
    ui/offers/options/optionWidget.h \
    ui/offers/offerDelegate.h \
    ui/widgets/contactCompleter.h \
    ui/offers/offerWidget.h \
    ui/widgets/contactInfoCompleter.h \
    ui/qtawesomesidebariconpainter.h \
    ui/widgets/contactLineEdit.h \
    ui/widgets/contactValidator.h \
    ui/calendar/calendarPage.h \
    ui/calendar/eventsCalendar.h \
    ui/calendar/dayOverviewWidget.h \
    ui/calendar/calendarModel.h \
    ui/calendar/dateDelegate.h \
    ui/calendar/bookingWidget.h \
    ui/contract/contractEditorWidget.h \
    ui/contract/contractEditorWindow.h \
    ui/contacts/contact/details/contactInfo/contactInfoLabelComboBox.h \
    ui/icons.h
