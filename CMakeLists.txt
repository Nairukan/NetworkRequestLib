cmake_minimum_required(VERSION 3.14)

project(NetworkRequestLib LANGUAGES CXX)

if (NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message(STATUS "This project has a top-level one called [${CMAKE_PROJECT_NAME}]")
else()
    message(STATUS "This project is a top-level one")
endif()

add_compile_definitions(DEBUG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_VERSION 1.1)
#add_compile_definitions(DEBUG_STATUS)

set(CURL_USE_STATIC_LIBS ON)

find_package(CURL REQUIRED COMPONENTS HTTP HTTPS)



# Добавляем сборку статической библиотеки
add_library(${PROJECT_NAME}_static STATIC)

target_sources(${PROJECT_NAME}_static
    PRIVATE
        src/request.cpp
        include/request.h
)

target_include_directories(${PROJECT_NAME}_static
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CURL_INCLUDE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME}_static
    PUBLIC
        CURL::libcurl_static
    PRIVATE
        -static-libgcc -static-libstdc++
)



# Добавляем сборку динамической библиотеки
add_library(${PROJECT_NAME}_shared SHARED)

target_sources(${PROJECT_NAME}_shared
    PRIVATE
        src/request.cpp
        include/request.h
)

target_include_directories(${PROJECT_NAME}_shared
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CURL_INCLUDE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME}_shared
    PUBLIC
        CURL::libcurl_shared    
    #PRIVATE
        #-static-libgcc -static-libstdc++
)

#[[
get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
#]]

# Устанавливаем заголовочные файлы
set(public_headers
    include/request.h
    include/NetworkRequestLib_global.h
)

set_target_properties(${PROJECT_NAME}_static PROPERTIES PUBLIC_HEADER "${public_headers}")
set_target_properties(${PROJECT_NAME}_static PROPERTIES DEBUG_POSTFIX "d")

set_target_properties(${PROJECT_NAME}_shared PROPERTIES PUBLIC_HEADER "${public_headers}")
set_target_properties(${PROJECT_NAME}_shared PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET ${PROJECT_NAME}_static PROPERTY POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)

# Устанавливаем цели для установки
install(TARGETS ${PROJECT_NAME}_static ${PROJECT_NAME}_shared
    EXPORT ${PROJECT_NAME}Targets
    #RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
    #ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    #LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION cmake
)

if(WIN32)
    
    set(DLL_DIRS "")
    foreach(I_DIR ${PC_CURL_INCLUDE_DIRS})
        list(APPEND DLL_DIRS "${I_DIR}/../bin")
    endforeach()
    set(GENERATED_DLL "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${CMAKE_SHARED_MODULE_PREFIX}${PROJECT_NAME}_shared")
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set (GENERATED_DLL_DEBUG "${GENERATED_DLL}d")
    endif()
    set (GENERATED_DLL_DEBUG "${GENERATED_DLL_DEBUG}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set (GENERATED_DLL_RELEASE "${GENERATED_DLL}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    #list(APPEND DLL_DIRS  "")
    #message(STATUS "dlls -  ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")

    # Переменная для хранения всех .dll файлов
    set(ALL_DLL_FILES_DEBUG "")

    # Проходим по каждой директории из списка
    foreach(DLL_DIR ${DLL_DIRS})
        # Получаем список всех .dll файлов в текущей директории
        file(GLOB DLL_FILES "${DLL_DIR}/*.dll")

        # Добавляем найденные .dll файлы в общую переменную
        list(APPEND ALL_DLL_FILES_DEBUG ${DLL_FILES})
    endforeach()
    set(ALL_DLL_FILES_RELEASE ${ALL_DLL_FILES_DEBUG})
    list(APPEND ALL_DLL_FILES_DEBUG ${GENERATED_DLL_DEBUG})
    list(APPEND ALL_DLL_FILES_RELEASE ${GENERATED_DLL_RELEASE})
    #message(STATUS "DLLS -  ${ALL_DLL_FILES}")

endif()

include(CMakePackageConfigHelpers)

# Генерируем файл конфигурации
write_basic_package_version_file(
    ${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(Config.cmake.in
    ${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION cmake
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION cmake
)
