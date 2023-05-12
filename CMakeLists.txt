cmake_minimum_required(VERSION 3.19)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(SDK_PATH "/Applications/ti/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS")
set(SDK_BIN "${SDK_PATH}/bin")

if(NOT CMAKE_FIND_ROOT_PATH)
    set(CMAKE_FIND_ROOT_PATH ${SDK_PATH})
endif()

set(CMAKE_C_COMPILER "${SDK_BIN}/armcl")
set(CMAKE_ASM_COMPILER "${SDK_BIN}/armcl")
set(CMAKE_CXX_COMPILER "${SDK_BIN}/armcl")
set(CMAKE_AR "${SDK_BIN}/armar")

# project settings

project(lwip_os_test2 C CXX ASM)

find_program(TOOLCHAIN_PATH ${CMAKE_C_COMPILER})
get_filename_component(TOOLCHAIN_PATH ${TOOLCHAIN_PATH} DIRECTORY)
get_filename_component(TOOLCHAIN_PATH ${TOOLCHAIN_PATH} DIRECTORY)

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/source/sys_link.cmd")

add_compile_options(-mv7R4 --code_state=32 --float_support=VFPv3D16 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --c99 --c++14)

include_directories(include)
include_directories(${TOOLCHAIN_PATH}/include)
include_directories(${TOOLCHAIN_PATH}/lib)

include_directories(example/hdk/inc)

include_directories(lwip-1.4.1)
include_directories(lwip-1.4.1/apps/httpserver_raw)
include_directories(lwip-1.4.1/src/include)
include_directories(lwip-1.4.1/src/include/ipv4)
include_directories(lwip-1.4.1/src/include/ipv4/lwip)
include_directories(lwip-1.4.1/ports/hdk/include)

add_link_options(--stack_size=0x800 --heap_size=0x800 -i${TOOLCHAIN_PATH}/lib -i${TOOLCHAIN_PATH}/include --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info=lwip_os_test2_linkInfo.xml --rom_model --be32 ${CMAKE_SOURCE_DIR}/source/sys_link.cmd)

set(SOURCE_FILES
        source/dabort.asm
        source/emac.c
        source/errata_SSWF021_45.c
        source/esm.c
        source/gio.c
        source/mdio.c
        source/notification.c
        source/phy_dp83640.c
        source/pinmux.c
        source/sci.c
        source/spi.c
        source/sys_core.asm
        source/sys_dma.c
        source/sys_intvecs.asm
        source/sys_main.c
        source/sys_mpu.asm
        source/sys_pcr.c
        source/sys_phantom.c
        source/sys_pmm.c
        source/sys_pmu.asm
        source/sys_selftest.c
        source/sys_startup.c
        source/sys_vim.c
        source/system.c
        source/os_croutine.c
        source/os_event_groups.c
        source/os_heap.c
        source/os_list.c
        source/os_mpu_wrappers.c
        source/os_port.c
        source/os_portasm.asm
        source/os_queue.c
        source/os_tasks.c
        source/os_timer.c
        example/hdk/src/lwip_main.c
        lwip-1.4.1/ports/hdk/lwiplib.c lwip-1.4.1/apps/httpserver_raw/httpd.c lwip-1.4.1/apps/httpserver_raw/fs.c lwip-1.4.1/src/core/timers.c)

add_executable(${PROJECT_NAME}.out ${SOURCE_FILES} ${LINKER_SCRIPT})
target_link_libraries(${PROJECT_NAME}.out ${TOOLCHAIN_PATH}/lib/rtsv7R4_T_be_v3D16_eabi.lib)