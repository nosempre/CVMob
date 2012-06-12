project(CVMob)

find_package(Qt4 4.8 REQUIRED)
include(${QT_USE_FILE})

find_package(OpenCV 2 REQUIRED)

set(CVMob_sources
    main.cpp
    view/aboutdialog.cpp
    view/cvmobmainwindow.cpp
    view/videoview.cpp
    model/cvmobvideomodel.cpp
    model/distancesproxymodel.cpp
    view/playbar.cpp
)

set(CVMob_headers
    view/aboutdialog.h
    view/cvmobmainwindow.h
    view/videoview.h
    model/cvmobvideomodel.hpp
    model/distancesproxymodel.hpp
    view/playbar.hpp
)

set(CVMob_forms
    view/aboutdialog.ui
    view/cvmobmainwindow.ui
    view/playbar.ui
)

set(CVMob_translations
    pt_BR_cvmob.ts
)

set(CVMob_resources cvmob_resource.qrc)

QT4_WRAP_CPP(CVMob_headers_moc ${CVMob_headers})
QT4_WRAP_UI(CVMob_forms_headers ${CVMob_forms})
QT4_ADD_RESOURCES(CVMob_resources_rcc ${CVMob_resources})
QT4_CREATE_TRANSLATION(CVMob_translations_qm ${CVMob_translations})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(cvmob
    ${CVMob_sources}
    ${CVMob_headers_moc}
    ${CVMob_forms_headers}
    ${CVMob_resources_rcc}
    ${CVMob_translations_qm}
)

target_link_libraries(cvmob ${QT_LIBRARIES} ${OpenCV_LIBS})