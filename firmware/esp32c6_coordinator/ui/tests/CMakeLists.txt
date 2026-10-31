cmake_minimum_required(VERSION 3.16)
project(coordinator_ui_tests C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(LV_BUILD_CONF_PATH "${CMAKE_CURRENT_LIST_DIR}/../lv_conf.h" CACHE PATH "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "" FORCE)
set(LV_FETCH_DEPENDENCIES OFF CACHE BOOL "" FORCE)
add_subdirectory("${LVGL_PATH}" lvgl)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(UI "${CMAKE_CURRENT_LIST_DIR}/..")
file(GLOB UI_ASSETS CONFIGURE_DEPENDS "${UI}/assets/*")
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ui_splash_asset.h"
    COMMAND "${Python3_EXECUTABLE}" "${UI}/generate_splash.py" "${UI}/assets/splash.json" "${CMAKE_CURRENT_BINARY_DIR}/ui_splash_asset.h"
    DEPENDS "${UI}/generate_splash.py" ${UI_ASSETS})
add_executable(ui-test model_and_preview.c "${UI}/ui_model.c" "${UI}/ui_geometry.c" "${UI}/ui_view.c"
    "${UI}/ui_layout.c" "${UI}/ui_controls.c" "${UI}/ui_feedback.c"
    "${UI}/../boards/waveshare_touch_1_9/board_identity.c"
    "${UI}/generated/screens.c" "${UI}/generated/ui.c" "${UI}/generated/images.c" "${UI}/generated/styles.c"
    "${CMAKE_CURRENT_BINARY_DIR}/ui_splash_asset.h")
target_include_directories(ui-test PRIVATE "${UI}" "${CMAKE_CURRENT_BINARY_DIR}" "${UI}/../boards/waveshare_touch_1_9" "${CMAKE_CURRENT_LIST_DIR}/stubs")
target_compile_options(ui-test PRIVATE -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer)
target_compile_options(lvgl PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
target_link_options(ui-test PRIVATE -fsanitize=address,undefined)
target_link_libraries(ui-test PRIVATE lvgl m)
