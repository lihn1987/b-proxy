QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../asio_manager/asio_manager.cpp \
    ../crypto_tools/aes/aes.cpp \
    ../crypto_tools/aes/ctaes.c \
    ../crypto_tools/blake2/blake2b.cc \
    ../crypto_tools/key_tools.cpp \
    ../crypto_tools/secp256k1/bench_ecdh.c \
    ../crypto_tools/secp256k1/secp256k1.c \
    ../log/log.cpp \
    ../net_lib/client_server.cpp \
    ../net_lib/client_socket_item.cpp \
    config.cpp \
    main.cpp \
    proxy.cpp \
    widget_main.cpp

HEADERS += \
    ../asio_manager/asio_manager.h \
    ../crypto_tools/aes/aes.h \
    ../crypto_tools/aes/ctaes.h \
    ../crypto_tools/blake2/blake2-impl.h \
    ../crypto_tools/blake2/blake2.h \
    ../crypto_tools/blake2/blamka-round-opt.h \
    ../crypto_tools/blake2/blamka-round-ref.h \
    ../crypto_tools/key_tools.h \
    ../crypto_tools/secp256k1/basic-config.h \
    ../crypto_tools/secp256k1/bench.h \
    ../crypto_tools/secp256k1/ecdsa.h \
    ../crypto_tools/secp256k1/ecdsa_impl.h \
    ../crypto_tools/secp256k1/eckey.h \
    ../crypto_tools/secp256k1/eckey_impl.h \
    ../crypto_tools/secp256k1/ecmult.h \
    ../crypto_tools/secp256k1/ecmult_const.h \
    ../crypto_tools/secp256k1/ecmult_const_impl.h \
    ../crypto_tools/secp256k1/ecmult_gen.h \
    ../crypto_tools/secp256k1/ecmult_gen_impl.h \
    ../crypto_tools/secp256k1/ecmult_impl.h \
    ../crypto_tools/secp256k1/field.h \
    ../crypto_tools/secp256k1/field_10x26.h \
    ../crypto_tools/secp256k1/field_10x26_impl.h \
    ../crypto_tools/secp256k1/field_5x52.h \
    ../crypto_tools/secp256k1/field_5x52_asm_impl.h \
    ../crypto_tools/secp256k1/field_5x52_impl.h \
    ../crypto_tools/secp256k1/field_5x52_int128_impl.h \
    ../crypto_tools/secp256k1/field_impl.h \
    ../crypto_tools/secp256k1/group.h \
    ../crypto_tools/secp256k1/group_impl.h \
    ../crypto_tools/secp256k1/hash.h \
    ../crypto_tools/secp256k1/hash_impl.h \
    ../crypto_tools/secp256k1/main_impl.h \
    ../crypto_tools/secp256k1/num.h \
    ../crypto_tools/secp256k1/num_gmp.h \
    ../crypto_tools/secp256k1/num_gmp_impl.h \
    ../crypto_tools/secp256k1/num_impl.h \
    ../crypto_tools/secp256k1/scalar.h \
    ../crypto_tools/secp256k1/scalar_4x64.h \
    ../crypto_tools/secp256k1/scalar_4x64_impl.h \
    ../crypto_tools/secp256k1/scalar_8x32.h \
    ../crypto_tools/secp256k1/scalar_8x32_impl.h \
    ../crypto_tools/secp256k1/scalar_impl.h \
    ../crypto_tools/secp256k1/scalar_low.h \
    ../crypto_tools/secp256k1/scalar_low_impl.h \
    ../crypto_tools/secp256k1/scratch.h \
    ../crypto_tools/secp256k1/scratch_impl.h \
    ../crypto_tools/secp256k1/secp256k1.h \
    ../crypto_tools/secp256k1/secp256k1_ecdh.h \
    ../crypto_tools/secp256k1/secp256k1_recovery.h \
    ../crypto_tools/secp256k1/testrand.h \
    ../crypto_tools/secp256k1/testrand_impl.h \
    ../crypto_tools/secp256k1/util.h \
    ../crypto_tools/string_tools.h \
    ../log/log.h \
    ../net_lib/client_server.h \
    ../net_lib/client_socket_item.h \
    config.h \
    proxy.h \
    widget_main.h

FORMS += \
    widget_main.ui
INCLUDEPATH += D:/code/boost/boost_1_80_0
CONFIG += debug_and_release

CONFIG(debug, debug|release){
LIBS += -LD:/code/boost/boost_1_80_0/stage/lib \
        -llibboost_json-vc142-mt-gd-x64-1_80 \
        -llibboost_random-vc142-mt-gd-x64-1_80 \
        -llibboost_filesystem-vc142-mt-gd-x64-1_80 \
        -lAdvapi32
} else {
LIBS += -LD:/code/boost/boost_1_80_0/stage/lib \
        -llibboost_json-vc142-mt-x64-1_80 \
        -llibboost_random-vc142-mt-x64-1_80 \
        -llibboost_filesystem-vc142-mt-x64-1_80 \
        -lAdvapi32
}
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
