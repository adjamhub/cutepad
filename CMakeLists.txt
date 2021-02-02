# CMake things ------------------------------------------------------------
cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(FeatureSummary)


# Project things ----------------------------------------------------------
project(cutepad VERSION "0.0.6" LANGUAGES CXX)

configure_file(config.h.in config.h)
include_directories(${CMAKE_CURRENT_BINARY_BIN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Qt things ---------------------------------------------------------------
set(QT_MINIMUM_VERSION "5.14.0")

find_package(Qt5 "${QT_MINIMUM_VERSION}" COMPONENTS REQUIRED
    Core
    Gui
    Widgets
    PrintSupport
    DBus
)


# KF5 things --------------------------------------------------------------
set(KF5_MINIMUM_VERSION "5.68.0")

find_package(KF5SyntaxHighlighting "${KF5_MINIMUM_VERSION}" REQUIRED)


# Compile && Link ---------------------------------------------------------
add_executable(cutepad
    src/main.cpp
    src/application.cpp
    src/cutepadadaptor.cpp
    src/mainwindow.cpp
    src/replacebar.cpp
    src/searchbar.cpp
    src/settingsdialog.cpp
    src/statusbar.cpp
    src/textedit.cpp
    resources.qrc
)

target_link_libraries(cutepad PRIVATE 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::PrintSupport
    Qt5::DBus
    # ----------------------
    KF5::SyntaxHighlighting
)


# INSTALL ----------------------------------------------------------------
install(TARGETS cutepad RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

install(FILES data/cutepad.desktop     DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")
install(FILES data/cutepad.appdata.xml DESTINATION "${CMAKE_INSTALL_PREFIX}/share/metainfo")
install(FILES LICENSE                  DESTINATION "${CMAKE_INSTALL_PREFIX}/share/adjam/cutepad")
install(FILES data/MANUAL              DESTINATION "${CMAKE_INSTALL_PREFIX}/share/adjam/cutepad")

install(DIRECTORY icons DESTINATION "${CMAKE_INSTALL_PREFIX}/share/adjam/cutepad")

# --------------------------------------------------------------------------
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
