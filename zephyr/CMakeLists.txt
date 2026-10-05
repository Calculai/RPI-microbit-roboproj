cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(microbit_zephyr_ble)

file(GLOB app_sources CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_LIST_DIR}/../ble/*.c"
	"${CMAKE_CURRENT_LIST_DIR}/../ble/*.cpp"
)

target_sources(app PRIVATE ${app_sources})