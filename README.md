# NetworkRequestLib
Lib covering libCurl
FOR INSTALL LIBARARY(NEED INSTALLED LibCURL)

    mkdir build && cd $_
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build . --target install
    rm -r ./* && cmake -DCMAKE_BUILD_TYPE=Debug ..
    cmake --build . --target install

FOR LINK TO PROJECT

    find_package(NetworkRequestLib CONFIG REQUIRED)
    include_directories(${NetworkRequestLib_INCLUDE_DIR})
    ...
    target_link_libraries(${PROJECT_NAME} ... ::NetworkRequestLib)
    #include <NetworkRequestLib/request.h>
    Use and enjoy
