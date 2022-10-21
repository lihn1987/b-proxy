cmake_minimum_required (VERSION 3.0)
project(server)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0  -DNO_QDEBUG")


aux_source_directory(./ main_src)
aux_source_directory(../net_lib net_lib)
aux_source_directory(../log/ log)
aux_source_directory(../crypto_tools crypto_tools)
aux_source_directory(../crypto_tools/aes aes)
aux_source_directory(../crypto_tools/blake2 blake)
aux_source_directory(../crypto_tools/secp256k1 secp256)
aux_source_directory(../asio_manager asio_manager)

aux_source_directory(../server/test test)

remove(aes ../crypto_tools/aes/ctaes.c)
remove(secp256 ../crypto_tools/secp256k1/gen_context.c)

add_executable(
    server 
    ${main_src} 
    ${net_lib}
    ${log}
    ${crypto_tools}
    ${asio_manager}
    ${aes}
    ${blake}
    ${secp256}
)

# add_executable(
#     test_app
#     ${test} 
#     ${net_lib}
#     ${log}
#     ${crypto_tools}
#     ${asio_manager}
#     ${aes}
#     ${blake}
#     ${secp256}
# )


target_link_directories(
    server PRIVATE 
    /usr/local/lib
)

target_link_libraries(
    server PRIVATE  
    libboost_log.a
    libboost_log_setup.a
    libboost_thread.a
    libboost_filesystem.a
    pthread) 

# target_link_directories(
#     test_app PRIVATE 
#     /usr/local/lib
# )

# target_link_libraries(
#     test_app PRIVATE  
#     libboost_log.a
#     libboost_log_setup.a
#     libboost_thread.a
#     libboost_filesystem.a
#     pthread) 