cmake_minimum_required(VERSION 3.5)

project(SIGameMP VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    gameeditorwindow.cpp
    gameeditorwindow.h
    gameeditorwindow.ui

    menu.cpp
    menu.h

    LobbyWindow.ui

    res.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SIGameMP
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        playerSetup.h
        playerSetup.cpp
        GameLogic.h
        GameLogic.cpp
        gameUI.h
        gameUI.cpp
        Player.h
        Player.cpp
        QuestionPackage.h
        QuestionPackage.cpp
        connectionWidget.h
        connectionWidget.cpp
        gameServer.h
        gameServer.cpp
        gameClient.h
        gameClient.cpp
        LobbyWindow.h LobbyWindow.cpp
    )
else()
    if(ANDROID)
        add_library(SIGameMP SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(SIGameMP
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(SIGameMP PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SIGameMP)
endif()

set_target_properties(SIGameMP PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SIGameMP
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SIGameMP)
endif()
