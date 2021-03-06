project(CVMob)

cmake_minimum_required(VERSION 2.6)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(OpenCV 2 REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} -std=c++11")

set(CVMob_sources
    main.cpp
    view/aboutdialog.cpp
    view/cvmobmainwindow.cpp
    view/videographicsview.cpp
    view/videoview.cpp
    model/videomodel.cpp
    model/itemmodels/klinkitemselectionmodel.cpp
    model/itemmodels/kmodelindexproxymapper.cpp
    model/proxies/anglesproxymodel.cpp
    model/proxies/baseproxymodel.cpp
    model/proxies/distancesproxymodel.cpp
    model/proxies/instantsproxymodel.cpp
    model/proxies/trajectoriesproxymodel.cpp
    model/proxies/videolistproxymodel.cpp
    model/jobs/anglecalcjob.cpp
    model/jobs/basejob.cpp
    model/jobs/trajectorycalcjob.cpp
    view/playbar.cpp
    view/playslider.cpp
    view/videostatus.cpp
    view/graphicsitems/angleitem.cpp
    view/graphicsitems/distanceitem.cpp
    view/graphicsitems/trajectoryinstantitem.cpp
    view/graphicsitems/trajectoryitem.cpp
)

set(CVMob_headers
    view/aboutdialog.hpp
    view/cvmobmainwindow.hpp
    view/videographicsview.hpp
    view/videoview.hpp
    model/videomodel.hpp
    model/proxies/anglesproxymodel.hpp
    model/proxies/baseproxymodel.hpp
    model/proxies/distancesproxymodel.hpp
    model/proxies/instantsproxymodel.hpp
    model/proxies/trajectoriesproxymodel.hpp
    model/proxies/videolistproxymodel.hpp
    model/jobs/anglecalcjob.hpp
    model/jobs/basejob.hpp
    model/jobs/trajectorycalcjob.hpp
    view/playbar.hpp
    view/playslider.hpp
    view/videostatus.hpp
    view/graphicsitems/angleitem.hpp
    view/graphicsitems/distanceitem.hpp
    view/graphicsitems/trajectoryinstantitem.hpp
    view/graphicsitems/trajectoryitem.hpp
)

set(kde_itemmodels_headers
    model/itemmodels/klinkitemselectionmodel.h
    model/itemmodels/kmodelindexproxymapper.h
)

set(CVMob_forms
    view/aboutdialog.ui
    view/cvmobmainwindow.ui
    view/playbar.ui
    view/videostatus.ui
)

set(CVMob_translations
    pt_BR_cvmob.ts
)

set(CVMob_resources cvmob_resource.qrc)

QT5_WRAP_CPP(CVMob_headers_moc ${CVMob_headers})
QT5_WRAP_CPP(kde_itemmodels_headers_moc ${kde_itemmodels_headers})
QT5_WRAP_UI(CVMob_forms_headers ${CVMob_forms})
QT5_ADD_RESOURCES(CVMob_resources_rcc ${CVMob_resources})
if (CVMOB_UPDATE_TRANSLATIONS)
  message("WARNING: make clean WILL WIPE OUT THE .ts TRANSLATION FILES IF EXECUTED")
  QT5_CREATE_TRANSLATION(CVMob_translations_qm ${CMAKE_CURRENT_SOURCE_DIR} ${CVMob_translations})
else ()
  QT5_ADD_TRANSLATION(CVMob_translations_qm ${CVMob_translations})
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(cvmob
    ${CVMob_sources}
    ${CVMob_headers_moc}
    ${CVMob_forms_headers}
    ${CVMob_resources_rcc}
    ${CVMob_translations_qm}
)

target_link_libraries(cvmob ${Qt5Widgets_LIBRARIES} ${OpenCV_LIBS})
