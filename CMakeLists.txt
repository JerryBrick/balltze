# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.16)

# Set project info
project(balltze
    VERSION 1.0.0
    LANGUAGES ASM C CXX
)

set(CMAKE_CXX_STANDARD 20)

# Set modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Include stuff
include(lua/code_gen/tag_definitions.cmake)
include(lib/lua/lua.cmake)
include(lib/fmt/fmt.cmake)
include(lib/invader/invader.cmake)
include(src/sound-subtitles-stapler/sss.cmake)

# Set include path
include_directories(include include/lua lib/)

# Define Balltze export macro
add_definitions(-DBALLTZE_EXPORTS)

# Turn on experimental features
option(BALLTZE_ENABLE_EXPERIMENTAL "enable experimental features" OFF)
if(BALLTZE_ENABLE_EXPERIMENTAL)
    add_definitions(-DBALLTZE_ENABLE_EXPERIMENTAL)
endif()

add_library(balltze SHARED
    src/balltze/command/command.cpp
    src/balltze/command/command.S
    src/balltze/command/help.cpp
    src/balltze/command/tab_completion.cpp
    src/balltze/config/chimera_preferences.cpp
    src/balltze/config/config.cpp
    src/balltze/config/ini.cpp
    src/balltze/engine/core.cpp
    src/balltze/engine/core.S
    src/balltze/engine/data_types.cpp
    src/balltze/engine/game_state.cpp
    src/balltze/engine/game_state.S
    src/balltze/engine/map.cpp
    src/balltze/engine/netgame.cpp
    src/balltze/engine/netgame.S
    src/balltze/engine/renderer.cpp
    src/balltze/engine/renderer.S
    src/balltze/engine/script.cpp
    src/balltze/engine/script.S
    src/balltze/engine/tag.cpp
    src/balltze/engine/tag.S
    src/balltze/engine/user_interface.cpp
    src/balltze/engine/user_interface.S
    src/balltze/event/camera.cpp
    src/balltze/event/console_command.cpp
    src/balltze/event/console_command.S
    src/balltze/event/d3d9.cpp
    src/balltze/event/d3d9.S
    src/balltze/event/event.cpp
    src/balltze/event/frame.cpp
    src/balltze/event/game_input.cpp
    src/balltze/event/game_input.S
    src/balltze/event/hud_hold_to_action_message.cpp
    src/balltze/event/hud_hold_to_action_message.S
    src/balltze/event/map_load.cpp
    src/balltze/event/map_load.S
    src/balltze/event/netgame_message.cpp
    src/balltze/event/netgame_message.S
    src/balltze/event/object_damage.cpp
    src/balltze/event/object_damage.S
    src/balltze/event/rcon_message.cpp
    src/balltze/event/render.cpp
    src/balltze/event/render.S
    src/balltze/event/server_connect.cpp
    src/balltze/event/server_connect.S
    src/balltze/event/sound_playback.cpp
    src/balltze/event/sound_playback.S
    src/balltze/event/tick.cpp
    src/balltze/event/widget.cpp
    src/balltze/event/widget.S
    src/balltze/features/gamepad_pause_button.cpp
    src/balltze/features/hud_button_prompts.cpp
    src/balltze/features/loading_screen/loading_screen.cpp
    src/balltze/features/loading_screen/loading_screen.S
    src/balltze/features/loading_screen/loading_screen.rc
    src/balltze/features/shaders/shader_transparent_chicago.cpp
    src/balltze/features/shaders/shader_transparent_chicago.S
    src/balltze/features/sound_subtitles.cpp
    src/balltze/features/tag_data_importing/map.cpp
    src/balltze/features/tag_data_importing/tag_data_importing.cpp
    src/balltze/features/tag_data_importing/tag_data_importing.S
    src/balltze/features/map_data_read_warden.cpp
    src/balltze/features/map_textures_preloading.cpp
    src/balltze/features/menu_widescreen.cpp
    src/balltze/features/menu_widescreen.S
    src/balltze/features/extended_limits.cpp
    src/balltze/features/extended_limits.S
    src/balltze/features/echo_message.cpp
    src/balltze/helpers/d3d9.cpp
    src/balltze/helpers/resources.cpp
    src/balltze/helpers/sprite_shader.rc
    src/balltze/lua_library/library.cpp
    src/balltze/math/bezier.cpp
    src/balltze/math/trig.cpp
    src/balltze/memory/codefinder.cpp
    src/balltze/memory/hook.cpp
    src/balltze/memory/memory.cpp
    src/balltze/output/draw_text.cpp
    src/balltze/output/draw_text.S
    src/balltze/output/logger.cpp
    src/balltze/output/messaging.cpp
    src/balltze/output/subtitles.cpp
    src/balltze/output/video.cpp
    src/balltze/output/video.S
    src/balltze/plugins/loader.cpp
    src/balltze/plugins/plugin.cpp
    src/balltze/plugins/lua/api.cpp
    src/balltze/plugins/lua/engine/core.cpp
    src/balltze/plugins/lua/engine/game_state.cpp
    src/balltze/plugins/lua/engine/map.cpp
    src/balltze/plugins/lua/engine/netgame.cpp
    src/balltze/plugins/lua/engine/renderer.cpp
    src/balltze/plugins/lua/engine/tag.cpp
    src/balltze/plugins/lua/engine/user_interface.cpp
    src/balltze/plugins/lua/chimera.cpp
    src/balltze/plugins/lua/command.cpp
    src/balltze/plugins/lua/engine.cpp
    src/balltze/plugins/lua/features.cpp
    src/balltze/plugins/lua/event.cpp
    src/balltze/plugins/lua/helpers.cpp
    src/balltze/plugins/lua/preloaded_libraries.cpp
    src/balltze/plugins/lua/preloaded_libraries.rc
    src/balltze/plugins/lua/lua-fmt/lfmt.c
    src/balltze/plugins/lua/logger.cpp
    src/balltze/plugins/lua/math.cpp
    src/balltze/plugins/lua/misc.cpp
    src/balltze/plugins/lua/output.cpp
    src/balltze/balltze.cpp

    ${TAG_REBASE_OFFSETS_FUNCTION_CPP} 
    ${TAG_RESOLVE_DEPENDENCIES_FUNCTION_CPP} 
    ${TAG_LUA_METATABLE_FUNCTION_CPP}
    ${TAG_COPY_DATA_FUNCTION_CPP}
)

# Set Balltze output properties
set_target_properties(balltze PROPERTIES PREFIX "")
set_target_properties(balltze PROPERTIES OUTPUT_NAME "balltze")
set_target_properties(balltze PROPERTIES LINK_FLAGS "-static -static-libgcc -static-libstdc++")
add_dependencies(balltze tag-definitions-headers)
target_link_libraries(balltze lua53 fmt invader d3d9 gdiplus)

# Install Balltze
install(TARGETS balltze DESTINATION "${CMAKE_INSTALL_PREFIX}")
