idf_component_register(SRCS "ui_model.c" "ui_geometry.c" "ui_view.c" "ui_runtime.c"
                           "generated/screens.c" "generated/ui.c" "generated/images.c" "generated/styles.c"
                       INCLUDE_DIRS "." "${CMAKE_CURRENT_BINARY_DIR}"
                       REQUIRES waveshare_display esp_timer nvs_flash lvgl esp_hw_support)
if(CMAKE_BUILD_EARLY_EXPANSION)
    return()
endif()
set(SPLASH_HEADER "${CMAKE_CURRENT_BINARY_DIR}/ui_splash_asset.h")
file(GLOB SPLASH_INPUTS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
add_custom_command(OUTPUT "${SPLASH_HEADER}"
    COMMAND ${PYTHON} "${CMAKE_CURRENT_SOURCE_DIR}/generate_splash.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/splash.json" "${SPLASH_HEADER}"
    DEPENDS ${SPLASH_INPUTS} "${CMAKE_CURRENT_SOURCE_DIR}/generate_splash.py"
    VERBATIM)
add_custom_target(coordinator_splash DEPENDS "${SPLASH_HEADER}")
add_dependencies(${COMPONENT_LIB} coordinator_splash)
