cmake_minimum_required(VERSION 3.16)

set(CUTEPAD_VERSION "0.0.3")
project(cutepad VERSION ${CUTEPAD_VERSION} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt things ---------------------------------------------------------------
set(QT_MINIMUM_VERSION "5.15.0")

find_package(Qt5 "${QT_MINIMUM_VERSION}" COMPONENTS REQUIRED
                Core
                Gui
                Widgets
                PrintSupport
)

# KF5 things --------------------------------------------------------------
set(KF5_MINIMUM_VERSION "5.68.0")

find_package(ECM "${KF5_MINIMUM_VERSION}" CONFIG REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(ECMSetupVersion)
include(ECMAddAppIcon)
include(FeatureSummary)

## Generate header with version number
ecm_setup_version(${CUTEPAD_VERSION}
                  VARIABLE_PREFIX CUTEPAD
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/version.h"
)

find_package(KF5SyntaxHighlighting "${KF5_MINIMUM_VERSION}")


# -------------------------------------------------------------------------
add_executable(cutepad
    src/main.cpp
    src/application.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainview.cpp
    src/textedit.cpp
    src/searchbar.cpp
    src/replacebar.cpp
)

target_link_libraries(cutepad PRIVATE 
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        # ----------------------
        KF5::SyntaxHighlighting
)

# -------------------------------------------------------------------------
install(TARGETS cutepad RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
install(FILES cutepad.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")

# --------------------------------------------------------------------------
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
