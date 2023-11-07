# Hi there

# Timings
 - Learn basic CMake + make basic ui and requests - 6h 40m
 - Basic scroll - 18m
 - Separate thread for requests - 56m
 - Address parsing(host and port) - 1h 35m

# Notes
 - Use OpenSSL for future https support. it is preinstalled on macOS. CMake changes:
    ```
        target_include_directories(executable PRIVATE /usr/local/include/)
        find_package(OpenSSL REQUIRED)
        target_link_libraries(executable ${OPENSSL_LIBRARIES})
    ```