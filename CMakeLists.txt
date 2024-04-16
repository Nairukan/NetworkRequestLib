cmake_minimum_required(VERSION 3.14)

project(NetworkRequestLib LANGUAGES CXX)

if (NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message(STATUS "This project has a top-level one called [${CMAKE_PROJECT_NAME}]")
else()
    message(STATUS "This project is a top-level one")
endif()

#add_compile_definitions(DEBUG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_VERSION 1.1)

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
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME}_static
    PRIVATE
        CURL::libcurl_static
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
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME}_shared
    PRIVATE
        CURL::libcurl_shared
        -static-libgcc -static-libstdc++
)

# Устанавливаем заголовочные файлы
set(public_headers
    include/request.h
    include/NetworkRequestLib_global.h
)

set_target_properties(${PROJECT_NAME}_static PROPERTIES PUBLIC_HEADER "${public_headers}")

set_target_properties(${PROJECT_NAME}_shared PROPERTIES PUBLIC_HEADER "${public_headers}")

set_property(TARGET ${PROJECT_NAME}_static PROPERTY POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)

# Устанавливаем цели для установки
install(TARGETS ${PROJECT_NAME}_static ${PROJECT_NAME}_shared
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION cmake
)

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
