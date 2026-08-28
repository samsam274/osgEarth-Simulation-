cmake_minimum_required(VERSION 3.13)
project(UAVViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SIMDIS SDK Temel Yollari
set(SIMDIS_SDK_DIR "C:/Users/samet/Downloads/SIMDIS_SDK_3rd_win64_vc-14.3/SIMDIS_SDK/3rd/win64_vc-14.3")

# Kaynak Dosyalari
add_executable(UAVViewer
    main.cpp
    RouteManager.cpp
    GuiPanel.cpp
    RouteManager.h
    GuiPanel.h
)


target_include_directories(UAVViewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${SIMDIS_SDK_DIR}/OpenSceneGraph/3.6.5/include"
    "${SIMDIS_SDK_DIR}/osgEarth/OSG-3.6.5/include"
    "${SIMDIS_SDK_DIR}/osgEarth/OSG-3.6.5/include/osgEarthImGui"
)

# Library Dizinleri
target_link_directories(UAVViewer PRIVATE
    "${SIMDIS_SDK_DIR}/OpenSceneGraph/3.6.5/lib"
    "${SIMDIS_SDK_DIR}/osgEarth/OSG-3.6.5/lib"
)

# Baglanacak Kutuphaneler
target_link_libraries(UAVViewer PRIVATE
    osg
    osgDB
    osgGA
    osgViewer
    osgText
    osgUtil
    OpenThreads
    osgEarth
    osgEarthImGui
    opengl32
)

# MSVC'ye ozel ayarlar
if(MSVC)
    # Turkce karakterli yorum/string'lerin bozulmamasi icin
    target_compile_options(UAVViewer PRIVATE /utf-8)

    # windows.h'in min/max makrolarini tanimlamasini engelle
    target_compile_definitions(UAVViewer PRIVATE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
        USE_SHADOW_CASTER
    )

 
    set_target_properties(UAVViewer PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        VS_DEBUGGER_ENVIRONMENT "PATH=${SIMDIS_SDK_DIR}/OpenSceneGraph/3.6.5/bin;${SIMDIS_SDK_DIR}/osgEarth/OSG-3.6.5/bin;%PATH%"
    )
endif()
