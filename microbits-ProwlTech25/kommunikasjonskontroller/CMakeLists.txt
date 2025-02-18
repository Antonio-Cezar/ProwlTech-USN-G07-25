cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(kommunikasjons_kontroller)

target_sources(app PRIVATE src/main.c src/canbus.c src/math_calc.c src/path_check.c src/interrupt_and_threads.c src/bt_controller.c)
