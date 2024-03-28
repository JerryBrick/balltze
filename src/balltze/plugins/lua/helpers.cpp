// SPDX-License-Identifier: GPL-3.0-only

#include <balltze/engine/data_types.hpp>
#include <balltze/engine/tag_definitions.hpp>
#include "../../logger.hpp"
#include "../loader.hpp"
#include "helpers.hpp"

namespace Balltze::Plugins {
    int lua_meta_object_read_only__newindex(lua_State *state) noexcept {
        return luaL_error(state, "Attempt to modify a read-only table");
    }

    int lua_meta_object__eq(lua_State *state) noexcept {
        // this may never happen due getequalhandler function
        if(!lua_istable(state, 1) || !lua_istable(state, 2)) { 
            lua_pushboolean(state, false);
            return 1;
        }

        lua_getfield(state, 1, "_data");
        if(!lua_islightuserdata(state, -1)) {
            lua_pop(state, 1);
            lua_pushboolean(state, false);
            return 1;
        }
        void *data1 = lua_touserdata(state, -1);
        lua_pop(state, 1);

        lua_getfield(state, 2, "_data");
        if(!lua_islightuserdata(state, -1)) {
            lua_pop(state, 1);
            lua_pushboolean(state, false);
            return 1;
        }
        void *data2 = lua_touserdata(state, -1);
        lua_pop(state, 1);

        lua_pushboolean(state, data1 == data2);
        return 1;
    }

    void lua_create_functions_table(lua_State *state, const char *name, const luaL_Reg *functions) noexcept {
        lua_pushstring(state, name);
        luaL_newlibtable(state, functions);
        luaL_setfuncs(state, functions, 0);
        lua_settable(state, -3);
    }

    Engine::ColorARGB lua_to_color_argb(lua_State *state, int index) {
        Engine::ColorARGB color;
        if(lua_istable(state, index)) {
            auto get_field = [&state, &index](const char *field, float &value) {
                lua_getfield(state, index, field);
                if(lua_isnumber(state, -1)) {
                    value = lua_tonumber(state, -1);
                }
                else {
                    auto *error = "Expected number for color field.";
                    luaL_error(state, error);
                    throw std::runtime_error(error);
                }
                lua_pop(state, 1);
            };

            try {
                get_field("red", color.red);
                get_field("green", color.green);
                get_field("blue", color.blue);
                get_field("alpha", color.alpha);
            }
            catch(std::runtime_error &e) {
                throw;
            }
        }
        else {
            if(lua_gettop(state) >= index + 3) {
                auto get_field = [&state, &index](float &value) {
                    if(lua_isnumber(state, index)) {
                        value = lua_tonumber(state, index);
                        index++;
                    }
                    else {
                        auto *error = "Expected number for color field.";
                        luaL_error(state, error);
                        throw std::runtime_error(error);
                    }
                };

                try {
                    get_field(color.red);
                    get_field(color.green);
                    get_field(color.blue);
                    get_field(color.alpha);
                }
                catch(std::runtime_error &e) {
                    throw;
                }
            }
            else {
                throw std::runtime_error("Could not get color from table or stack.");
            }
        }
        return color;
    }

    std::string engine_edition_to_string(Engine::EngineEdition edition) {
        switch(edition) {
            case Engine::ENGINE_TYPE_RETAIL:
                return "retail";
            case Engine::ENGINE_TYPE_DEMO:
                return "demo";
            case Engine::ENGINE_TYPE_CUSTOM_EDITION:
                return "custom";
            default:
                return "unknown";
        }
    }

    Engine::EngineEdition engine_edition_from_string(const std::string &edition) {
        if(edition == "retail") {
            return Engine::ENGINE_TYPE_RETAIL;
        }
        else if(edition == "demo") {
            return Engine::ENGINE_TYPE_DEMO;
        }
        else if(edition == "custom") {
            return Engine::ENGINE_TYPE_CUSTOM_EDITION;
        }
        else {
            throw std::runtime_error("Invalid engine edition.");
        }
    }

    std::string cache_file_engine_to_string(Engine::CacheFileEngine engine) {
        switch(engine) {
            case Engine::CACHE_FILE_XBOX:
                return "xbox";
            case Engine::CACHE_FILE_DEMO:
                return "demo";
            case Engine::CACHE_FILE_RETAIL:
                return "retail";
            case Engine::CACHE_FILE_CUSTOM_EDITION:
                return "custom";
            case Engine::CACHE_FILE_INVADER:
                return "invader";
            case Engine::CACHE_FILE_DEMO_COMPRESSED:
                return "demo compressed";
            case Engine::CACHE_FILE_RETAIL_COMPRESSED:
                return "retail compressed";
            case Engine::CACHE_FILE_CUSTOM_EDITION_COMPRESSED:
                return "custom compressed";
            default:
                return "unknown";
        }   
    }

    Engine::CacheFileEngine cache_file_engine_from_string(const std::string &engine) {
        if(engine == "xbox") {
            return Engine::CACHE_FILE_XBOX;
        }
        else if(engine == "demo") {
            return Engine::CACHE_FILE_DEMO;
        }
        else if(engine == "retail") {
            return Engine::CACHE_FILE_RETAIL;
        }
        else if(engine == "custom") {
            return Engine::CACHE_FILE_CUSTOM_EDITION;
        }
        else if(engine == "invader") {
            return Engine::CACHE_FILE_INVADER;
        }
        else if(engine == "demo compressed") {
            return Engine::CACHE_FILE_DEMO_COMPRESSED;
        }
        else if(engine == "retail compressed") {
            return Engine::CACHE_FILE_RETAIL_COMPRESSED;
        }
        else if(engine == "custom compressed") {
            return Engine::CACHE_FILE_CUSTOM_EDITION_COMPRESSED;
        }
        else {
            throw std::runtime_error("Invalid cache file engine.");
        }
    }

    std::string map_game_type_to_string(Engine::MapGameType type) {
        switch(type) {
            case Engine::MAP_SINGLE_PLAYER:
                return "single_player";
            case Engine::MAP_MULTIPLAYER:
                return "multiplayer";
            case Engine::MAP_USER_INTERFACE:
                return "user_interface";
            default:
                return "unknown";
        }
    }

    Engine::MapGameType map_game_type_from_string(const std::string &type) {
        if(type == "single_player") {
            return Engine::MAP_SINGLE_PLAYER;
        }
        else if(type == "multiplayer") {
            return Engine::MAP_MULTIPLAYER;
        }
        else if(type == "user_interface") {
            return Engine::MAP_USER_INTERFACE;
        }
        else {
            throw std::runtime_error("Invalid map game type.");
        }
    }

    std::string network_game_server_type_to_string(Engine::NetworkGameServerType type) {
        switch(type) {
            case Engine::NETWORK_GAME_SERVER_NONE:
                return "none";
            case Engine::NETWORK_GAME_SERVER_DEDICATED:
                return "dedicated";
            case Engine::NETWORK_GAME_SERVER_LOCAL:
                return "local";
            default:
                return "unknown";
        }
    }

    Engine::NetworkGameServerType network_game_server_type_from_string(const std::string &type) {
        if(type == "none") {
            return Engine::NETWORK_GAME_SERVER_NONE;
        }
        else if(type == "dedicated") {
            return Engine::NETWORK_GAME_SERVER_DEDICATED;
        }
        else if(type == "local") {
            return Engine::NETWORK_GAME_SERVER_LOCAL;
        }
        else {
            throw std::runtime_error("Invalid server type.");
        }
    }

    std::string network_game_server_game_type_to_string(Engine::NetworkGameType gametype) {
        switch(gametype) {
            case Engine::NETWORK_GAMETYPE_CTF:
                return "ctf";
            case Engine::NETWORK_GAMETYPE_SLAYER:
                return "slayer";
            case Engine::NETWORK_GAMETYPE_ODDBALL:
                return "oddball";
            case Engine::NETWORK_GAMETYPE_KING:
                return "king";
            case Engine::NETWORK_GAMETYPE_RACE:
                return "race";
            default:
                return "none";
        }
    }

    Engine::NetworkGameType network_game_server_game_type_from_string(const std::string &gametype) {
        if(gametype == "ctf") {
            return Engine::NETWORK_GAMETYPE_CTF;
        }
        else if(gametype == "slayer") {
            return Engine::NETWORK_GAMETYPE_SLAYER;
        }
        else if(gametype == "oddball") {
            return Engine::NETWORK_GAMETYPE_ODDBALL;
        }
        else if(gametype == "king") {
            return Engine::NETWORK_GAMETYPE_KING;
        }
        else if(gametype == "race") {
            return Engine::NETWORK_GAMETYPE_RACE;
        }
        else {
            throw std::runtime_error("Invalid server game type.");
        }
    }

    void lua_push_engine_matrix(lua_State *state, Engine::Matrix &matrix) noexcept {
        lua_newtable(state);
        for(std::size_t i = 0; i < 3; i++) {
            lua_newtable(state);
            for(std::size_t j = 0; j < 3; j++) {
                lua_pushnumber(state, matrix[i][j]);
                lua_rawseti(state, -2, j + 1);
            }
            lua_rawseti(state, -2, i + 1);
        }
    }

    std::shared_ptr<Engine::Matrix> lua_to_engine_matrix(lua_State *state, int index) noexcept {
        auto matrix_ptr = new Engine::Matrix();
        std::shared_ptr<Engine::Matrix> matrix(matrix_ptr);
        for(std::size_t i = 0; i < 3; i++) {
            lua_rawgeti(state, index, i + 1);
            for(std::size_t j = 0; j < 3; j++) {
                lua_rawgeti(state, -1, j + 1);
                matrix[i][j] = luaL_checknumber(state, -1);
                lua_pop(state, 1);
            }
            lua_pop(state, 1);
        }
        return matrix;
    }

    void lua_push_engine_resolution(lua_State *state, Engine::Resolution &resolution) noexcept {
        lua_newtable(state);
        lua_pushinteger(state, resolution.width);
        lua_setfield(state, -2, "width");
        lua_pushinteger(state, resolution.height);
        lua_setfield(state, -2, "height");
    }

    std::string object_type_to_string(Engine::ObjectType type) {
        switch(type) {
            case Engine::OBJECT_TYPE_BIPED: 
                return "biped";
            case Engine::OBJECT_TYPE_VEHICLE:
                return "vehicle";
            case Engine::OBJECT_TYPE_WEAPON:
                return "weapon";
            case Engine::OBJECT_TYPE_EQUIPMENT:
                return "equipment";
            case Engine::OBJECT_TYPE_GARBAGE:
                return "garbage";
            case Engine::OBJECT_TYPE_PROJECTILE:
                return "projectile";
            case Engine::OBJECT_TYPE_SCENERY:
                return "scenery";
            case Engine::OBJECT_TYPE_DEVICE_MACHINE:
                return "device_machine";
            case Engine::OBJECT_TYPE_DEVICE_CONTROL:
                return "device_control";
            case Engine::OBJECT_TYPE_DEVICE_LIGHT_FIXTURE:
                return "device_light_fixture";
            case Engine::OBJECT_TYPE_PLACEHOLDER:
                return "placeholder";
            case Engine::OBJECT_TYPE_SOUND_SCENERY:
                return "sound_scenery";
            default:
                return "unknown";
        }
    }

    Engine::ObjectType object_type_from_string(const std::string &type) {
        if(type == "biped") {
            return Engine::OBJECT_TYPE_BIPED;
        }
        else if(type == "vehicle") {
            return Engine::OBJECT_TYPE_VEHICLE;
        }
        else if(type == "weapon") {
            return Engine::OBJECT_TYPE_WEAPON;
        }
        else if(type == "equipment") {
            return Engine::OBJECT_TYPE_EQUIPMENT;
        }
        else if(type == "garbage") {
            return Engine::OBJECT_TYPE_GARBAGE;
        }
        else if(type == "projectile") {
            return Engine::OBJECT_TYPE_PROJECTILE;
        }
        else if(type == "scenery") {
            return Engine::OBJECT_TYPE_SCENERY;
        }
        else if(type == "device_machine") {
            return Engine::OBJECT_TYPE_DEVICE_MACHINE;
        }
        else if(type == "device_control") {
            return Engine::OBJECT_TYPE_DEVICE_CONTROL;
        }
        else if(type == "device_light_fixture") {
            return Engine::OBJECT_TYPE_DEVICE_LIGHT_FIXTURE;
        }
        else if(type == "placeholder") {
            return Engine::OBJECT_TYPE_PLACEHOLDER;
        }
        else if(type == "sound_scenery") {
            return Engine::OBJECT_TYPE_SOUND_SCENERY;
        }
        else {
            throw std::runtime_error("Invalid object type.");
        }
    }

    std::string object_network_role_to_string(Engine::ObjectNetworkRole role) {
        switch(role) {
            case Engine::OBJECT_NETWORK_ROLE_MASTER:
                return "master";
            case Engine::OBJECT_NETWORK_ROLE_PUPPET:
                return "puppet";
            case Engine::OBJECT_NETWORK_ROLE_LOCALLY_CONTROLLED_PUPPET:
                return "locally_controlled_puppet";
            case Engine::OBJECT_NETWORK_ROLE_LOCAL_ONLY:
                return "local_only";
            default:
                return "unknown";
        }
    }

    Engine::ObjectNetworkRole object_network_role_from_string(const std::string &role) {
        if(role == "master") {
            return Engine::OBJECT_NETWORK_ROLE_MASTER;
        }
        else if(role == "puppet") {
            return Engine::OBJECT_NETWORK_ROLE_PUPPET;
        }
        else if(role == "locally_controlled_puppet") {
            return Engine::OBJECT_NETWORK_ROLE_LOCALLY_CONTROLLED_PUPPET;
        }
        else if(role == "local_only") {
            return Engine::OBJECT_NETWORK_ROLE_LOCAL_ONLY;
        }
        else {
            throw std::runtime_error("Invalid object network role.");
        }
    }

    std::string object_attachment_type_to_string(Engine::BaseObjectAttachmentType type) {
        switch(type) {
            case Engine::OBJECT_ATTACHMENT_TYPE_INVALID: 
                return "invalid";
            case Engine::OBJECT_ATTACHMENT_TYPE_LIGHT:
                return "light";
            case Engine::OBJECT_ATTACHMENT_TYPE_LOOPING_SOUND:
                return "looping_sound";
            case Engine::OBJECT_ATTACHMENT_TYPE_EFFECT:
                return "effect";
            case Engine::OBJECT_ATTACHMENT_TYPE_CONTRAIL:
                return "contrail";
            case Engine::OBJECT_ATTACHMENT_TYPE_PARTICLE:
                return "particle";
            default:
                return "unknown";
        }
    }

    Engine::BaseObjectAttachmentType object_attachment_type_from_string(const std::string &type) {
        if(type == "invalid") {
            return Engine::OBJECT_ATTACHMENT_TYPE_INVALID;
        }
        else if(type == "light") {
            return Engine::OBJECT_ATTACHMENT_TYPE_LIGHT;
        }
        else if(type == "looping_sound") {
            return Engine::OBJECT_ATTACHMENT_TYPE_LOOPING_SOUND;
        }
        else if(type == "effect") {
            return Engine::OBJECT_ATTACHMENT_TYPE_EFFECT;
        }
        else if(type == "contrail") {
            return Engine::OBJECT_ATTACHMENT_TYPE_CONTRAIL;
        }
        else if(type == "particle") {
            return Engine::OBJECT_ATTACHMENT_TYPE_PARTICLE;
        }
        else {
            throw std::runtime_error("Invalid dynamic object attachment type.");
        }
    }

    std::string network_game_multiplayer_team_to_string(Engine::NetworkGameMultiplayerTeam team) {
        switch(team) {
            case Engine::NETWORK_GAME_TEAM_RED:
                return "red";
            case Engine::NETWORK_GAME_TEAM_BLUE:
                return "blue";
            default:
                return "none";
        }
    }
    
    Engine::NetworkGameMultiplayerTeam network_game_multiplayer_team_from_string(const std::string &team) {
        if(team == "red") {
            return Engine::NETWORK_GAME_TEAM_RED;
        }
        else if(team == "blue") {
            return Engine::NETWORK_GAME_TEAM_BLUE;
        }
        else {
            throw std::runtime_error("Invalid multiplayer team.");
        }
    }

    std::string camera_type_to_string(Engine::CameraType camera_type) {
        switch(camera_type) {
            case Engine::CAMERA_FIRST_PERSON: 
                return "first_person";
            case Engine::CAMERA_VEHICLE:
                return "vehicle";
            case Engine::CAMERA_CINEMATIC:
                return "cinematic";
            case Engine::CAMERA_DEBUG:
                return "debug";
            default:
                return "unknown";
        }
    }

    Engine::CameraType camera_type_from_string(const std::string &camera_type) {
        if(camera_type == "first_person") {
            return Engine::CAMERA_FIRST_PERSON;
        }
        else if(camera_type == "vehicle") {
            return Engine::CAMERA_VEHICLE;
        }
        else if(camera_type == "cinematic") {
            return Engine::CAMERA_CINEMATIC;
        }
        else if(camera_type == "debug") {
            return Engine::CAMERA_DEBUG;
        }
        else {
            throw std::runtime_error("Invalid camera type.");
        }
    }

    Engine::Point2DInt lua_to_point2_d_int(lua_State *state, int index) {
        Engine::Point2DInt point;
        if(lua_istable(state, index)) {
            auto get_field = [&state, &index](const char *field, std::int16_t &value) {
                lua_getfield(state, index, field);
                if(lua_isinteger(state, -1)) {
                    value = lua_tointeger(state, -1);
                }
                else {
                    auto *error = "Expected integer for Point2DInt field.";
                    throw std::runtime_error(error);
                }
                lua_pop(state, 1);
            };

            try {
                get_field("x", point.x);
                get_field("y", point.y);
            }
            catch(std::runtime_error &e) {
                throw;
            }
        }
        else {
            if(lua_gettop(state) >= index + 1) {
                auto get_field = [&state, &index](std::int16_t &value) {
                    if(lua_isinteger(state, index)) {
                        value = lua_tointeger(state, index);
                        index++;
                    }
                    else {
                        auto *error = "Expected integer for Point2DInt field.";
                        luaL_error(state, error);
                        throw std::runtime_error(error);
                    }
                };

                try {
                    get_field(point.x);
                    get_field(point.y);
                }
                catch(std::runtime_error &e) {
                    throw;
                }
            }
            else {
                throw std::runtime_error("Could not get Point2DInt from table or stack.");
            }
        }
        return point;
    }

    Engine::Point3D lua_to_point3_d(lua_State *state, int index) {
        Engine::Point3D point;
        if(lua_istable(state, index)) {
            auto get_field = [&state, &index](const char *field, float &value) {
                lua_getfield(state, index, field);
                if(lua_isnumber(state, -1)) {
                    value = lua_tonumber(state, -1);
                }
                else {
                    auto *error = "Expected number for Point3D field.";
                    throw std::runtime_error(error);
                }
                lua_pop(state, 1);
            };

            try {
                get_field("x", point.x);
                get_field("y", point.y);
                get_field("z", point.z);
            }
            catch(std::runtime_error &e) {
                throw;
            }
        }
        else {
            if(lua_gettop(state) >= index + 1) {
                auto get_field = [&state, &index](float &value) {
                    if(lua_isnumber(state, index)) {
                        value = lua_tonumber(state, index);
                        index++;
                    }
                    else {
                        auto *error = "Expected number for Point3D field.";
                        luaL_error(state, error);
                        throw std::runtime_error(error);
                    }
                };

                try {
                    get_field(point.x);
                    get_field(point.y);
                    get_field(point.z);
                }
                catch(std::runtime_error &e) {
                    throw;
                }
            }
            else {
                throw std::runtime_error("Could not get Point3D from table or stack.");
            }
        }
        return point;
    }

    Engine::ColorARGBInt lua_to_color_a_r_g_b_int(lua_State *state, int index) {
        Engine::ColorARGBInt color;
        if(lua_istable(state, index)) {
            auto get_field = [&state, &index](const char *field, std::uint8_t &value) {
                lua_getfield(state, index, field);
                if(lua_isinteger(state, -1)) {
                    value = lua_tointeger(state, -1);
                }
                else {
                    auto *error = "Expected integer for ColorARGBInt field.";
                    throw std::runtime_error(error);
                }
                lua_pop(state, 1);
            };

            try {
                get_field("red", color.red);
                get_field("green", color.green);
                get_field("blue", color.blue);
                get_field("alpha", color.alpha);
            }
            catch(std::runtime_error &e) {
                throw;
            }
        }
        else {
            if(lua_gettop(state) >= index + 1) {
                auto get_field = [&state, &index](std::uint8_t &value) {
                    if(lua_isinteger(state, index)) {
                        value = lua_tointeger(state, index);
                        index++;
                    }
                    else {
                        auto *error = "Expected integer for ColorARGBInt field.";
                        throw std::runtime_error(error);
                    }
                };

                try {
                    get_field(color.red);
                    get_field(color.green);
                    get_field(color.blue);
                    get_field(color.alpha);
                }
                catch(std::runtime_error &e) {
                    throw;
                }
            }
            else {
                throw std::runtime_error("Could not get ColorARGBInt from table or stack.");
            }
        }
        return color;
    }

    std::string input_device_to_string(Engine::InputDevice device) {
        switch(device) {
            case Engine::INPUT_DEVICE_KEYBOARD:
                return "keyboard";
            case Engine::INPUT_DEVICE_MOUSE:
                return "mouse";
            case Engine::INPUT_DEVICE_GAMEPAD:
                return "gamepad";
            default:
                return "unknown";
        }
    }

    Engine::InputDevice input_device_from_string(const std::string &device) {
        if(device == "keyboard") {
            return Engine::INPUT_DEVICE_KEYBOARD;
        }
        else if(device == "mouse") {
            return Engine::INPUT_DEVICE_MOUSE;
        }
        else if(device == "gamepad") {
            return Engine::INPUT_DEVICE_GAMEPAD;
        }
        else {
            throw std::runtime_error("Invalid input device.");
        }
    }

    void lua_engine_attach_tag_data_metatable(lua_State *state) noexcept;

    void lua_push_engine_tag(lua_State *state, Engine::Tag *tag) noexcept {
        lua_newtable(state);

        auto primary_class = Engine::tag_class_to_string(tag->primary_class);
        lua_pushstring(state, primary_class.c_str());
        lua_setfield(state, -2, "primaryClass");

        auto secondary_class = Engine::tag_class_to_string(tag->secondary_class);
        lua_pushstring(state, secondary_class.c_str());
        lua_setfield(state, -2, "secondaryClass");

        auto tertiary_class = Engine::tag_class_to_string(tag->tertiary_class);
        lua_pushstring(state, tertiary_class.c_str());
        lua_setfield(state, -2, "tertiaryClass");

        lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&tag->handle));
        lua_setfield(state, -2, "handle");

        lua_pushstring(state, tag->path);
        lua_setfield(state, -2, "path");

        lua_pushinteger(state, reinterpret_cast<std::uint32_t>(tag->data));
        lua_setfield(state, -2, "dataAddress");

        lua_pushboolean(state, tag->indexed);
        lua_setfield(state, -2, "indexed");
        
        lua_newtable(state);
        lua_pushlightuserdata(state, tag->data);
        lua_setfield(state, -2, "_tag_data");
        lua_pushinteger(state, tag->primary_class);
        lua_setfield(state, -2, "_tag_class");
        lua_pushinteger(state, tag->handle.handle);
        lua_setfield(state, -2, "_tag_handle");
        lua_engine_attach_tag_data_metatable(state);
        lua_setfield(state, -2, "data");
    }

    static int lua_meta_engine_tag__index(lua_State *state) noexcept {
        auto *tag = lua_from_meta_object<Engine::Tag>(state, 1);
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {
            return luaL_error(state, "Invalid key type");
        }

        std::string field = key;
        if(field == "primaryClass") {
            lua_pushstring(state, Engine::tag_class_to_string(tag->primary_class).c_str());
        }
        else if(field == "secondaryClass") {
            lua_pushstring(state, Engine::tag_class_to_string(tag->secondary_class).c_str());
        }
        else if(field == "tertiaryClass") {
            lua_pushstring(state, Engine::tag_class_to_string(tag->tertiary_class).c_str());
        }
        else if(field == "handle") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&tag->handle));
        }
        else if(field == "path") {
            lua_pushstring(state, tag->path);
        }
        else if(field == "dataAddress") {
            lua_pushinteger(state, reinterpret_cast<std::uint32_t>(tag->data));
        }
        else if(field == "indexed") {
            lua_pushboolean(state, tag->indexed);
        }
        else {
            return luaL_error(state, "Invalid key");
        }
        return 1;
    }

    static int lua_meta_engine_tag__newindex(lua_State *state) noexcept {
        auto *tag = lua_from_meta_object<Engine::Tag>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "primaryClass") {
            auto primary_class = Engine::tag_class_from_string(luaL_checkstring(state, 3));
            if(primary_class == Engine::TAG_CLASS_NULL) {
                return luaL_error(state, "Invalid tag class");
            }
            tag->primary_class = primary_class;
        }
        else if(field == "secondaryClass") {
            auto secondary_class = Engine::tag_class_from_string(luaL_checkstring(state, 3));
            if(secondary_class == Engine::TAG_CLASS_NULL) {
                return luaL_error(state, "Invalid tag class");
            }
            tag->secondary_class = secondary_class;
        }
        else if(field == "tertiaryClass") {
            auto tertiary_class = Engine::tag_class_from_string(luaL_checkstring(state, 3));
            if(tertiary_class == Engine::TAG_CLASS_NULL) {
                return luaL_error(state, "Invalid tag class");
            }
            tag->tertiary_class = tertiary_class;
        }
        else if(field == "handle") {
            return luaL_error(state, "Cannot modify tag handle");
        }
        else if(field == "path") {
            return luaL_error(state, "Cannot modify tag path");
        }
        else if(field == "dataAddress") {
            auto data = luaL_checkinteger(state, 3);
            tag->data = reinterpret_cast<std::byte *>(data);
        }
        else if(field == "indexed") {
            return luaL_error(state, "Cannot modify indexed flag");
        }
        else {
            return luaL_error(state, "Invalid key");
        }
        return 0;
    }

    void lua_push_meta_engine_tag(lua_State *state, Engine::Tag &tag, bool read_only) noexcept {
        lua_push_meta_object(state, tag, lua_meta_engine_tag__index, lua_meta_engine_tag__newindex, read_only);
    }

    void lua_push_engine_resource_handle(lua_State *state, const Engine::ResourceHandle &handle) noexcept {
        lua_newtable(state);

        lua_pushinteger(state, handle.handle);
        lua_setfield(state, -2, "handle");

        lua_pushinteger(state, handle.id);
        lua_setfield(state, -2, "id");

        lua_pushinteger(state, handle.index);
        lua_setfield(state, -2, "index");
    }

    void lua_push_engine_resource_handle(lua_State *state, Engine::ResourceHandle *handle) noexcept {
        lua_push_engine_resource_handle(state, *handle);
    }

    void lua_push_engine_object_handle(lua_State *state, const Engine::ObjectHandle &handle) noexcept {
        lua_push_engine_resource_handle(state, static_cast<const Engine::ResourceHandle &>(handle));
    }

    void lua_push_engine_tag_handle(lua_State *state, const Engine::TagHandle &handle) noexcept {
        lua_push_engine_resource_handle(state, static_cast<const Engine::ResourceHandle &>(handle));
    }

    void lua_push_engine_player_handle(lua_State *state, const Engine::PlayerHandle &handle) noexcept {
        lua_push_engine_resource_handle(state, static_cast<const Engine::ResourceHandle &>(handle));
    }

    std::string unit_throwing_grenade_state_to_string(Engine::UnitThrowingGrenadeState state) {
        switch(state) {
            case Engine::UNIT_THROWING_GRENADE_STATE_NONE:
                return "none";
            case Engine::UNIT_THROWING_GRENADE_STATE_BEGIN:
                return "begin";
            case Engine::UNIT_THROWING_GRENADE_STATE_IN_HAND:
                return "in_hand";
            case Engine::UNIT_THROWING_GRENADE_STATE_RELEASED:
                return "released";
            default:
                return "unknown";
        }
    }

    Engine::UnitThrowingGrenadeState unit_throwing_grenade_state_from_string(const std::string &state) {
        if(state == "none") {
            return Engine::UNIT_THROWING_GRENADE_STATE_NONE;
        }
        else if(state == "begin") {
            return Engine::UNIT_THROWING_GRENADE_STATE_BEGIN;
        }
        else if(state == "in_hand") {
            return Engine::UNIT_THROWING_GRENADE_STATE_IN_HAND;
        }
        else if(state == "released") {
            return Engine::UNIT_THROWING_GRENADE_STATE_RELEASED;
        }
        else {
            return Engine::UNIT_THROWING_GRENADE_STATE_NONE;
        }
    }

    std::string unit_animation_state_to_string(Engine::UnitAnimationState state) {
        switch(state) {
            case Engine::UNI_ANIMATION_STATE_INVALID:
                return "invalid";
            case Engine::UNI_ANIMATION_STATE_IDLE:
                return "idle";
            case Engine::UNI_ANIMATION_STATE_GESTURE:
                return "gesture";
            case Engine::UNI_ANIMATION_STATE_TURN_LEFT:
                return "turn_left";
            case Engine::UNI_ANIMATION_STATE_TURN_RIGHT:
                return "turn_right";
            case Engine::UNI_ANIMATION_STATE_MOVE_FRONT:
                return "move_front";
            case Engine::UNI_ANIMATION_STATE_MOVE_BACK:
                return "move_back";
            case Engine::UNI_ANIMATION_STATE_MOVE_LEFT:
                return "move_left";
            case Engine::UNI_ANIMATION_STATE_MOVE_RIGHT:
                return "move_right";
            case Engine::UNI_ANIMATION_STATE_STUNNED_FRONT:
                return "stunned_front";
            case Engine::UNI_ANIMATION_STATE_STUNNED_BACK:
                return "stunned_back";
            case Engine::UNI_ANIMATION_STATE_STUNNED_LEFT:
                return "stunned_left";
            case Engine::UNI_ANIMATION_STATE_STUNNED_RIGHT:
                return "stunned_right";
            case Engine::UNI_ANIMATION_STATE_SLIDE_FRONT:
                return "slide_front";
            case Engine::UNI_ANIMATION_STATE_SLIDE_BACK:
                return "slide_back";
            case Engine::UNI_ANIMATION_STATE_SLIDE_LEFT:
                return "slide_left";
            case Engine::UNI_ANIMATION_STATE_SLIDE_RIGHT:
                return "slide_right";
            case Engine::UNI_ANIMATION_STATE_READY:
                return "ready";
            case Engine::UNI_ANIMATION_STATE_PUT_AWAY:
                return "put_away";
            case Engine::UNI_ANIMATION_STATE_AIM_STILL:
                return "aim_still";
            case Engine::UNI_ANIMATION_STATE_AIM_MOVE:
                return "aim_move";
            case Engine::UNI_ANIMATION_STATE_AIRBORNE:
                return "airborne";
            case Engine::UNI_ANIMATION_STATE_LAND_SOFT:
                return "land_soft";
            case Engine::UNI_ANIMATION_STATE_LAND_HARD:
                return "land_hard";
            case Engine::UNI_ANIMATION_STATE_UNKNOWN23:
                return "unknown23";
            case Engine::UNI_ANIMATION_STATE_AIRBORNE_DEAD:
                return "airborne_dead";
            case Engine::UNI_ANIMATION_STATE_LANDING_DEAD:
                return "landing_dead";
            case Engine::UNI_ANIMATION_STATE_SEAT_ENTER:
                return "seat_enter";
            case Engine::UNI_ANIMATION_STATE_SEAT_EXIT:
                return "seat_exit";
            case Engine::UNI_ANIMATION_STATE_CUSTOM_ANIMATION:
                return "custom_animation";
            case Engine::UNI_ANIMATION_STATE_IMPULSE:
                return "impulse";
            case Engine::UNI_ANIMATION_STATE_MELEE:
                return "melee";
            case Engine::UNI_ANIMATION_STATE_MELEE_AIRBORNE:
                return "melee_airborne";
            case Engine::UNI_ANIMATION_STATE_MELEE_CONTINUOUS:
                return "melee_continuous";
            case Engine::UNI_ANIMATION_STATE_THROW_GRENADE:
                return "throw_grenade";
            case Engine::UNI_ANIMATION_STATE_RESSURECT_FRONT:
                return "ressurect_front";
            case Engine::UNI_ANIMATION_STATE_RESSURECT_BACK:
                return "ressurect_back";
            case Engine::UNI_ANIMATION_STATE_FEEDING:
                return "feeding";
            case Engine::UNI_ANIMATION_STATE_SURPRISE_FRONT:
                return "surprise_front";
            case Engine::UNI_ANIMATION_STATE_SURPRISE_BACK:
                return "surprise_back";
            case Engine::UNI_ANIMATION_STATE_LEAP_START:
                return "leap_start";
            case Engine::UNI_ANIMATION_STATE_LEAP_AIRBORNE:
                return "leap_airborne";
            case Engine::UNI_ANIMATION_STATE_LEAP_MELEE:
                return "leap_melee";
            case Engine::UNI_ANIMATION_STATE_UNKNOWN42:
                return "unknown42";
            case Engine::UNI_ANIMATION_STATE_BERSERK:
                return "berserk";
            case Engine::UNI_ANIMATION_STATE_YELO_SEAT_BOARDING:
                return "yelo_seat_boarding";
            case Engine::UNI_ANIMATION_STATE_YELO_SEAT_EJECTING:
                return "yelo_seat_ejecting";
            case Engine::UNI_ANIMATION_STATE_YELO_MOUNTING:
                return "yelo_mounting";
            case Engine::UNI_ANIMATION_STATE_YELO_TRANSFORMING:
                return "yelo_transforming";
            default:
                return "unknown";
        }
    }

    Engine::UnitAnimationState unit_animation_state_from_string(const std::string &state) {
        if(state == "invalid") {
            return Engine::UNI_ANIMATION_STATE_INVALID;
        }
        else if(state == "idle") {
            return Engine::UNI_ANIMATION_STATE_IDLE;
        }
        else if(state == "gesture") {
            return Engine::UNI_ANIMATION_STATE_GESTURE;
        }
        else if(state == "turn_left") {
            return Engine::UNI_ANIMATION_STATE_TURN_LEFT;
        }
        else if(state == "turn_right") {
            return Engine::UNI_ANIMATION_STATE_TURN_RIGHT;
        }
        else if(state == "move_front") {
            return Engine::UNI_ANIMATION_STATE_MOVE_FRONT;
        }
        else if(state == "move_back") {
            return Engine::UNI_ANIMATION_STATE_MOVE_BACK;
        }
        else if(state == "move_left") {
            return Engine::UNI_ANIMATION_STATE_MOVE_LEFT;
        }
        else if(state == "move_right") {
            return Engine::UNI_ANIMATION_STATE_MOVE_RIGHT;
        }
        else if(state == "stunned_front") {
            return Engine::UNI_ANIMATION_STATE_STUNNED_FRONT;
        }
        else if(state == "stunned_back") {
            return Engine::UNI_ANIMATION_STATE_STUNNED_BACK;
        }
        else if(state == "stunned_left") {
            return Engine::UNI_ANIMATION_STATE_STUNNED_LEFT;
        }
        else if(state == "stunned_right") {
            return Engine::UNI_ANIMATION_STATE_STUNNED_RIGHT;
        }
        else if(state == "slide_front") {
            return Engine::UNI_ANIMATION_STATE_SLIDE_FRONT;
        }
        else if(state == "slide_back") {
            return Engine::UNI_ANIMATION_STATE_SLIDE_BACK;
        }
        else if(state == "slide_left") {
            return Engine::UNI_ANIMATION_STATE_SLIDE_LEFT;
        }
        else if(state == "slide_right") {
            return Engine::UNI_ANIMATION_STATE_SLIDE_RIGHT;
        }
        else if(state == "ready") {
            return Engine::UNI_ANIMATION_STATE_READY;
        }
        else if(state == "put_away") {
            return Engine::UNI_ANIMATION_STATE_PUT_AWAY;
        }
        else if(state == "aim_still") {
            return Engine::UNI_ANIMATION_STATE_AIM_STILL;
        }
        else if(state == "aim_move") {
            return Engine::UNI_ANIMATION_STATE_AIM_MOVE;
        }
        else if(state == "airborne") {
            return Engine::UNI_ANIMATION_STATE_AIRBORNE;
        }
        else if(state == "land_soft") {
            return Engine::UNI_ANIMATION_STATE_LAND_SOFT;
        }
        else if(state == "land_hard") {
            return Engine::UNI_ANIMATION_STATE_LAND_HARD;
        }
        else if(state == "unknown23") {
            return Engine::UNI_ANIMATION_STATE_UNKNOWN23;
        }
        else if(state == "airborne_dead") {
            return Engine::UNI_ANIMATION_STATE_AIRBORNE_DEAD;
        }
        else if(state == "landing_dead") {
            return Engine::UNI_ANIMATION_STATE_LANDING_DEAD;
        }
        else if(state == "seat_enter") {
            return Engine::UNI_ANIMATION_STATE_SEAT_ENTER;
        }
        else if(state == "seat_exit") {
            return Engine::UNI_ANIMATION_STATE_SEAT_EXIT;
        }
        else if(state == "custom_animation") {
            return Engine::UNI_ANIMATION_STATE_CUSTOM_ANIMATION;
        }
        else if(state == "impulse") {
            return Engine::UNI_ANIMATION_STATE_IMPULSE;
        }
        else if(state == "melee") {
            return Engine::UNI_ANIMATION_STATE_MELEE;
        }
        else if(state == "melee_airborne") {
            return Engine::UNI_ANIMATION_STATE_MELEE_AIRBORNE;
        }
        else if(state == "melee_continuous") {
            return Engine::UNI_ANIMATION_STATE_MELEE_CONTINUOUS;
        }
        else if(state == "throw_grenade") {
            return Engine::UNI_ANIMATION_STATE_THROW_GRENADE;
        }
        else if(state == "ressurect_front") {
            return Engine::UNI_ANIMATION_STATE_RESSURECT_FRONT;
        }
        else if(state == "ressurect_back") {
            return Engine::UNI_ANIMATION_STATE_RESSURECT_BACK;
        }
        else if(state == "feeding") {
            return Engine::UNI_ANIMATION_STATE_FEEDING;
        }
        else if(state == "surprise_front") {
            return Engine::UNI_ANIMATION_STATE_SURPRISE_FRONT;
        }
        else if(state == "surprise_back") {
            return Engine::UNI_ANIMATION_STATE_SURPRISE_BACK;
        }
        else if(state == "leap_start") {
            return Engine::UNI_ANIMATION_STATE_LEAP_START;
        }
        else if(state == "leap_airborne") {
            return Engine::UNI_ANIMATION_STATE_LEAP_AIRBORNE;
        }
        else if(state == "leap_melee") {
            return Engine::UNI_ANIMATION_STATE_LEAP_MELEE;
        }
        else if(state == "unknown42") {
            return Engine::UNI_ANIMATION_STATE_UNKNOWN42;
        }
        else if(state == "berserk") {
            return Engine::UNI_ANIMATION_STATE_BERSERK;
        }
        else if(state == "yelo_seat_boarding") {
            return Engine::UNI_ANIMATION_STATE_YELO_SEAT_BOARDING;
        }
        else if(state == "yelo_seat_ejecting") {
            return Engine::UNI_ANIMATION_STATE_YELO_SEAT_EJECTING;
        }
        else if(state == "yelo_mounting") {
            return Engine::UNI_ANIMATION_STATE_YELO_MOUNTING;
        }
        else if(state == "yelo_transforming") {
            return Engine::UNI_ANIMATION_STATE_YELO_TRANSFORMING;
        }
        else {
            throw std::runtime_error("Invalid unit animation state.");
        }
    }

    std::string unit_replacement_animation_state_to_string(Engine::UnitReplacementAnimationState state) {
        switch(state) {
            case Engine::UNIT_REPLACEMENT_ANIMATION_STATE_NONE:
                return "none";
            case Engine::UNIT_REPLACEMENT_ANIMATION_STATE_DISARM:
                return "disarm";
            case Engine::UNIT_REPLACEMENT_ANIMATION_STATE_WEAPON_DROP:
                return "weapon_drop";
            case Engine::UNIT_REPLACEMENT_ANIMATION_STATE_WEAPON_READY:
                return "weapon_ready";
            case Engine::UNIT_REPLACEMENT_ANIMATION_STATE_WEAPON_PUT_AWAY:
                return "weapon_put_away";
            case Engine::UNIT_REPLACEMENT_ANIMATION_STATE_WEAPON_RELOAD1:
                return "weapon_reload1";
            case Engine::UNIT_REPLACEMENT_ANIMATION_STATE_WEAPON_RELOAD2:
                return "weapon_reload2";
            case Engine::UNIT_REPLACEMENT_ANIMATION_STATE_MELEE:
                return "melee";
            case Engine::UNIT_REPLACEMENT_ANIMATION_STATE_THROW_GRENADE:
                return "throw_grenade";
            default:
                return "unknown";
        }
    }

    Engine::UnitReplacementAnimationState unit_replacement_animation_state_from_string(const std::string &state) {
        if(state == "none") {
            return Engine::UNIT_REPLACEMENT_ANIMATION_STATE_NONE;
        }
        else if(state == "disarm") {
            return Engine::UNIT_REPLACEMENT_ANIMATION_STATE_DISARM;
        }
        else if(state == "weapon_drop") {
            return Engine::UNIT_REPLACEMENT_ANIMATION_STATE_WEAPON_DROP;
        }
        else if(state == "weapon_ready") {
            return Engine::UNIT_REPLACEMENT_ANIMATION_STATE_WEAPON_READY;
        }
        else if(state == "weapon_put_away") {
            return Engine::UNIT_REPLACEMENT_ANIMATION_STATE_WEAPON_PUT_AWAY;
        }
        else if(state == "weapon_reload1") {
            return Engine::UNIT_REPLACEMENT_ANIMATION_STATE_WEAPON_RELOAD1;
        }
        else if(state == "weapon_reload2") {
            return Engine::UNIT_REPLACEMENT_ANIMATION_STATE_WEAPON_RELOAD2;
        }
        else if(state == "melee") {
            return Engine::UNIT_REPLACEMENT_ANIMATION_STATE_MELEE;
        }
        else if(state == "throw_grenade") {
            return Engine::UNIT_REPLACEMENT_ANIMATION_STATE_THROW_GRENADE;
        }
        else {
            throw std::runtime_error("Invalid unit replacement animation state.");
        }
    }

    std::string unit_overlay_animation_state_to_string(Engine::UnitOverlayAnimationState state) {
        switch(state) {
            case Engine::UNIT_OVERLAY_ANIMATION_STATE_NONE:
                return "none";
            case Engine::UNIT_OVERLAY_ANIMATION_STATE_FIRE1:
                return "fire1";
            case Engine::UNIT_OVERLAY_ANIMATION_STATE_FIRE2:
                return "fire2";
            case Engine::UNIT_OVERLAY_ANIMATION_STATE_CHARGED1:
                return "charged1";
            case Engine::UNIT_OVERLAY_ANIMATION_STATE_CHARGED2:
                return "charged2";
            case Engine::UNIT_OVERLAY_ANIMATION_STATE_CHAMBER1:
                return "chamber1";
            case Engine::UNIT_OVERLAY_ANIMATION_STATE_CHAMBER2:
                return "chamber2";
            default:
                return "unknown";
        }
    }

    Engine::UnitOverlayAnimationState unit_overlay_animation_state_from_string(const std::string &state) {
        if(state == "none") {
            return Engine::UNIT_OVERLAY_ANIMATION_STATE_NONE;
        }
        else if(state == "fire1") {
            return Engine::UNIT_OVERLAY_ANIMATION_STATE_FIRE1;
        }
        else if(state == "fire2") {
            return Engine::UNIT_OVERLAY_ANIMATION_STATE_FIRE2;
        }
        else if(state == "charged1") {
            return Engine::UNIT_OVERLAY_ANIMATION_STATE_CHARGED1;
        }
        else if(state == "charged2") {
            return Engine::UNIT_OVERLAY_ANIMATION_STATE_CHARGED2;
        }
        else if(state == "chamber1") {
            return Engine::UNIT_OVERLAY_ANIMATION_STATE_CHAMBER1;
        }
        else if(state == "chamber2") {
            return Engine::UNIT_OVERLAY_ANIMATION_STATE_CHAMBER2;
        }
        else {
            throw std::runtime_error("Invalid unit overlay animation state.");
        }
    }

    std::string unit_base_seat_to_string(Engine::UnitBaseSeat seat) {
        switch(seat) {
            case Engine::UNIT_BASE_SEAT_ASLEEP:
                return "asleep";
            case Engine::UNIT_BASE_SEAT_ALERT:
                return "alert";
            case Engine::UNIT_BASE_SEAT_STAND:
                return "stand";
            case Engine::UNIT_BASE_SEAT_CROUCH:
                return "crouch";
            case Engine::UNIT_BASE_SEAT_FLEE:
                return "flee";
            case Engine::UNIT_BASE_SEAT_FLAMING:
                return "flaming";
            default:
                return "unknown";
        }
    }

    Engine::UnitBaseSeat unit_base_seat_from_string(const std::string &seat) {
        if(seat == "asleep") {
            return Engine::UNIT_BASE_SEAT_ASLEEP;
        }
        else if(seat == "alert") {
            return Engine::UNIT_BASE_SEAT_ALERT;
        }
        else if(seat == "stand") {
            return Engine::UNIT_BASE_SEAT_STAND;
        }
        else if(seat == "crouch") {
            return Engine::UNIT_BASE_SEAT_CROUCH;
        }
        else if(seat == "flee") {
            return Engine::UNIT_BASE_SEAT_FLEE;
        }
        else if(seat == "flaming") {
            return Engine::UNIT_BASE_SEAT_FLAMING;
        }
        else {
            throw std::runtime_error("Invalid unit base seat.");
        }
    }

    std::string unit_speech_priority_to_string(Engine::UnitSpeechPriority priority) {
        switch(priority) {
            case Engine::UNIT_SPEECH_PRIORITY_NONE:
                return "none";
            case Engine::UNIT_SPEECH_PRIORITY_IDLE:
                return "idle";
            case Engine::UNIT_SPEECH_PRIORITY_PAIN:
                return "pain";
            case Engine::UNIT_SPEECH_PRIORITY_TALK:
                return "talk";
            case Engine::UNIT_SPEECH_PRIORITY_COMMUNICATE:
                return "communicate";
            case Engine::UNIT_SPEECH_PRIORITY_SHOUT:
                return "shout";
            case Engine::UNIT_SPEECH_PRIORITY_SCRIPT:
                return "script";
            case Engine::UNIT_SPEECH_PRIORITY_INVOLUNTARY:
                return "involuntary";
            case Engine::UNIT_SPEECH_PRIORITY_EXLAIM:
                return "exclaim";
            case Engine::UNIT_SPEECH_PRIORITY_SCREAM:
                return "scream";
            case Engine::UNIT_SPEECH_PRIORITY_DEATH:
                return "death";
            default:
                return "unknown";
        }
    }

    Engine::UnitSpeechPriority unit_speech_priority_from_string(const std::string &priority) {
        if(priority == "none") {
            return Engine::UNIT_SPEECH_PRIORITY_NONE;
        }
        else if(priority == "idle") {
            return Engine::UNIT_SPEECH_PRIORITY_IDLE;
        }
        else if(priority == "pain") {
            return Engine::UNIT_SPEECH_PRIORITY_PAIN;
        }
        else if(priority == "talk") {
            return Engine::UNIT_SPEECH_PRIORITY_TALK;
        }
        else if(priority == "communicate") {
            return Engine::UNIT_SPEECH_PRIORITY_COMMUNICATE;
        }
        else if(priority == "shout") {
            return Engine::UNIT_SPEECH_PRIORITY_SHOUT;
        }
        else if(priority == "script") {
            return Engine::UNIT_SPEECH_PRIORITY_SCRIPT;
        }
        else if(priority == "involuntary") {
            return Engine::UNIT_SPEECH_PRIORITY_INVOLUNTARY;
        }
        else if(priority == "exclaim") {
            return Engine::UNIT_SPEECH_PRIORITY_EXLAIM;
        }
        else if(priority == "scream") {
            return Engine::UNIT_SPEECH_PRIORITY_SCREAM;
        }
        else if(priority == "death") {
            return Engine::UNIT_SPEECH_PRIORITY_DEATH;
        }
        else {
            throw std::runtime_error("Invalid unit speech priority.");
        }
    }

    std::string unit_scream_type_to_string(Engine::UnitScreamType type) {
        switch(type) {
            case Engine::UNIT_SCREAM_TYPE_FEAR:
                return "fear";
            case Engine::UNIT_SCREAM_TYPE_ENEMY_GRENADE:
                return "enemy_grenade";
            case Engine::UNIT_SCREAM_TYPE_PAIN:
                return "pain";
            case Engine::UNIT_SCREAM_TYPE_MAIMED_LIMB:
                return "maimed_limb";
            case Engine::UNIT_SCREAM_TYPE_MAIMED_HEAD:
                return "maimed_head";
            case Engine::UNIT_SCREAM_TYPE_RESSURECTION:
                return "ressurection";
            default:
                return "unknown";
        }
    }

    Engine::UnitScreamType unit_scream_type_from_string(const std::string &type) {
        if(type == "fear") {
            return Engine::UNIT_SCREAM_TYPE_FEAR;
        }
        else if(type == "enemy_grenade") {
            return Engine::UNIT_SCREAM_TYPE_ENEMY_GRENADE;
        }
        else if(type == "pain") {
            return Engine::UNIT_SCREAM_TYPE_PAIN;
        }
        else if(type == "maimed_limb") {
            return Engine::UNIT_SCREAM_TYPE_MAIMED_LIMB;
        }
        else if(type == "maimed_head") {
            return Engine::UNIT_SCREAM_TYPE_MAIMED_HEAD;
        }
        else if(type == "ressurection") {
            return Engine::UNIT_SCREAM_TYPE_RESSURECTION;
        }
        else {
            throw std::runtime_error("Invalid unit scream type.");
        }
    }

    std::string ai_communication_type_to_string(Engine::AiCommunicationType type) {
        switch(type) {
            case Engine::AI_COMMUNICATION_TYPE_DEATH:
                return "death";
            case Engine::AI_COMMUNICATION_TYPE_SPREE:
                return "spree";
            case Engine::AI_COMMUNICATION_TYPE_HURT:
                return "hurt";
            case Engine::AI_COMMUNICATION_TYPE_DAMAGE:
                return "damage";
            case Engine::AI_COMMUNICATION_TYPE_SIGHTED_ENEMY:
                return "sighted_enemy";
            case Engine::AI_COMMUNICATION_TYPE_FOUND_ENEMY:
                return "found_enemy";
            case Engine::AI_COMMUNICATION_TYPE_UNEXPECTED_ENEMY:
                return "unexpected_enemy";
            case Engine::AI_COMMUNICATION_TYPE_FOUND_DEAD_FRIEND:
                return "found_dead_friend";
            case Engine::AI_COMMUNICATION_TYPE_ALLEGIANCE_CHANGED:
                return "allegiance_changed";
            case Engine::AI_COMMUNICATION_TYPE_GRENADE_THROWING:
                return "grenade_throwing";
            case Engine::AI_COMMUNICATION_TYPE_GRENADE_STARTLE:
                return "grenade_startle";
            case Engine::AI_COMMUNICATION_TYPE_GRENADE_SIGHTED:
                return "grenade_sighted";
            case Engine::AI_COMMUNICATION_TYPE_GRENADE_DANGER:
                return "grenade_danger";
            case Engine::AI_COMMUNICATION_TYPE_LOST_CONTACT:
                return "lost_contact";
            case Engine::AI_COMMUNICATION_TYPE_BLOCKED:
                return "blocked";
            case Engine::AI_COMMUNICATION_TYPE_ALERT_NONCOMBAT:
                return "alert_noncombat";
            case Engine::AI_COMMUNICATION_TYPE_SEARCH_START:
                return "search_start";
            case Engine::AI_COMMUNICATION_TYPE_SEARCH_QUERY:
                return "search_query";
            case Engine::AI_COMMUNICATION_TYPE_SEARCH_REPORT:
                return "search_report";
            case Engine::AI_COMMUNICATION_TYPE_SEARCH_ABANDON:
                return "search_abandon";
            case Engine::AI_COMMUNICATION_TYPE_SEARCH_GROUP_ABANDON:
                return "search_group_abandon";
            case Engine::AI_COMMUNICATION_TYPE_UNCOVER_START:
                return "uncover_start";
            case Engine::AI_COMMUNICATION_TYPE_ADVANCE:
                return "advance";
            case Engine::AI_COMMUNICATION_TYPE_RETREAT:
                return "retreat";
            case Engine::AI_COMMUNICATION_TYPE_COVER:
                return "cover";
            case Engine::AI_COMMUNICATION_TYPE_SIGHTED_FRIEND_PLAYER:
                return "sighted_friend_player";
            case Engine::AI_COMMUNICATION_TYPE_SHOOTING:
                return "shooting";
            case Engine::AI_COMMUNICATION_TYPE_SHOOTING_VEHICLE:
                return "shooting_vehicle";
            case Engine::AI_COMMUNICATION_TYPE_SHOOTING_BERSERK:
                return "shooting_berserk";
            case Engine::AI_COMMUNICATION_TYPE_SHOOTING_GROUP:
                return "shooting_group";
            case Engine::AI_COMMUNICATION_TYPE_SHOOTING_TRAITOR:
                return "shooting_traitor";
            case Engine::AI_COMMUNICATION_TYPE_FLEE:
                return "flee";
            case Engine::AI_COMMUNICATION_TYPE_FLEE_LEADER_DIED:
                return "flee_leader_died";
            case Engine::AI_COMMUNICATION_TYPE_FLEE_IDLE:
                return "flee_idle";
            case Engine::AI_COMMUNICATION_TYPE_ATTEMPTED_FLEE:
                return "attempted_flee";
            case Engine::AI_COMMUNICATION_TYPE_HIDING_FINISHED:
                return "hiding_finished";
            case Engine::AI_COMMUNICATION_TYPE_VEHICLE_ENTRY:
                return "vehicle_entry";
            case Engine::AI_COMMUNICATION_TYPE_VEHICLE_EXIT:
                return "vehicle_exit";
            case Engine::AI_COMMUNICATION_TYPE_VEHICLE_WOOHOO:
                return "vehicle_woohoo";
            case Engine::AI_COMMUNICATION_TYPE_VEHICLE_SCARED:
                return "vehicle_scared";
            case Engine::AI_COMMUNICATION_TYPE_VEHICLE_FALLING:
                return "vehicle_falling";
            case Engine::AI_COMMUNICATION_TYPE_SURPRISE:
                return "surprise";
            case Engine::AI_COMMUNICATION_TYPE_BERSERK:
                return "berserk";
            case Engine::AI_COMMUNICATION_TYPE_MELEE:
                return "melee";
            case Engine::AI_COMMUNICATION_TYPE_DIVE:
                return "dive";
            case Engine::AI_COMMUNICATION_TYPE_UNCONVER_EXCLAMATION:
                return "uncover_exclamation";
            case Engine::AI_COMMUNICATION_TYPE_FALLING:
                return "falling";
            case Engine::AI_COMMUNICATION_TYPE_LEAP:
                return "leap";
            case Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_ALONE:
                return "postcombat_alone";
            case Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_UNSCATHED:
                return "postcombat_unscathed";
            case Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_WOUNDED:
                return "postcombat_wounded";
            case Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_MASSACRE:
                return "postcombat_massacre";
            case Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_TRIUMPH:
                return "postcombat_triumph";
            case Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_CHECK_ENEMY:
                return "postcombat_check_enemy";
            case Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_CHECK_FRIEND:
                return "postcombat_check_friend";
            case Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_SHOOT_CORPSE:
                return "postcombat_shoot_corpse";
            case Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_CELEBRATE:
                return "postcombat_celebrate";
            default:
                return "unknown";
        }
    }

    Engine::AiCommunicationType ai_communication_type_from_string(const std::string &type) {
        if(type == "death") {
            return Engine::AI_COMMUNICATION_TYPE_DEATH;
        }
        else if(type == "spree") {
            return Engine::AI_COMMUNICATION_TYPE_SPREE;
        }
        else if(type == "hurt") {
            return Engine::AI_COMMUNICATION_TYPE_HURT;
        }
        else if(type == "damage") {
            return Engine::AI_COMMUNICATION_TYPE_DAMAGE;
        }
        else if(type == "sighted_enemy") {
            return Engine::AI_COMMUNICATION_TYPE_SIGHTED_ENEMY;
        }
        else if(type == "found_enemy") {
            return Engine::AI_COMMUNICATION_TYPE_FOUND_ENEMY;
        }
        else if(type == "unexpected_enemy") {
            return Engine::AI_COMMUNICATION_TYPE_UNEXPECTED_ENEMY;
        }
        else if(type == "found_dead_friend") {
            return Engine::AI_COMMUNICATION_TYPE_FOUND_DEAD_FRIEND;
        }
        else if(type == "allegiance_changed") {
            return Engine::AI_COMMUNICATION_TYPE_ALLEGIANCE_CHANGED;
        }
        else if(type == "grenade_throwing") {
            return Engine::AI_COMMUNICATION_TYPE_GRENADE_THROWING;
        }
        else if(type == "grenade_startle") {
            return Engine::AI_COMMUNICATION_TYPE_GRENADE_STARTLE;
        }
        else if(type == "grenade_sighted") {
            return Engine::AI_COMMUNICATION_TYPE_GRENADE_SIGHTED;
        }
        else if(type == "grenade_danger") {
            return Engine::AI_COMMUNICATION_TYPE_GRENADE_DANGER;
        }
        else if(type == "lost_contact") {
            return Engine::AI_COMMUNICATION_TYPE_LOST_CONTACT;
        }
        else if(type == "blocked") {
            return Engine::AI_COMMUNICATION_TYPE_BLOCKED;
        }
        else if(type == "alert_noncombat") {
            return Engine::AI_COMMUNICATION_TYPE_ALERT_NONCOMBAT;
        }
        else if(type == "search_start") {
            return Engine::AI_COMMUNICATION_TYPE_SEARCH_START;
        }
        else if(type == "search_query") {
            return Engine::AI_COMMUNICATION_TYPE_SEARCH_QUERY;
        }
        else if(type == "search_report") {
            return Engine::AI_COMMUNICATION_TYPE_SEARCH_REPORT;
        }
        else if(type == "search_abandon") {
            return Engine::AI_COMMUNICATION_TYPE_SEARCH_ABANDON;
        }
        else if(type == "search_group_abandon") {
            return Engine::AI_COMMUNICATION_TYPE_SEARCH_GROUP_ABANDON;
        }
        else if(type == "uncover_start") {
            return Engine::AI_COMMUNICATION_TYPE_UNCOVER_START;
        }
        else if(type == "advance") {
            return Engine::AI_COMMUNICATION_TYPE_ADVANCE;
        }
        else if(type == "retreat") {
            return Engine::AI_COMMUNICATION_TYPE_RETREAT;
        }
        else if(type == "cover") {
            return Engine::AI_COMMUNICATION_TYPE_COVER;
        }
        else if(type == "sighted_friend_player") {
            return Engine::AI_COMMUNICATION_TYPE_SIGHTED_FRIEND_PLAYER;
        }
        else if(type == "shooting") {
            return Engine::AI_COMMUNICATION_TYPE_SHOOTING;
        }
        else if(type == "shooting_vehicle") {
            return Engine::AI_COMMUNICATION_TYPE_SHOOTING_VEHICLE;
        }
        else if(type == "shooting_berserk") {
            return Engine::AI_COMMUNICATION_TYPE_SHOOTING_BERSERK;
        }
        else if(type == "shooting_group") {
            return Engine::AI_COMMUNICATION_TYPE_SHOOTING_GROUP;
        }
        else if(type == "shooting_traitor") {
            return Engine::AI_COMMUNICATION_TYPE_SHOOTING_TRAITOR;
        }
        else if(type == "flee") {
            return Engine::AI_COMMUNICATION_TYPE_FLEE;
        }
        else if(type == "flee_leader_died") {
            return Engine::AI_COMMUNICATION_TYPE_FLEE_LEADER_DIED;
        }
        else if(type == "flee_idle") {
            return Engine::AI_COMMUNICATION_TYPE_FLEE_IDLE;
        }
        else if(type == "attempted_flee") {
            return Engine::AI_COMMUNICATION_TYPE_ATTEMPTED_FLEE;
        }
        else if(type == "hiding_finished") {
            return Engine::AI_COMMUNICATION_TYPE_HIDING_FINISHED;
        }
        else if(type == "vehicle_entry") {
            return Engine::AI_COMMUNICATION_TYPE_VEHICLE_ENTRY;
        }
        else if(type == "vehicle_exit") {
            return Engine::AI_COMMUNICATION_TYPE_VEHICLE_EXIT;
        }
        else if(type == "vehicle_woohoo") {
            return Engine::AI_COMMUNICATION_TYPE_VEHICLE_WOOHOO;
        }
        else if(type == "vehicle_scared") {
            return Engine::AI_COMMUNICATION_TYPE_VEHICLE_SCARED;
        }
        else if(type == "vehicle_falling") {
            return Engine::AI_COMMUNICATION_TYPE_VEHICLE_FALLING;
        }
        else if(type == "surprise") {
            return Engine::AI_COMMUNICATION_TYPE_SURPRISE;
        }
        else if(type == "berserk") {
            return Engine::AI_COMMUNICATION_TYPE_BERSERK;
        }
        else if(type == "melee") {
            return Engine::AI_COMMUNICATION_TYPE_MELEE;
        }
        else if(type == "dive") {
            return Engine::AI_COMMUNICATION_TYPE_DIVE;
        }
        else if(type == "uncover_exclamation") {
            return Engine::AI_COMMUNICATION_TYPE_UNCONVER_EXCLAMATION;
        }
        else if(type == "falling") {
            return Engine::AI_COMMUNICATION_TYPE_FALLING;
        }
        else if(type == "leap") {
            return Engine::AI_COMMUNICATION_TYPE_LEAP;
        }
        else if(type == "postcombat_alone") {
            return Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_ALONE;
        }
        else if(type == "postcombat_unscathed") {
            return Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_UNSCATHED;
        }
        else if(type == "postcombat_wounded") {
            return Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_WOUNDED;
        }
        else if(type == "postcombat_massacre") {
            return Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_MASSACRE;
        }
        else if(type == "postcombat_triumph") {
            return Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_TRIUMPH;
        }
        else if(type == "postcombat_check_enemy") {
            return Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_CHECK_ENEMY;
        }
        else if(type == "postcombat_check_friend") {
            return Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_CHECK_FRIEND;
        }
        else if(type == "postcombat_shoot_corpse") {
            return Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_SHOOT_CORPSE;
        }
        else if(type == "postcombat_celebrate") {
            return Engine::AI_COMMUNICATION_TYPE_POSTCOMBAT_CELEBRATE;
        }
        else {
            throw std::runtime_error("Invalid ai communication type.");
        }
    }

    std::string biped_movement_state_to_string(Engine::BipedMovementState state) {
        switch(state) {
            case Engine::BIPED_MOVEMENT_STATE_MOVING:
                return "moving";
            case Engine::BIPED_MOVEMENT_STATE_IDLE:
                return "idle";
            case Engine::BIPED_MOVEMENT_STATE_GESTURING:
                return "gesturing";
            default:
                return "unknown";
        }
    }

    Engine::BipedMovementState biped_movement_state_from_string(const std::string &state) {
        if(state == "moving") {
            return Engine::BIPED_MOVEMENT_STATE_MOVING;
        }
        else if(state == "idle") {
            return Engine::BIPED_MOVEMENT_STATE_IDLE;
        }
        else if(state == "gesturing") {
            return Engine::BIPED_MOVEMENT_STATE_GESTURING;
        }
        else {
            throw std::runtime_error("Invalid biped movement state.");
        }
    }

    std::string weapon_state_to_string(Engine::WeaponState state) {
        switch(state) {
            case Engine::WEAPON_STATE_IDLE:
                return "idle";
            case Engine::WEAPON_STATE_FIRE1:
                return "fire1";
            case Engine::WEAPON_STATE_FIRE2:
                return "fire2";
            case Engine::WEAPON_STATE_CHAMBER1:
                return "chamber1";
            case Engine::WEAPON_STATE_CHAMBER2:
                return "chamber2";
            case Engine::WEAPON_STATE_RELOAD1:
                return "reload1";
            case Engine::WEAPON_STATE_RELOAD2:
                return "reload2";
            case Engine::WEAPON_STATE_CHARGED1:
                return "charged1";
            case Engine::WEAPON_STATE_CHARGED2:
                return "charged2";
            case Engine::WEAPON_STATE_READY:
                return "ready";
            case Engine::WEAPON_STATE_PUT_AWAY:
                return "put_away";
            default:
                return "unknown";
        }
    }

    Engine::WeaponState weapon_state_from_string(const std::string &state) {
        if(state == "idle") {
            return Engine::WEAPON_STATE_IDLE;
        }
        else if(state == "fire1") {
            return Engine::WEAPON_STATE_FIRE1;
        }
        else if(state == "fire2") {
            return Engine::WEAPON_STATE_FIRE2;
        }
        else if(state == "chamber1") {
            return Engine::WEAPON_STATE_CHAMBER1;
        }
        else if(state == "chamber2") {
            return Engine::WEAPON_STATE_CHAMBER2;
        }
        else if(state == "reload1") {
            return Engine::WEAPON_STATE_RELOAD1;
        }
        else if(state == "reload2") {
            return Engine::WEAPON_STATE_RELOAD2;
        }
        else if(state == "charged1") {
            return Engine::WEAPON_STATE_CHARGED1;
        }
        else if(state == "charged2") {
            return Engine::WEAPON_STATE_CHARGED2;
        }
        else if(state == "ready") {
            return Engine::WEAPON_STATE_READY;
        }
        else if(state == "put_away") {
            return Engine::WEAPON_STATE_PUT_AWAY;
        }
        else {
            throw std::runtime_error("Invalid weapon state.");
        }
    }

    std::string weapon_magazine_state_to_string(Engine::WeaponMagazineState state) {
        switch(state) {
            case Engine::WEAPON_MAGAZINE_STATE_IDLE:
                return "idle";
            case Engine::WEAPON_MAGAZINE_STATE_CHAMBERING_START:
                return "chambering_start";
            case Engine::WEAPON_MAGAZINE_STATE_CHAMBERING_FINISH:
                return "chambering_finish";
            case Engine::WEAPON_MAGAZINE_STATE_CHAMBERING:
                return "chambering";
            default:
                return "unknown";
        }
    }

    Engine::WeaponMagazineState weapon_magazine_state_from_string(const std::string &state) {
        if(state == "idle") {
            return Engine::WEAPON_MAGAZINE_STATE_IDLE;
        }
        else if(state == "chambering_start") {
            return Engine::WEAPON_MAGAZINE_STATE_CHAMBERING_START;
        }
        else if(state == "chambering_finish") {
            return Engine::WEAPON_MAGAZINE_STATE_CHAMBERING_FINISH;
        }
        else if(state == "chambering") {
            return Engine::WEAPON_MAGAZINE_STATE_CHAMBERING;
        }
        else {
            throw std::runtime_error("Invalid weapon magazine state.");
        }
    }

    std::string network_color_to_string(Engine::NetworkPlayerColor color) {
        switch(color) {
            case Engine::NETWORK_COLOR_WHITE:
                return "white";
            case Engine::NETWORK_COLOR_BLACK:
                return "black";
            case Engine::NETWORK_COLOR_RED:
                return "red";
            case Engine::NETWORK_COLOR_BLUE:
                return "blue";
            case Engine::NETWORK_COLOR_GRAY:
                return "gray";
            case Engine::NETWORK_COLOR_YELLOW:
                return "yellow";
            case Engine::NETWORK_COLOR_GREEN:
                return "green";
            case Engine::NETWORK_COLOR_PINK:
                return "pink";
            case Engine::NETWORK_COLOR_PURPLE:
                return "purple";
            case Engine::NETWORK_COLOR_CYAN:
                return "cyan";
            case Engine::NETWORK_COLOR_COBALT:
                return "cobalt";
            case Engine::NETWORK_COLOR_ORANGE:
                return "orange";
            case Engine::NETWORK_COLOR_TEAL:
                return "teal";
            case Engine::NETWORK_COLOR_SAGE:
                return "sage";
            case Engine::NETWORK_COLOR_BROWN:
                return "brown";
            case Engine::NETWORK_COLOR_TAN:
                return "tan";
            case Engine::NETWORK_COLOR_MAROON:
                return "maroon";
            case Engine::NETWORK_COLOR_SALMON:
                return "salmon";
            default:
                return "unknown";
        }
    }

    Engine::NetworkPlayerColor network_color_from_string(const std::string &color) {
        if(color == "white") {
            return Engine::NETWORK_COLOR_WHITE;
        }
        else if(color == "black") {
            return Engine::NETWORK_COLOR_BLACK;
        }
        else if(color == "red") {
            return Engine::NETWORK_COLOR_RED;
        }
        else if(color == "blue") {
            return Engine::NETWORK_COLOR_BLUE;
        }
        else if(color == "gray") {
            return Engine::NETWORK_COLOR_GRAY;
        }
        else if(color == "yellow") {
            return Engine::NETWORK_COLOR_YELLOW;
        }
        else if(color == "green") {
            return Engine::NETWORK_COLOR_GREEN;
        }
        else if(color == "pink") {
            return Engine::NETWORK_COLOR_PINK;
        }
        else if(color == "purple") {
            return Engine::NETWORK_COLOR_PURPLE;
        }
        else if(color == "cyan") {
            return Engine::NETWORK_COLOR_CYAN;
        }
        else if(color == "cobalt") {
            return Engine::NETWORK_COLOR_COBALT;
        }
        else if(color == "orange") {
            return Engine::NETWORK_COLOR_ORANGE;
        }
        else if(color == "teal") {
            return Engine::NETWORK_COLOR_TEAL;
        }
        else if(color == "sage") {
            return Engine::NETWORK_COLOR_SAGE;
        }
        else if(color == "brown") {
            return Engine::NETWORK_COLOR_BROWN;
        }
        else if(color == "tan") {
            return Engine::NETWORK_COLOR_TAN;
        }
        else if(color == "maroon") {
            return Engine::NETWORK_COLOR_MAROON;
        }
        else if(color == "salmon") {
            return Engine::NETWORK_COLOR_SALMON;
        }
        else {
            throw std::runtime_error("Invalid network color.");
        }
    }

    std::string player_objective_mode_to_string(Engine::PlayerObjectiveMode mode) {
        switch(mode) {
            case Engine::PLAYER_OBJECTIVE_MODE_RACE:
                return "race";
            case Engine::PLAYER_OBJECTIVE_MODE_HILL:
                return "hill";
            case Engine::PLAYER_OBJECTIVE_MODE_JUGGERNAUT:
                return "juggernaut";
            case Engine::PLAYER_OBJECTIVE_MODE_BALL:
                return "ball";
            case Engine::PLAYER_OBJECTIVE_MODE_NONE:
                return "none";
            default:
                return "unknown";
        }
    }

    Engine::PlayerObjectiveMode player_objective_mode_from_string(const std::string &objective) {
        if(objective == "race") {
            return Engine::PLAYER_OBJECTIVE_MODE_RACE;
        }
        else if(objective == "hill") {
            return Engine::PLAYER_OBJECTIVE_MODE_HILL;
        }
        else if(objective == "juggernaut") {
            return Engine::PLAYER_OBJECTIVE_MODE_JUGGERNAUT;
        }
        else if(objective == "ball") {
            return Engine::PLAYER_OBJECTIVE_MODE_BALL;
        }
        else if(objective == "none") {
            return Engine::PLAYER_OBJECTIVE_MODE_NONE;
        }
        else {
            throw std::runtime_error("Invalid player objective mode.");
        }
    }

    static int lua_engine_color_a_r_g_b_int__index(lua_State *state) {
        auto *color = lua_from_meta_object<Engine::ColorARGBInt>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "red") { 
            lua_pushinteger(state, color->red); 
            return 1; 
        }
        else if(field == "green") { 
            lua_pushinteger(state, color->green); 
            return 1; 
        }
        else if(field == "blue") { 
            lua_pushinteger(state, color->blue); 
            return 1; 
        }
        else if(field == "alpha") { 
            lua_pushinteger(state, color->alpha); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_color_a_r_g_b_int__newindex(lua_State *state) {
        auto *color = lua_from_meta_object<Engine::ColorARGBInt>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "red") { 
            color->red = static_cast<std::uint8_t>(luaL_checkinteger(state, 3)); 
        }
        else if(field == "green") { 
            color->green = static_cast<std::uint8_t>(luaL_checkinteger(state, 3)); 
        }
        else if(field == "blue") { 
            color->blue = static_cast<std::uint8_t>(luaL_checkinteger(state, 3)); 
        }
        else if(field == "alpha") { 
            color->alpha = static_cast<std::uint8_t>(luaL_checkinteger(state, 3)); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_color_a_r_g_b_int(lua_State *state, Engine::ColorARGBInt &color, bool read_only) noexcept {
        lua_push_meta_object(state, color, lua_engine_color_a_r_g_b_int__index, lua_engine_color_a_r_g_b_int__newindex, read_only); 
    }

    static int lua_engine_tag_dependency__index(lua_State *state) {
        auto *dependency = lua_from_meta_object<Engine::TagDependency>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "handle") { 
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&dependency->tag_handle));
            return 1; 
        }
        else if(field == "class") {
            auto tag_class = Engine::tag_class_to_string(*reinterpret_cast<Engine::TagClassInt *>(dependency->tag_fourcc)); 
            lua_pushstring(state, tag_class.c_str()); 
            return 1; 
        }
        else if(field == "path") { 
            char path[dependency->path_size + 1];
            std::memcpy(path, dependency->path, dependency->path_size);
            path[dependency->path_size] = '\0';
            lua_pushstring(state, path);
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_tag_dependency__newindex(lua_State *state) noexcept {
        auto *dependency = lua_from_meta_object<Engine::TagDependency>(state, 1); 
        auto *key = luaL_checkstring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "handle") { 
            dependency->tag_handle.handle = luaL_checkinteger(state, 3); 
        }
        else if(field == "class") {
            auto *tag_class = lua_tostring(state, 3);
            if(tag_class == nullptr) {
                return luaL_error(state, "Invalid tag class");
            }
            *reinterpret_cast<Engine::TagClassInt *>(dependency->tag_fourcc) = Engine::tag_class_from_string(tag_class);
        }
        else if(field == "path") { 
            auto *path = lua_tostring(state, 3);
            if(path == nullptr) {
                return luaL_error(state, "Invalid path");
            }
            if(dependency->path_size < std::strlen(path)) {
                return luaL_error(state, "Path is too long");
            }
            std::memcpy(dependency->path, path, std::strlen(path));
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_tag_dependency(lua_State *state, Engine::TagDependency &dependency, bool read_only) noexcept {
        lua_push_meta_object(state, dependency, lua_engine_tag_dependency__index, lua_engine_tag_dependency__newindex, read_only); 
    }

    static int lua_engine_point2_d__index(lua_State *state) noexcept {
        auto *point = lua_from_meta_object<Engine::Point2D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "x") { 
            lua_pushnumber(state, point->x); 
            return 1; 
        }
        else if(field == "y") { 
            lua_pushnumber(state, point->y); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_point2_d__newindex(lua_State *state) noexcept {
        auto *point = lua_from_meta_object<Engine::Point2D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "x") { 
            point->x = luaL_checknumber(state, 3); 
        }
        else if(field == "y") { 
            point->y = luaL_checknumber(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_point2_d(lua_State *state, Engine::Point2D &point, bool read_only) noexcept {
        lua_push_meta_object(state, point, lua_engine_point2_d__index, lua_engine_point2_d__newindex, read_only); 
    }

    static int lua_engine_point3_d__index(lua_State *state) noexcept {
        auto *point = lua_from_meta_object<Engine::Point3D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "x") { 
            lua_pushinteger(state, point->x); 
            return 1; 
        }
        else if(field == "y") { 
            lua_pushinteger(state, point->y); 
            return 1; 
        }
        else if(field == "z") { 
            lua_pushinteger(state, point->z); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_point3_d__newindex(lua_State *state) noexcept {
        auto *point = lua_from_meta_object<Engine::Point3D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "x") { 
            point->x = luaL_checkinteger(state, 3); 
        }
        else if(field == "y") { 
            point->y = luaL_checkinteger(state, 3); 
        }
        else if(field == "z") { 
            point->z = luaL_checkinteger(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_point3_d(lua_State *state, Engine::Point3D &point, bool read_only) noexcept {
        lua_push_meta_object(state, point, lua_engine_point3_d__index, lua_engine_point3_d__newindex, read_only); 
    }

    static int lua_engine_tag_data_offset__index(lua_State *state) noexcept {
        auto *offset = lua_from_meta_object<Engine::TagDataOffset>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "size") { 
            lua_pushinteger(state, offset->size); 
            return 1; 
        }
        else if(field == "external") { 
            lua_pushboolean(state, offset->external);
            return 1; 
        }
        else if(field == "fileOffset") { 
            auto file_offset = offset->file_offset;
            if(file_offset == 0) {
                lua_pushnil(state);
            }
            else {
                lua_pushinteger(state, file_offset);
            }
            return 1; 
        }
        else if(field == "pointer") {
            auto pointer = offset->pointer;
            if(pointer) {
                lua_pushinteger(state, reinterpret_cast<std::uintptr_t>(pointer));
            }
            else {
                lua_pushnil(state);
            }
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_tag_data_offset__newindex(lua_State *state) noexcept {
        auto *offset = lua_from_meta_object<Engine::TagDataOffset>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "size") { 
            offset->size = luaL_checkinteger(state, 3); 
        }
        else if(field == "external") { 
            offset->external = lua_toboolean(state, 3);
        }
        else if(field == "fileOffset") { 
            offset->file_offset = luaL_checkinteger(state, 3);
        }
        else if(field == "pointer") {
            offset->pointer = reinterpret_cast<std::byte *>(luaL_checkinteger(state, 3));
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_tag_data_offset(lua_State *state, Engine::TagDataOffset &offset, bool read_only) noexcept {
        lua_push_meta_object(state, offset, lua_engine_tag_data_offset__index, lua_engine_tag_data_offset__newindex, read_only); 
    }

    static int lua_engine_color_a_r_g_b__index(lua_State *state) {
        auto *color = lua_from_meta_object<Engine::ColorARGB>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "alpha") { 
            lua_pushnumber(state, color->alpha); 
            return 1; 
        }
        else if(field == "red") { 
            lua_pushnumber(state, color->red); 
            return 1; 
        }
        else if(field == "green") { 
            lua_pushnumber(state, color->green); 
            return 1; 
        }
        else if(field == "blue") { 
            lua_pushnumber(state, color->blue); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_color_a_r_g_b__newindex(lua_State *state) noexcept {
        auto *color = lua_from_meta_object<Engine::ColorARGB>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "alpha") { 
            color->alpha = luaL_checknumber(state, 3); 
        }
        else if(field == "red") { 
            color->red = luaL_checknumber(state, 3); 
        }
        else if(field == "green") { 
            color->green = luaL_checknumber(state, 3); 
        }
        else if(field == "blue") { 
            color->blue = luaL_checknumber(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_color_a_r_g_b(lua_State *state, Engine::ColorARGB &color, bool read_only) noexcept {
        lua_push_meta_object(state, color, lua_engine_color_a_r_g_b__index, lua_engine_color_a_r_g_b__newindex, read_only); 
    }

    static int lua_engine_rectangle2_d__index(lua_State *state) {
        auto *rectangle = lua_from_meta_object<Engine::Rectangle2D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "top") { 
            lua_pushinteger(state, rectangle->top); 
            return 1; 
        }
        else if(field == "left") { 
            lua_pushinteger(state, rectangle->left); 
            return 1; 
        }
        else if(field == "bottom") { 
            lua_pushinteger(state, rectangle->bottom); 
            return 1; 
        }
        else if(field == "right") { 
            lua_pushinteger(state, rectangle->right); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_rectangle2_d__newindex(lua_State *state) noexcept {
        auto *rectangle = lua_from_meta_object<Engine::Rectangle2D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "top") { 
            rectangle->top = luaL_checkinteger(state, 3); 
        }
        else if(field == "left") { 
            rectangle->left = luaL_checkinteger(state, 3); 
        }
        else if(field == "bottom") { 
            rectangle->bottom = luaL_checkinteger(state, 3); 
        }
        else if(field == "right") { 
            rectangle->right = luaL_checkinteger(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_rectangle2_d(lua_State *state, Engine::Rectangle2D &rectangle, bool read_only) noexcept {
        lua_push_meta_object(state, rectangle, lua_engine_rectangle2_d__index, lua_engine_rectangle2_d__newindex, read_only); 
    }

    static int lua_engine_rectangle2_d_f__index(lua_State *state) {
        auto *rectangle = lua_from_meta_object<Engine::Rectangle2DF>(state, 1); 
        auto *key = lua_tostring(state, 2); 
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "top") { 
            lua_pushnumber(state, rectangle->top); 
            return 1; 
        }
        else if(field == "left") { 
            lua_pushnumber(state, rectangle->left); 
            return 1; 
        }
        else if(field == "bottom") { 
            lua_pushnumber(state, rectangle->bottom); 
            return 1; 
        }
        else if(field == "right") { 
            lua_pushnumber(state, rectangle->right); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_rectangle2_d_f__newindex(lua_State *state) noexcept {
        auto *rectangle = lua_from_meta_object<Engine::Rectangle2DF>(state, 1); 
        auto *key = lua_tostring(state, 2); 
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "top") { 
            rectangle->top = luaL_checknumber(state, 3); 
        }
        else if(field == "left") { 
            rectangle->left = luaL_checknumber(state, 3); 
        }
        else if(field == "bottom") { 
            rectangle->bottom = luaL_checknumber(state, 3); 
        }
        else if(field == "right") { 
            rectangle->right = luaL_checknumber(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_rectangle2_d_f(lua_State *state, Engine::Rectangle2DF &rectangle, bool read_only) noexcept {
        lua_push_meta_object(state, rectangle, lua_engine_rectangle2_d_f__index, lua_engine_rectangle2_d_f__newindex, read_only); 
    }

    static int lua_engine_point2_d_int__index(lua_State *state) {
        auto *point = lua_from_meta_object<Engine::Point2DInt>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "x") { 
            lua_pushinteger(state, point->x); 
            return 1; 
        }
        else if(field == "y") { 
            lua_pushinteger(state, point->y); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_point2_d_int__newindex(lua_State *state) noexcept {
        auto *point = lua_from_meta_object<Engine::Point2DInt>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "x") { 
            point->x = luaL_checkinteger(state, 3); 
        }
        else if(field == "y") { 
            point->y = luaL_checkinteger(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_point2_d_int(lua_State *state, Engine::Point2DInt &point, bool read_only) noexcept {
        lua_push_meta_object(state, point, lua_engine_point2_d_int__index, lua_engine_point2_d_int__newindex, read_only); 
    }

    static int lua_engine_euler2_d__index(lua_State *state) {
        auto *euler = lua_from_meta_object<Engine::Euler2D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "yaw") { 
            lua_pushnumber(state, euler->yaw); 
            return 1; 
        }
        else if(field == "pitch") { 
            lua_pushnumber(state, euler->pitch); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_euler2_d__newindex(lua_State *state) noexcept {
        auto *euler = lua_from_meta_object<Engine::Euler2D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "yaw") { 
            euler->yaw = luaL_checknumber(state, 3); 
        }
        else if(field == "pitch") { 
            euler->pitch = luaL_checknumber(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_euler2_d(lua_State *state, Engine::Euler2D &euler, bool read_only) noexcept {
        lua_push_meta_object(state, euler, lua_engine_euler2_d__index, lua_engine_euler2_d__newindex, read_only); 
    }

    static int lua_engine_euler3_d__index(lua_State *state) {
        auto *euler = lua_from_meta_object<Engine::Euler3D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "yaw") { 
            lua_pushnumber(state, euler->yaw); 
            return 1; 
        }
        else if(field == "pitch") { 
            lua_pushnumber(state, euler->pitch); 
            return 1; 
        }
        else if(field == "roll") { 
            lua_pushnumber(state, euler->roll); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_euler3_d__newindex(lua_State *state) noexcept {
        auto *euler = lua_from_meta_object<Engine::Euler3D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "yaw") { 
            euler->yaw = luaL_checknumber(state, 3); 
        }
        else if(field == "pitch") { 
            euler->pitch = luaL_checknumber(state, 3); 
        }
        else if(field == "roll") { 
            euler->roll = luaL_checknumber(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_euler3_d(lua_State *state, Engine::Euler3D &euler, bool read_only) noexcept {
        lua_push_meta_object(state, euler, lua_engine_euler3_d__index, lua_engine_euler3_d__newindex, read_only); 
    }

    static int lua_engine_euler3_d_p_y_r__index(lua_State *state) {
        auto *euler = lua_from_meta_object<Engine::Euler3DPYR>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "yaw") { 
            lua_pushnumber(state, euler->yaw); 
            return 1; 
        }
        else if(field == "pitch") { 
            lua_pushnumber(state, euler->pitch); 
            return 1; 
        }
        else if(field == "roll") { 
            lua_pushnumber(state, euler->roll); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_euler3_d_p_y_r__newindex(lua_State *state) noexcept {
        auto *euler = lua_from_meta_object<Engine::Euler3DPYR>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "yaw") { 
            euler->yaw = luaL_checknumber(state, 3); 
        }
        else if(field == "pitch") { 
            euler->pitch = luaL_checknumber(state, 3); 
        }
        else if(field == "roll") { 
            euler->roll = luaL_checknumber(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_euler3_d_p_y_r(lua_State *state, Engine::Euler3DPYR &euler, bool read_only) noexcept {
        lua_push_meta_object(state, euler, lua_engine_euler3_d_p_y_r__index, lua_engine_euler3_d_p_y_r__newindex, read_only); 
    }

    static int lua_engine_vector2_d__index(lua_State *state) {
        auto *vector = lua_from_meta_object<Engine::Vector2D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "i") { 
            lua_pushnumber(state, vector->i); 
            return 1; 
        }
        else if(field == "j") { 
            lua_pushnumber(state, vector->j); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_vector2_d__newindex(lua_State *state) noexcept {
        auto *vector = lua_from_meta_object<Engine::Vector2D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "i") { 
            vector->i = luaL_checknumber(state, 3); 
        }
        else if(field == "j") { 
            vector->j = luaL_checknumber(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_vector2_d(lua_State *state, Engine::Vector2D &vector, bool read_only) noexcept {
        lua_push_meta_object(state, vector, lua_engine_vector2_d__index, lua_engine_vector2_d__newindex, read_only); 
    }

    static int lua_engine_vector3_d__index(lua_State *state) {
        auto *vector = lua_from_meta_object<Engine::Vector3D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "i") { 
            lua_pushnumber(state, vector->i); 
            return 1; 
        }
        else if(field == "j") { 
            lua_pushnumber(state, vector->j); 
            return 1; 
        }
        else if(field == "k") { 
            lua_pushnumber(state, vector->k); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_vector3_d__newindex(lua_State *state) noexcept {
        auto *vector = lua_from_meta_object<Engine::Vector3D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "i") { 
            vector->i = luaL_checknumber(state, 3); 
        }
        else if(field == "j") { 
            vector->j = luaL_checknumber(state, 3); 
        }
        else if(field == "k") { 
            vector->k = luaL_checknumber(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_vector3_d(lua_State *state, Engine::Vector3D &vector, bool read_only) noexcept {
        lua_push_meta_object(state, vector, lua_engine_vector3_d__index, lua_engine_vector3_d__newindex, read_only); 
    }

    static int lua_engine_color_r_g_b__index(lua_State *state) noexcept {
        auto *color = lua_from_meta_object<Engine::ColorRGB>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "red") { 
            lua_pushnumber(state, color->red); 
            return 1; 
        }
        else if(field == "green") { 
            lua_pushnumber(state, color->green); 
            return 1; 
        }
        else if(field == "blue") { 
            lua_pushnumber(state, color->blue); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_color_r_g_b__newindex(lua_State *state) noexcept {
        auto *color = lua_from_meta_object<Engine::ColorRGB>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "red") { 
            color->red = luaL_checknumber(state, 3); 
        }
        else if(field == "green") { 
            color->green = luaL_checknumber(state, 3); 
        }
        else if(field == "blue") { 
            color->blue = luaL_checknumber(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_color_r_g_b(lua_State *state, Engine::ColorRGB &color, bool read_only) noexcept {
        lua_push_meta_object(state, color, lua_engine_color_r_g_b__index, lua_engine_color_r_g_b__newindex, read_only); 
    }

    static int lua_engine_quaternion__index(lua_State *state) noexcept {
        auto *quaternion = lua_from_meta_object<Engine::Quaternion>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "i") { 
            lua_pushnumber(state, quaternion->i); 
            return 1; 
        }
        else if(field == "j") { 
            lua_pushnumber(state, quaternion->j); 
            return 1; 
        }
        else if(field == "k") { 
            lua_pushnumber(state, quaternion->k); 
            return 1; 
        }
        else if(field == "w") { 
            lua_pushnumber(state, quaternion->w); 
            return 1; 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_quaternion__newindex(lua_State *state) noexcept {
        auto *quaternion = lua_from_meta_object<Engine::Quaternion>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "i") { 
            quaternion->i = luaL_checknumber(state, 3); 
        }
        else if(field == "j") { 
            quaternion->j = luaL_checknumber(state, 3); 
        }
        else if(field == "k") { 
            quaternion->k = luaL_checknumber(state, 3); 
        }
        else if(field == "w") { 
            quaternion->w = luaL_checknumber(state, 3); 
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_quaternion(lua_State *state, Engine::Quaternion &quaternion, bool read_only) noexcept {
        lua_push_meta_object(state, quaternion, lua_engine_quaternion__index, lua_engine_quaternion__newindex, read_only); 
    }

    static int lua_engine_plane3_d__index(lua_State *state) noexcept {
        auto *plane = lua_from_meta_object<Engine::Plane3D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "vector") { 
            lua_push_meta_engine_vector3_d(state, plane->vector);
            return 1;
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_plane3_d__newindex(lua_State *state) noexcept {
        auto *plane = lua_from_meta_object<Engine::Plane3D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "vector") { 
            return luaL_error(state, "Invalid operation");
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_plane3_d(lua_State *state, Engine::Plane3D &plane, bool read_only) noexcept {
        lua_push_meta_object(state, plane, lua_engine_plane3_d__index, lua_engine_plane3_d__newindex, read_only); 
    }

    static int lua_engine_plane2_d__index(lua_State *state) noexcept {
        auto *plane = lua_from_meta_object<Engine::Plane2D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "vector") { 
            lua_push_meta_engine_vector2_d(state, plane->vector);
            return 1;
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_plane2_d__newindex(lua_State *state) noexcept {
        auto *plane = lua_from_meta_object<Engine::Plane2D>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "vector") { 
            return luaL_error(state, "Invalid operation");
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_plane2_d(lua_State *state, Engine::Plane2D &plane, bool read_only) noexcept {
        lua_push_meta_object(state, plane, lua_engine_plane2_d__index, lua_engine_plane2_d__newindex, read_only); 
    }

    extern std::string lua_engine_u_i_widget_type_to_string(Engine::TagDefinitions::UIWidgetType value) noexcept; 
    extern Engine::TagDefinitions::UIWidgetType lua_engine_u_i_widget_type_from_string(std::string str); 

    static int lua_engine_widget__index(lua_State *state) noexcept {
        auto *widget = lua_from_meta_object<Engine::Widget>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "definitionTagHandle") { 
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&widget->definition_tag_handle));
            return 1;
        }
        else if(field == "name") {
            lua_pushstring(state, widget->name);
            return 1;
        }
        else if(field == "controllerIndex") {
            lua_pushinteger(state, widget->controller_index);
            return 1;
        }
        else if(field == "position") {
            lua_push_meta_engine_point2_d_int(state, widget->position);
            return 1;
        }
        else if(field == "type") {
            std::string type = lua_engine_u_i_widget_type_to_string(widget->type);
            lua_pushstring(state, type.c_str());
            return 1;
        }
        else if(field == "visible") {
            lua_pushboolean(state, widget->visible);
            return 1;
        }
        else if(field == "renderRegardlessOfControllerIndex") {
            lua_pushboolean(state, widget->render_regardless_of_controller_index);
            return 1;
        }
        else if(field == "pausesGameTime") {
            lua_pushboolean(state, widget->pauses_game_time);
            return 1;
        }
        else if(field == "deleted") {
            lua_pushboolean(state, widget->deleted);
            return 1;
        }
        else if(field == "creationProcessStartTime") {
            lua_pushinteger(state, widget->creation_process_start_time);
            return 1;
        }
        else if(field == "msToClose") {
            lua_pushinteger(state, widget->ms_to_close);
            return 1;
        }
        else if(field == "msToCloseFadeTime") {
            lua_pushinteger(state, widget->ms_to_close_fade_time);
            return 1;
        }
        else if(field == "opacity") {
            lua_pushnumber(state, widget->opacity);
            return 1;
        }
        else if(field == "previousWidget") {
            if(widget->previous_widget == nullptr) {
                lua_pushnil(state);
            }
            else {
                lua_push_meta_engine_widget(state, *widget->previous_widget);
            }
            return 1;
        }
        else if(field == "nextWidget") {
            if(widget->next_widget == nullptr) {
                lua_pushnil(state);
            }
            else {
                lua_push_meta_engine_widget(state, *widget->next_widget);
            }
            return 1;
        }
        else if(field == "parentWidget") {
            if(widget->parent_widget == nullptr) {
                lua_pushnil(state);
            }
            else {
                lua_push_meta_engine_widget(state, *widget->parent_widget);
            }
            return 1;
        }
        else if(field == "childWidget") {
            if(widget->child_widget == nullptr) {
                lua_pushnil(state);
            }
            else {
                lua_push_meta_engine_widget(state, *widget->child_widget);
            }
            return 1;
        }
        else if(field == "focusedChild") {
            if(widget->focused_child == nullptr) {
                lua_pushnil(state);
            }
            else {
                lua_push_meta_engine_widget(state, *widget->focused_child);
            }
            return 1;
        }
        else if(field == "textAddress") {
            lua_pushinteger(state, reinterpret_cast<std::uint32_t>(const_cast<wchar_t *>(widget->text_box.text)));
            return 1;
        }
        else if(field == "cursorIndex") {
            lua_pushinteger(state, widget->text_box.cursor_index);
            return 1;
        }
        else if(field == "extendedDescription") {
            lua_push_meta_engine_widget(state, *widget->extended_description);
            return 1;
        }
        else if(field == "bitmapIndex") {
            lua_pushinteger(state, widget->bitmap_index);
            return 1;
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_widget__newindex(lua_State *state) noexcept {
        auto *widget = lua_from_meta_object<Engine::Widget>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key; 
        if(field == "definitionTagHandle") { 
            widget->definition_tag_handle.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "name") {
            return luaL_error(state, "Unsupported operation");
        }
        else if(field == "controllerIndex") {
            widget->controller_index = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "position") {
            return luaL_error(state, "Unsupported operation");
        }
        else if(field == "type") {
            try {
                std::string type = luaL_checkstring(state, 3);
                widget->type = lua_engine_u_i_widget_type_from_string(type);
            }
            catch(...) {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "visible") {
            widget->visible = lua_toboolean(state, 3);
        }
        else if(field == "renderRegardlessOfControllerIndex") {
            widget->render_regardless_of_controller_index = lua_toboolean(state, 3);
        }
        else if(field == "pausesGameTime") {
            widget->pauses_game_time = lua_toboolean(state, 3);
        }
        else if(field == "deleted") {
            widget->deleted = lua_toboolean(state, 3);
        }
        else if(field == "creationProcessStartTime") {
            widget->creation_process_start_time = luaL_checkinteger(state, 3);
        }
        else if(field == "msToClose") {
            widget->ms_to_close = luaL_checkinteger(state, 3);
        }
        else if(field == "msToCloseFadeTime") {
            widget->ms_to_close_fade_time = luaL_checkinteger(state, 3);
        }
        else if(field == "opacity") {
            float opacity = luaL_checknumber(state, 3);
            if(opacity < 0.0f || opacity > 1.0f) {
                return luaL_error(state, "Invalid value");
            }
            widget->opacity = opacity;
        }
        else if(field == "previousWidget") {
            auto *previous_widget = lua_from_meta_object<Engine::Widget>(state, 3);
            if(previous_widget) {
                widget->previous_widget = previous_widget;
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "nextWidget") {
            auto *next_widget = lua_from_meta_object<Engine::Widget>(state, 3);
            if(next_widget) {
                widget->next_widget = next_widget;
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "parentWidget") {
            auto *parent_widget = lua_from_meta_object<Engine::Widget>(state, 3);
            if(parent_widget) {
                widget->parent_widget = parent_widget;
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "childWidget") {
            auto *child_widget = lua_from_meta_object<Engine::Widget>(state, 3);
            if(child_widget) {
                widget->child_widget = child_widget;
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "focusedChild") {
            auto *focused_child = lua_from_meta_object<Engine::Widget>(state, 3);
            if(focused_child) {
                widget->focused_child = focused_child;
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "textAddress") {
            return luaL_error(state, "Unsupported operation");
        }
        else if(field == "cursorIndex") {
            widget->text_box.cursor_index = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "extendedDescription") {
            auto *extended_description = lua_from_meta_object<Engine::Widget>(state, 3);
            if(extended_description) {
                widget->extended_description = extended_description;
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "bitmapIndex") {
            widget->bitmap_index = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_widget(lua_State *state, Engine::Widget &widget, bool read_only) noexcept {
        lua_push_meta_object(state, widget, lua_engine_widget__index, lua_engine_widget__newindex, read_only); 
    }

    static int lua_engine_camera_data__index(lua_State *state) noexcept {
        auto *camera = lua_from_meta_object<Engine::CameraData>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "position") { 
            lua_push_meta_engine_point3_d(state, camera->position);
            return 1;
        }
        else if(field == "orientation") {
            lua_newtable(state);
            lua_push_meta_engine_point3_d(state, camera->orientation[0]);
            lua_rawseti(state, -2, 1);
            lua_push_meta_engine_point3_d(state, camera->orientation[1]);
            lua_rawseti(state, -2, 2);
            return 1;
        }
        else if(field == "fov") {
            lua_pushnumber(state, camera->fov);
            return 1;
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
    }

    static int lua_engine_camera_data__newindex(lua_State *state) noexcept {
        auto *camera = lua_from_meta_object<Engine::CameraData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "position") { 
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "orientation") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "fov") {
            camera->fov = luaL_checknumber(state, 3);
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_camera_data(lua_State *state, Engine::CameraData &camera, bool read_only) noexcept {
        lua_push_meta_object(state, camera, lua_engine_camera_data__index, lua_engine_camera_data__newindex, read_only); 
    }

    static int lua_engine_rotation_matrix__index(lua_State *state) noexcept {
        auto *rotation_matrix = lua_from_meta_object<Engine::RotationMatrix>(state, 1); 
        auto index = luaL_checkinteger(state, 2);

        if(index < 1 || index > 3) {  
            return luaL_error(state, "Index out of bounds"); 
        } 
        
        if(index == 1) { 
            lua_push_meta_engine_point3_d(state, rotation_matrix->v[0]);
        }
        else if(index == 2) {
            lua_push_meta_engine_point3_d(state, rotation_matrix->v[1]);
        }
        else if(index == 3) {
            lua_push_meta_engine_point3_d(state, rotation_matrix->v[2]);
        }
        return 1;
    }

    static int lua_engine_rotation_matrix__newindex(lua_State *state) noexcept {
        auto *rotation_matrix = lua_from_meta_object<Engine::RotationMatrix>(state, 1); 
        auto index = luaL_checkinteger(state, 2);

        if(index < 1 || index > 3) {  
            return luaL_error(state, "Index out of bounds"); 
        } 
        
        if(index == 1) { 
            return luaL_error(state, "Invalid operation");
        }
        else if(index == 2) {
            return luaL_error(state, "Invalid operation");
        }
        else if(index == 3) {
            return luaL_error(state, "Invalid operation");
        }
        return 0;
    }

    void lua_push_meta_engine_rotation_matrix(lua_State *state, Engine::RotationMatrix &matrix, bool read_only) noexcept {
        lua_push_meta_object(state, matrix, lua_engine_rotation_matrix__index, lua_engine_rotation_matrix__newindex, read_only); 
    }

    static int lua_engine_model_node__index(lua_State *state) noexcept {
        auto *model_node = lua_from_meta_object<Engine::ModelNode>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "scale") { 
            lua_pushnumber(state, model_node->scale);
            return 1;
        }
        else if(field == "rotationMatrix") {
            lua_push_meta_engine_rotation_matrix(state, model_node->rotation);
            return 1;
        }
        else if(field == "position") {
            lua_push_meta_engine_point3_d(state, model_node->position);
            return 1;
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    static int lua_engine_model_node__newindex(lua_State *state) noexcept {
        auto *model_node = lua_from_meta_object<Engine::ModelNode>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "scale") { 
            model_node->scale = luaL_checknumber(state, 3);
        }
        else if(field == "rotationMatrix") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "position") {
            return luaL_error(state, "Invalid operation");
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_model_node(lua_State *state, Engine::ModelNode &node, bool read_only) noexcept {
        lua_push_meta_object(state, node, lua_engine_model_node__index, lua_engine_model_node__newindex, read_only); 
    }

    static int lua_engine_object_flags__index(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::BaseObjectFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "noCollision") { 
            lua_pushboolean(state, flags->no_collision);
        }
        else if(field == "onGround") {
            lua_pushboolean(state, flags->on_ground);
        }
        else if(field == "ignoreGravity") {
            lua_pushboolean(state, flags->ignore_gravity);
        }
        else if(field == "inWater") {
            lua_pushboolean(state, flags->in_water);
        }
        else if(field == "stationary") {
            lua_pushboolean(state, flags->stationary);
        }
        else if(field == "noCollision2") {
            lua_pushboolean(state, flags->no_collision2);
        }
        else if(field == "hasSoundLoopingAttachment") {
            lua_pushboolean(state, flags->has_sound_looping_attachment);
        }
        else if(field == "connectedToMap") {
            lua_pushboolean(state, flags->connected_to_map);
        }
        else if(field == "notPlacedAutomatically") {
            lua_pushboolean(state, flags->not_placed_automatically);
        }
        else if(field == "isDeviceMachine") {
            lua_pushboolean(state, flags->is_device_machine);
        }
        else if(field == "isElevator") {
            lua_pushboolean(state, flags->is_elevator);
        }
        else if(field == "isElevator_2") {
            lua_pushboolean(state, flags->is_elevator_2);
        }
        else if(field == "isGarbage") {
            lua_pushboolean(state, flags->is_garbage);
        }
        else if(field == "noShadow") {
            lua_pushboolean(state, flags->no_shadow);
        }
        else if(field == "deleteAtDeactivation") {
            lua_pushboolean(state, flags->delete_at_deactivation);
        }
        else if(field == "doNotReactivate") {
            lua_pushboolean(state, flags->do_not_reactivate);
        }
        else if(field == "outsideOfMap") {
            lua_pushboolean(state, flags->outside_of_map);
        }
        else if(field == "collidable") {
            lua_pushboolean(state, flags->collidable);
        }
        else if(field == "hasCollisionModel") {
            lua_pushboolean(state, flags->has_collision_model);
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_object_flags__newindex(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::BaseObjectFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        bool value = lua_toboolean(state, 3);
        if(field == "noCollision") { 
            flags->no_collision = value;
        }
        else if(field == "onGround") {
            flags->on_ground = value;
        }
        else if(field == "ignoreGravity") {
            flags->ignore_gravity = value;
        }
        else if(field == "inWater") {
            flags->in_water = value;
        }
        else if(field == "stationary") {
            flags->stationary = value;
        }
        else if(field == "noCollision2") {
            flags->no_collision2 = value;
        }
        else if(field == "hasSoundLoopingAttachment") {
            flags->has_sound_looping_attachment = value;
        }
        else if(field == "connectedToMap") {
            flags->connected_to_map = value;
        }
        else if(field == "notPlacedAutomatically") {
            flags->not_placed_automatically = value;
        }
        else if(field == "isDeviceMachine") {
            flags->is_device_machine = value;
        }
        else if(field == "isElevator") {
            flags->is_elevator = value;
        }
        else if(field == "isElevator2") {
            flags->is_elevator_2 = value;
        }
        else if(field == "isGarbage") {
            flags->is_garbage = value;
        }
        else if(field == "noShadow") {
            flags->no_shadow = value;
        }
        else if(field == "deleteAtDeactivation") {
            flags->delete_at_deactivation = value;
        }
        else if(field == "doNotReactivate") {
            flags->do_not_reactivate = value;
        }
        else if(field == "outsideOfMap") {
            flags->outside_of_map = value;
        }
        else if(field == "collidable") {
            flags->collidable = value;
        }
        else if(field == "hasCollisionModel") {
            flags->has_collision_model = value;
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_object_flags(lua_State *state, Engine::BaseObjectFlags &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_object_flags__index, lua_engine_object_flags__newindex, read_only); 
    }

    static int lua_engine_object_network__index(lua_State *state) noexcept {
        auto *object_network = lua_from_meta_object<Engine::BaseObjectNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "validPosition") { 
            lua_pushboolean(state, object_network->valid_position);
        }
        else if(field == "position") {
            lua_push_meta_engine_vector3_d(state, object_network->position);
        }
        else if(field == "validForwardAndUp") { 
            lua_pushboolean(state, object_network->valid_forward_and_up);
        }
        else if(field == "orientation") {
            lua_newtable(state);
            lua_push_meta_engine_point3_d(state, object_network->orientation[0]);
            lua_rawseti(state, -2, 1);
            lua_push_meta_engine_point3_d(state, object_network->orientation[1]);
            lua_rawseti(state, -2, 2);
        }
        else if(field == "validTransitionalVelocity") { 
            lua_pushboolean(state, object_network->valid_transitional_velocity);
        }
        else if(field == "transitionalVelocity") {
            lua_push_meta_engine_vector3_d(state, object_network->transitional_velocity);
        }
        else if(field == "validTimestamp") { 
            lua_pushboolean(state, object_network->valid_timestamp);
        }
        else if(field == "timestamp") { 
            lua_pushinteger(state, object_network->timestamp);
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_object_network__newindex(lua_State *state) noexcept {
        auto *object_network = lua_from_meta_object<Engine::BaseObjectNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key"); 
        } 
        
        std::string field = key;
        if(field == "validPosition") { 
            if(lua_isboolean(state, 3)) {
                object_network->valid_position = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "position") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "validForwardAndUp") { 
            if(lua_isboolean(state, 3)) {
                object_network->valid_forward_and_up = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "orientation") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "validTransitionalVelocity") { 
            if(lua_isboolean(state, 3)) {
                object_network->valid_transitional_velocity = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "transitionalVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "validTimestamp") { 
            if(lua_isboolean(state, 3)) {
                object_network->valid_timestamp = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else if(field == "timestamp") { 
            if(lua_isinteger(state, 3)) {
                object_network->timestamp = static_cast<Engine::TickCount32>(lua_tointeger(state, 3));
            }
            else {
                return luaL_error(state, "Invalid value");
            }
        }
        else { 
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_object_network(lua_State *state, Engine::BaseObjectNetwork &network, bool read_only) noexcept {
        lua_push_meta_object(state, network, lua_engine_object_network__index, lua_engine_object_network__newindex, read_only); 
    }

    static int lua_engine_scenario_location__index(lua_State *state) noexcept {
        auto *location = lua_from_meta_object<Engine::ScenarioLocation>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "leafId") { 
            lua_pushinteger(state, location->leaf_id);
        }
        else if(field == "clusterId") {
            lua_pushinteger(state, location->cluster_id);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_scenario_location__newindex(lua_State *state) noexcept {
        auto *location = lua_from_meta_object<Engine::ScenarioLocation>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key"); 
        } 
        
        std::string field = key;
        if(field == "leafId") { 
            location->leaf_id = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "clusterId") {
            location->cluster_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_scenario_location(lua_State *state, Engine::ScenarioLocation &location, bool read_only) noexcept {
        lua_push_meta_object(state, location, lua_engine_scenario_location__index, lua_engine_scenario_location__newindex, read_only); 
    }

    static int lua_engine_object_vitals_flags__index(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::BaseObjectVitalsFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "healthDamageEffectApplied") { 
            lua_pushboolean(state, flags->health_damage_effect_applied);
        }
        else if(field == "shieldDamageEffectApplied") { 
            lua_pushboolean(state, flags->shield_damage_effect_applied);
        }
        else if(field == "healthDepleted") { 
            lua_pushboolean(state, flags->health_depleted);
        }
        else if(field == "shieldDepleted") { 
            lua_pushboolean(state, flags->shield_depleted);
        }
        else if(field == "killed") { 
            lua_pushboolean(state, flags->killed);
        }
        else if(field == "killedSilent") { 
            lua_pushboolean(state, flags->killed_silent);
        }
        else if(field == "cannotMeleeAttack") { 
            lua_pushboolean(state, flags->cannot_melee_attack);
        }
        else if(field == "invulnerable") { 
            lua_pushboolean(state, flags->invulnerable);
        }
        else if(field == "shieldRecharging") { 
            lua_pushboolean(state, flags->shield_recharging);
        }
        else if(field == "killedNoStats") { 
            lua_pushboolean(state, flags->killed_no_stats);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_object_vitals_flags__newindex(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::BaseObjectVitalsFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        bool value = lua_toboolean(state, 3);
        if(field == "healthDamageEffectApplied") { 
            flags->health_damage_effect_applied = value;
        }
        else if(field == "shieldDamageEffectApplied") { 
            flags->shield_damage_effect_applied = value;
        }
        else if(field == "healthDepleted") { 
            flags->health_depleted = value;
        }
        else if(field == "shieldDepleted") { 
            flags->shield_depleted = value;
        }
        else if(field == "killed") { 
            flags->killed = value;
        }
        else if(field == "killedSilent") { 
            flags->killed_silent = value;
        }
        else if(field == "cannotMeleeAttack") { 
            flags->cannot_melee_attack = value;
        }
        else if(field == "invulnerable") { 
            flags->invulnerable = value;
        }
        else if(field == "shieldRecharging") { 
            flags->shield_recharging = value;
        }
        else if(field == "killedNoStats") { 
            flags->killed_no_stats = value;
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_object_vitals_flags(lua_State *state, Engine::BaseObjectVitalsFlags &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_object_vitals_flags__index, lua_engine_object_vitals_flags__newindex, read_only); 
    }

    static int lua_engine_object_vitals__index(lua_State *state) noexcept {
        auto *vitals = lua_from_meta_object<Engine::BaseObjectVitals>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "baseHealth") { 
            lua_pushnumber(state, vitals->base_health);
        }
        else if(field == "baseShield") {
            lua_pushnumber(state, vitals->base_shield);
        }
        else if(field == "health") {
            lua_pushnumber(state, vitals->health);
        }
        else if(field == "shield") {
            lua_pushnumber(state, vitals->shield);
        }
        else if(field == "currentShieldDamage") {
            lua_pushnumber(state, vitals->current_shield_damage);
        }
        else if(field == "currentHealthDamage") {
            lua_pushnumber(state, vitals->current_health_damage);
        }
        else if(field == "entangledObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&vitals->entangled_object));
        }
        else if(field == "recentShieldDamage") {
            lua_pushnumber(state, vitals->recent_shield_damage);
        }
        else if(field == "recentHealthDamage") {
            lua_pushnumber(state, vitals->recent_health_damage);
        }
        else if(field == "recentShieldDamageTime") {
            lua_pushinteger(state, vitals->recent_shield_damage_time);
        }
        else if(field == "recentHealthDamageTime") {
            lua_pushinteger(state, vitals->recent_health_damage_time);
        }
        else if(field == "shieldStunTime") {
            lua_pushinteger(state, vitals->shield_stun_time);
        }
        else if(field == "flags") {
            lua_push_meta_engine_object_vitals_flags(state, vitals->flags);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_object_vitals__newindex(lua_State *state) noexcept {
        auto *vitals = lua_from_meta_object<Engine::BaseObjectVitals>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key"); 
        } 
        
        std::string field = key;
        if(field == "baseHealth") { 
            vitals->base_health = luaL_checknumber(state, 3);
        }
        else if(field == "baseShield") {
            vitals->base_shield = luaL_checknumber(state, 3);
        }
        else if(field == "health") {
            vitals->health = luaL_checknumber(state, 3);
        }
        else if(field == "shield") {
            vitals->shield = luaL_checknumber(state, 3);
        }
        else if(field == "currentShieldDamage") {
            vitals->current_shield_damage = luaL_checknumber(state, 3);
        }
        else if(field == "currentHealthDamage") {
            vitals->current_health_damage = luaL_checknumber(state, 3);
        }
        else if(field == "entangledObject") {
            vitals->entangled_object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "recentShieldDamage") {
            vitals->recent_shield_damage = luaL_checknumber(state, 3);
        }
        else if(field == "recentHealthDamage") {
            vitals->recent_health_damage = luaL_checknumber(state, 3);
        }
        else if(field == "recentShieldDamageTime") {
            vitals->recent_shield_damage_time = static_cast<Engine::TickCount32>(luaL_checkinteger(state, 3));
        }
        else if(field == "recentHealthDamageTime") {
            vitals->recent_health_damage_time = static_cast<Engine::TickCount32>(luaL_checkinteger(state, 3));
        }
        else if(field == "shieldStunTime") {
            vitals->shield_stun_time = static_cast<Engine::TickCount16>(luaL_checkinteger(state, 3));
        }
        else if(field == "flags") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_object_vitals(lua_State *state, Engine::BaseObjectVitals &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_object_vitals__index, lua_engine_object_vitals__newindex, read_only); 
    }

    static int lua_engine_object_attachments_data__index(lua_State *state) noexcept {
        auto *attachments_data = lua_from_meta_object<Engine::BaseObjectAttachmentsData>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "types") { 
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(attachments_data->types) / sizeof(attachments_data->types[0]); i++) {
                lua_pushstring(state, object_attachment_type_to_string(attachments_data->types[i]).c_str());
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "attachments") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(attachments_data->attachments) / sizeof(attachments_data->attachments[0]); i++) {
                lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&attachments_data->attachments[i]));
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "firstWidget") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&attachments_data->first_widget));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_object_attachments_data__newindex(lua_State *state) noexcept {
        auto *attachments_data = lua_from_meta_object<Engine::BaseObjectAttachmentsData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "types") { 
            try {
                for(std::size_t i = 0; i < sizeof(attachments_data->types) / sizeof(attachments_data->types[0]); i++) {
                    lua_rawgeti(state, 3, i + 1);
                    if(!lua_isnil(state, -1)) {
                        attachments_data->types[i] = object_attachment_type_from_string(luaL_checkstring(state, -1));
                    }
                    lua_pop(state, 1);
                }
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "attachments") {
            try {
                for(std::size_t i = 0; i < sizeof(attachments_data->attachments) / sizeof(attachments_data->attachments[0]); i++) {
                    lua_rawgeti(state, 3, i + 1);
                    if(!lua_isnil(state, -1)) {
                        attachments_data->attachments[i] = static_cast<Engine::ObjectHandle>(luaL_checkinteger(state, -1));
                    }
                    lua_pop(state, 1);
                }
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "firstWidget") {
            attachments_data->first_widget.handle = luaL_checkinteger(state, 3);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_object_attachments_data(lua_State *state, Engine::BaseObjectAttachmentsData &attachments_data, bool read_only) noexcept {
        lua_push_meta_object(state, attachments_data, lua_engine_object_attachments_data__index, lua_engine_object_attachments_data__newindex, read_only); 
    }

    static int lua_engine_object_region_destroyeds__index(lua_State *state) noexcept {
        auto *regions = lua_from_meta_object<Engine::BaseObjectRegionDestroyeds>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "region0") { 
            lua_pushboolean(state, regions->region_0);
        }
        else if(field == "region1") {
            lua_pushboolean(state, regions->region_1);
        }
        else if(field == "region2") {
            lua_pushboolean(state, regions->region_2);
        }
        else if(field == "region3") {
            lua_pushboolean(state, regions->region_3);
        }
        else if(field == "region4") {
            lua_pushboolean(state, regions->region_4);
        }
        else if(field == "region5") {
            lua_pushboolean(state, regions->region_5);
        }
        else if(field == "region6") {
            lua_pushboolean(state, regions->region_6);
        }
        else if(field == "region7") {
            lua_pushboolean(state, regions->region_7);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_object_region_destroyeds__newindex(lua_State *state) noexcept {
        auto *regions = lua_from_meta_object<Engine::BaseObjectRegionDestroyeds>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        bool value = lua_toboolean(state, 3);
        if(field == "region0") { 
            regions->region_0 = value;
        }
        else if(field == "region1") {
            regions->region_1 = value;
        }
        else if(field == "region2") {
            regions->region_2 = value;
        }
        else if(field == "region3") {
            regions->region_3 = value;
        }
        else if(field == "region4") {
            regions->region_4 = value;
        }
        else if(field == "region5") {
            regions->region_5 = value;
        }
        else if(field == "region6") {
            regions->region_6 = value;
        }
        else if(field == "region7") {
            regions->region_7 = value;
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_object_region_destroyeds(lua_State *state, Engine::BaseObjectRegionDestroyeds &regions, bool read_only) noexcept {
        lua_push_meta_object(state, regions, lua_engine_object_region_destroyeds__index, lua_engine_object_region_destroyeds__newindex, read_only); 
    }

    static int lua_engine_object_block_reference__index(lua_State *state) noexcept {
        auto *block_reference = lua_from_meta_object<Engine::BaseObjectBlockReference>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "size") { 
            lua_pushinteger(state, block_reference->size);
        }
        else if(field == "offset") {
            lua_pushinteger(state, block_reference->offset);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_object_block_reference__newindex(lua_State *state) noexcept {
        auto *block_reference = lua_from_meta_object<Engine::BaseObjectBlockReference>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isinteger(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "size") { 
            block_reference->size = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "offset") {
            block_reference->offset = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_object_block_reference(lua_State *state, Engine::BaseObjectBlockReference &block_reference, bool read_only) noexcept {
        lua_push_meta_object(state, block_reference, lua_engine_object_block_reference__index, lua_engine_object_block_reference__newindex, read_only); 
    }

    extern std::string lua_engine_scenario_team_index_to_string(Engine::TagDefinitions::ScenarioTeamIndex value) noexcept; 
    extern Engine::TagDefinitions::ScenarioTeamIndex lua_engine_scenario_team_index_from_string(std::string str); 

    static int lua_engine_object__index(lua_State *state) noexcept {
        auto *object = lua_from_meta_object<Engine::BaseObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "tagHandle") { 
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->tag_handle));
        }
        else if(field == "networkRole") {
            lua_pushstring(state, object_network_role_to_string(object->network_role).c_str());
        }
        else if(field == "shouldForceBaselineUpdate") {
            lua_pushboolean(state, object->should_force_baseline_update == 1);
        }
        else if(field == "existenceTime") {
            lua_pushinteger(state, object->existence_time);
        }
        else if(field == "flags") {
            lua_push_meta_engine_object_flags(state, object->flags);
        }
        else if(field == "objectMarkerId") {
            lua_pushinteger(state, object->object_marker_id);
        }
        else if(field == "network") {
            lua_push_meta_engine_object_network(state, object->network);
        }
        else if(field == "position") {
            lua_push_meta_engine_point3_d(state, object->position);
        }
        else if(field == "velocity") {
            lua_push_meta_engine_point3_d(state, object->velocity);
        }
        else if(field == "orientation") {
            lua_newtable(state);
            lua_push_meta_engine_point3_d(state, object->orientation[0]);
            lua_rawseti(state, -2, 1);
            lua_push_meta_engine_point3_d(state, object->orientation[1]);
            lua_rawseti(state, -2, 2);
        }
        else if(field == "rotationVelocity") {
            lua_push_meta_engine_euler3_d_p_y_r(state, object->rotation_velocity, false);
        }
        else if(field == "scenarioLocation") {
            lua_push_meta_engine_scenario_location(state, object->scenario_location);
        }
        else if(field == "centerPosition") {
            lua_push_meta_engine_point3_d(state, object->center_position);
        }
        else if(field == "boundingRadius") {
            lua_pushnumber(state, object->bounding_radius);
        }
        else if(field == "scale") {
            lua_pushnumber(state, object->scale);
        }
        else if(field == "type") {
            lua_pushstring(state, object_type_to_string(object->type).c_str());
        }
        else if(field == "teamOwner") {
            lua_pushstring(state, lua_engine_scenario_team_index_to_string(object->team_owner).c_str());
        }
        else if(field == "multiplayerTeamOwner") {
            lua_pushstring(state, network_game_multiplayer_team_to_string(object->multiplayer_team_owner).c_str());
        }
        else if(field == "nameListIndex") {
            lua_pushinteger(state, object->name_list_index);
        }
        else if(field == "movingTime") {
            lua_pushinteger(state, object->moving_time);
        }
        else if(field == "variantIndex") {
            lua_pushinteger(state, object->variant_index);
        }
        else if(field == "player") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->player));
        }
        else if(field == "ownerObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->owner_object));
        }
        else if(field == "animationTagHandle") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->animation_tag_handle));
        }
        else if(field == "animationIndex") {
            lua_pushinteger(state, object->animation_index);
        }
        else if(field == "animationFrame") {
            lua_pushinteger(state, object->animation_frame);
        }
        else if(field == "animationInterpolationFrame") {
            lua_pushinteger(state, object->animation_interpolation_frame);
        }
        else if(field == "animationInterpolationFrameCount") {
            lua_pushinteger(state, object->animation_interpolation_frame_count);
        }
        else if(field == "vitals") {
            lua_push_meta_engine_object_vitals(state, object->vitals);
        }
        else if(field == "clusterPartition") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->cluster_partition));
        }
        else if(field == "unknownObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->unknown_object));
        }
        else if(field == "nextObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->next_object));
        }
        else if(field == "firstObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->first_object));
        }
        else if(field == "parentObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->parent_object));
        }
        else if(field == "parentAttachmentNode") {
            lua_pushinteger(state, object->parent_attachment_node);
        }
        else if(field == "forceShieldUpdate") {
            lua_pushboolean(state, object->force_shield_update);
        }
        else if(field == "attachmentData") {
            lua_push_meta_engine_object_attachments_data(state, object->attachment_data);
        }
        else if(field == "cachedRenderState") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->cached_render_state));
        }
        else if(field == "regionDestroyeds") {
            lua_push_meta_engine_object_region_destroyeds(state, object->region_destroyeds);
        }
        else if(field == "shaderPermutation") {
            lua_pushinteger(state, object->shader_permutation);
        }
        else if(field == "regionHealths") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(object->region_healths) / sizeof(object->region_healths[0]); i++) {
                lua_pushinteger(state, object->region_healths[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "regionPermutationIds") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(object->region_permutation_ids) / sizeof(object->region_permutation_ids[0]); i++) {
                lua_pushinteger(state, object->region_permutation_ids[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "colorChange") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(object->color_change) / sizeof(object->color_change[0]); i++) {
                lua_push_meta_engine_color_r_g_b(state, object->color_change[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "colorChange2") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(object->color_change_2) / sizeof(object->color_change_2[0]); i++) {
                lua_push_meta_engine_color_r_g_b(state, object->color_change_2[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "nodeOrientations") {
            lua_newtable(state);
            lua_push_meta_engine_object_block_reference(state, object->node_orientations[0]);
            lua_rawseti(state, -2, 1);
            lua_push_meta_engine_object_block_reference(state, object->node_orientations[1]);
            lua_rawseti(state, -2, 2);
        }
        else if(field == "nodeMatricesBlock") {
            lua_push_meta_engine_object_block_reference(state, object->node_matrices_block);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_object__newindex(lua_State *state) noexcept {
        auto *object = lua_from_meta_object<Engine::BaseObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key"); 
        } 
        
        std::string field = key;
        if(field == "tagHandle") { 
            object->tag_handle.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "networkRole") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "shouldForceBaselineUpdate") {
            object->should_force_baseline_update = luaL_checkinteger(state, 3);
        }
        else if(field == "existenceTime") {
            object->existence_time = static_cast<Engine::TickCount32>(luaL_checkinteger(state, 3));
        }
        else if(field == "flags") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "objectMarkerId") {
            object->object_marker_id = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "network") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "position") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "velocity") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "orientation") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "rotationVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "scenarioLocation") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "centerPosition") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "boundingRadius") {
            object->bounding_radius = luaL_checknumber(state, 3);
        }
        else if(field == "scale") {
            object->scale = luaL_checknumber(state, 3);
        }
        else if(field == "type") {
            try {
                object->type = object_type_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "teamOwner") {
            try {
                object->team_owner = lua_engine_scenario_team_index_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "multiplayerTeamOwner") {
            try {
                object->multiplayer_team_owner = network_game_multiplayer_team_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "nameListIndex") {
            object->name_list_index = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "movingTime") {
            object->moving_time = static_cast<Engine::TickCount16>(luaL_checkinteger(state, 3));
        }
        else if(field == "variantIndex") {
            object->variant_index = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "player") {
            object->player.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "ownerObject") {
            object->owner_object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "animationTagHandle") {
            object->animation_tag_handle.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "animationIndex") {
            object->animation_index = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "animationFrame") {
            object->animation_frame = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "animationInterpolationFrame") {
            object->animation_interpolation_frame = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "animationInterpolationFrameCount") {
            object->animation_interpolation_frame_count = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "vitals") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "clusterPartition") {
            object->cluster_partition.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "unknownObject") {
            object->unknown_object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "nextObject") {
            object->next_object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "firstObject") {
            object->first_object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "parentObject") {
            object->parent_object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "parentAttachmentNode") {
            object->parent_attachment_node = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "forceShieldUpdate") {
            if(lua_isboolean(state, 3)) {
                object->force_shield_update = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "attachmentData") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "cachedRenderState") {
            object->cached_render_state.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "regionDestroyeds") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "shaderPermutation") {
            object->shader_permutation = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "regionHealths") {
            for(std::size_t i = 0; i < sizeof(object->region_healths) / sizeof(object->region_healths[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    object->region_healths[i] = static_cast<std::uint8_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "regionPermutationIds") {
            for(std::size_t i = 0; i < sizeof(object->region_permutation_ids) / sizeof(object->region_permutation_ids[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    object->region_permutation_ids[i] = static_cast<std::int8_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "colorChange") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "colorChange2") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "nodeOrientations") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "nodeMatricesBlock") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_object(lua_State *state, Engine::BaseObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_object__index, lua_engine_object__newindex, read_only); 
    }

    static int lua_engine_unit_recent_damager__index(lua_State *state) noexcept {
        auto *object = lua_from_meta_object<Engine::UnitRecentDamager>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "lastDamageTime") {
            lua_pushinteger(state, object->last_damage_time);
        }
        else if(field == "totalDamage") {
            lua_pushnumber(state, object->total_damage);
        }
        else if(field == "object") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->object));
        }
        else if(field == "player") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&object->player));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_unit_recent_damager__newindex(lua_State *state) noexcept {
        auto *object = lua_from_meta_object<Engine::UnitRecentDamager>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key"); 
        } 
        
        std::string field = key;
        if(field == "lastDamageTime") {
            object->last_damage_time = static_cast<Engine::TickCount32>(luaL_checkinteger(state, 3));
        }
        else if(field == "totalDamage") {
            object->total_damage = luaL_checknumber(state, 3);
        }
        else if(field == "object") {
            object->object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "player") {
            object->player.handle = luaL_checkinteger(state, 3);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_unit_recent_damager(lua_State *state, Engine::UnitRecentDamager &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_unit_recent_damager__index, lua_engine_unit_recent_damager__newindex, read_only); 
    }

    static int lua_engine_unit_flags__index(lua_State *state) {
        auto *flags = lua_from_meta_object<Engine::UnitFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "unknownBipedSpeechRelated") {
            lua_pushboolean(state, flags->unknown_biped_speech_related);
        }
        else if(field == "powerUp") {
            lua_pushboolean(state, flags->power_up);
        }
        else if(field == "powerUpAddition") {
            lua_pushboolean(state, flags->power_up_addition);
        }
        else if(field == "controllable") {
            lua_pushboolean(state, flags->controllable);
        }
        else if(field == "berserking") {
            lua_pushboolean(state, flags->berserking);
        }
        else if(field == "unknownIntegratedLightRelated") {
            lua_pushboolean(state, flags->unknown_integrated_light_related);
        }
        else if(field == "willNotDropItems") {
            lua_pushboolean(state, flags->will_not_drop_items);
        }
        else if(field == "unknown") {
            lua_pushboolean(state, flags->unknown);
        }
        else if(field == "canBlink") {
            lua_pushboolean(state, flags->can_blink);
        }
        else if(field == "impervious") {
            lua_pushboolean(state, flags->impervious);
        }
        else if(field == "suspended") {
            lua_pushboolean(state, flags->suspended);
        }
        else if(field == "blind") {
            lua_pushboolean(state, flags->blind);
        }
        else if(field == "unknownNvRelated") {
            lua_pushboolean(state, flags->unknown_nv_related);
        }
        else if(field == "possessed") {
            lua_pushboolean(state, flags->possessed);
        }
        else if(field == "desiresFlashlightOn") {
            lua_pushboolean(state, flags->desires_flashlight_on);
        }
        else if(field == "desiresFlashlightOff") {
            lua_pushboolean(state, flags->desires_flashlight_off);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_unit_flags__newindex(lua_State *state) {
        auto *flags = lua_from_meta_object<Engine::UnitFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        bool value = lua_toboolean(state, 3);
        if(field == "unknownBipedSpeechRelated") {
            flags->unknown_biped_speech_related = value;
        }
        else if(field == "powerUp") {
            flags->power_up = value;
        }
        else if(field == "powerUpAddition") {
            flags->power_up_addition = value;
        }
        else if(field == "controllable") {
            flags->controllable = value;
        }
        else if(field == "berserking") {
            flags->berserking = value;
        }
        else if(field == "unknownIntegratedLightRelated") {
            flags->unknown_integrated_light_related = value;
        }
        else if(field == "willNotDropItems") {
            flags->will_not_drop_items = value;
        }
        else if(field == "unknown") {
            flags->unknown = value;
        }
        else if(field == "canBlink") {
            flags->can_blink = value;
        }
        else if(field == "impervious") {
            flags->impervious = value;
        }
        else if(field == "suspended") {
            flags->suspended = value;
        }
        else if(field == "blind") {
            flags->blind = value;
        }
        else if(field == "unknownNvRelated") {
            flags->unknown_nv_related = value;
        }
        else if(field == "possessed") {
            flags->possessed = value;
        }
        else if(field == "desiresFlashlightOn") {
            flags->desires_flashlight_on = value;
        }
        else if(field == "desiresFlashlightOff") {
            flags->desires_flashlight_off = value;
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_unit_flags(lua_State *state, Engine::UnitFlags &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_unit_flags__index, lua_engine_unit_flags__newindex, read_only); 
    }

    static int lua_engine_unit_control_flags__index(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::UnitControlFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "crouch") {
            lua_pushboolean(state, flags->crouch);
        }
        else if(field == "jump") {
            lua_pushboolean(state, flags->jump);
        }
        else if(field == "user1") {
            lua_pushboolean(state, flags->user1);
        }
        else if(field == "user2") {
            lua_pushboolean(state, flags->user2);
        }
        else if(field == "light") {
            lua_pushboolean(state, flags->light);
        }
        else if(field == "exactFacing") {
            lua_pushboolean(state, flags->exact_facing);
        }
        else if(field == "action") {
            lua_pushboolean(state, flags->action);
        }
        else if(field == "melee") {
            lua_pushboolean(state, flags->melee);
        }
        else if(field == "lookDontTurn") {
            lua_pushboolean(state, flags->look_dont_turn);
        }
        else if(field == "forceAlert") {
            lua_pushboolean(state, flags->force_alert);
        }
        else if(field == "reload") {
            lua_pushboolean(state, flags->reload);
        }
        else if(field == "primaryTrigger") {
            lua_pushboolean(state, flags->primary_trigger);
        }
        else if(field == "secondaryTrigger") {
            lua_pushboolean(state, flags->secondary_trigger);
        }
        else if(field == "grenade") {
            lua_pushboolean(state, flags->grenade);
        }
        else if(field == "exchangeWeapon") {
            lua_pushboolean(state, flags->exchange_weapon);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_unit_control_flags__newindex(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::UnitControlFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        bool value = lua_toboolean(state, 3);
        if(field == "crouch") {
            flags->crouch = value;
        }
        else if(field == "jump") {
            flags->jump = value;
        }
        else if(field == "user1") {
            flags->user1 = value;
        }
        else if(field == "user2") {
            flags->user2 = value;
        }
        else if(field == "light") {
            flags->light = value;
        }
        else if(field == "exactFacing") {
            flags->exact_facing = value;
        }
        else if(field == "action") {
            flags->action = value;
        }
        else if(field == "melee") {
            flags->melee = value;
        }
        else if(field == "lookDontTurn") {
            flags->look_dont_turn = value;
        }
        else if(field == "forceAlert") {
            flags->force_alert = value;
        }
        else if(field == "reload") {
            flags->reload = value;
        }
        else if(field == "primaryTrigger") {
            flags->primary_trigger = value;
        }
        else if(field == "secondaryTrigger") {
            flags->secondary_trigger = value;
        }
        else if(field == "grenade") {
            flags->grenade = value;
        }
        else if(field == "exchangeWeapon") {
            flags->exchange_weapon = value;
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_unit_control_flags(lua_State *state, Engine::UnitControlFlags &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_unit_control_flags__index, lua_engine_unit_control_flags__newindex, read_only); 
    }

    static int lua_engine_unit_animation_state_data__index(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::UnitAnimationStateData>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "animationIndex") {
            lua_pushinteger(state, data->animation_index);
        }
        else if(field == "frameIndex") {
            lua_pushinteger(state, data->frame_index);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_unit_animation_state_data__newindex(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::UnitAnimationStateData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isinteger(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        std::int16_t value = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        if(field == "animationIndex") {
            data->animation_index = value;
        }
        else if(field == "frameIndex") {
            data->frame_index = value;
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_unit_animation_state_data(lua_State *state, Engine::UnitAnimationStateData &data, bool read_only) noexcept {
        lua_push_meta_object(state, data, lua_engine_unit_animation_state_data__index, lua_engine_unit_animation_state_data__newindex, read_only); 
    }

    static int lua_engine_unit_animation_flags__index(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::UnitAnimationFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "animationBit0Unknown") {
            lua_pushboolean(state, flags->animation_bit0_unknown);
        }
        else if(field == "animationBit1Unknown") {
            lua_pushboolean(state, flags->animation_bit1_unknown);
        }
        else if(field == "animationBit2Unknown") {
            lua_pushboolean(state, flags->animation_bit2_unknown);
        }
        else if(field == "animationBit3Unknown") {
            lua_pushboolean(state, flags->animation_bit3_unknown);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_unit_animation_flags__newindex(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::UnitAnimationFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        bool value = lua_toboolean(state, 3);
        if(field == "animationBit0Unknown") {
            flags->animation_bit0_unknown = value;
        }
        else if(field == "animationBit1Unknown") {
            flags->animation_bit1_unknown = value;
        }
        else if(field == "animationBit2Unknown") {
            flags->animation_bit2_unknown = value;
        }
        else if(field == "animationBit3Unknown") {
            flags->animation_bit3_unknown = value;
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_unit_animation_flags(lua_State *state, Engine::UnitAnimationFlags &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_unit_animation_flags__index, lua_engine_unit_animation_flags__newindex, read_only); 
    }

    static int lua_engine_unit_animation_data__index(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::UnitAnimationData>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "flags") {
            lua_push_meta_engine_unit_animation_flags(state, data->flags);
        }
        else if(field == "unknownSomeAnimationIndexMaybe") {
            lua_pushinteger(state, data->unknown_some_animation_index_maybe);
        }
        else if(field == "unknownSomeAnimationIndex") {
            lua_pushinteger(state, data->unknown_some_animation_index);
        }
        else if(field == "seatIndex") {
            lua_pushinteger(state, data->seat_index);
        }
        else if(field == "seatWeaponIndex") {
            lua_pushinteger(state, data->seat_weapon_index);
        }
        else if(field == "weaponTypeIndex") {
            lua_pushinteger(state, data->weapon_type_index);
        }
        else if(field == "state") {
            lua_pushstring(state, unit_animation_state_to_string(data->state).c_str());
        }
        else if(field == "replacementState") {
            lua_pushstring(state, unit_replacement_animation_state_to_string(data->replacement_state).c_str());
        }
        else if(field == "overlayState") {
            lua_pushstring(state, unit_overlay_animation_state_to_string(data->overlay_state).c_str());
        }
        else if(field == "desiredState") {
            lua_pushstring(state, unit_animation_state_to_string(data->desired_state).c_str());
        }
        else if(field == "baseSeat") {
            lua_pushstring(state, unit_base_seat_to_string(data->base_seat).c_str());
        }
        else if(field == "emotion") {
            lua_pushinteger(state, data->emotion);
        }
        else if(field == "replacementAnimation") {
            lua_push_meta_engine_unit_animation_state_data(state, data->replacement_animation);
        }
        else if(field == "overlayStateAnimation") {
            lua_push_meta_engine_unit_animation_state_data(state, data->overlay_state_animation);
        }
        else if(field == "weaponIk") {
            lua_push_meta_engine_unit_animation_state_data(state, data->weapon_ik);
        }
        else if(field == "updateLook") {
            lua_pushboolean(state, data->update_look);
        }
        else if(field == "updateAim") {
            lua_pushboolean(state, data->update_aim);
        }
        else if(field == "lookingBounds") {
            lua_push_meta_engine_rectangle2_d_f(state, data->looking_bounds);
        }
        else if(field == "aimingBounds") {
            lua_push_meta_engine_rectangle2_d_f(state, data->aiming_bounds);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_unit_animation_data__newindex(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::UnitAnimationData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "flags") {
            lua_push_meta_engine_unit_animation_flags(state, data->flags);
        }
        else if(field == "unknownSomeAnimationIndexMaybe") {
            data->unknown_some_animation_index_maybe = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknownSomeAnimationIndex") {
            data->unknown_some_animation_index = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "seatIndex") {
            data->seat_index = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "seatWeaponIndex") {
            data->seat_weapon_index = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "weaponTypeIndex") {
            data->weapon_type_index = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "state") {
            try {
                data->state = unit_animation_state_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "replacementState") {
            try {
                data->replacement_state = unit_replacement_animation_state_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "overlayState") {
            try {
                data->overlay_state = unit_overlay_animation_state_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "desiredState") {
            try {
                data->desired_state = unit_animation_state_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "baseSeat") {
            try {
                data->base_seat = unit_base_seat_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "emotion") {
            data->emotion = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "replacementAnimation") {
            lua_push_meta_engine_unit_animation_state_data(state, data->replacement_animation);
        }
        else if(field == "overlayStateAnimation") {
            lua_push_meta_engine_unit_animation_state_data(state, data->overlay_state_animation);
        }
        else if(field == "weaponIk") {
            lua_push_meta_engine_unit_animation_state_data(state, data->weapon_ik);
        }
        else if(field == "updateLook") {
            if(lua_isboolean(state, 3)) {
                data->update_look = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "updateAim") {
            if(lua_isboolean(state, 3)) {
                data->update_aim = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "lookingBounds") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "aimingBounds") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_unit_animation_data(lua_State *state, Engine::UnitAnimationData &data, bool read_only) noexcept {
        lua_push_meta_object(state, data, lua_engine_unit_animation_data__index, lua_engine_unit_animation_data__newindex, read_only); 
    }

    static int lua_engine_ai_communication_packet__index(lua_State *state) noexcept {
        auto *packet = lua_from_meta_object<Engine::AiCommunicationPacket>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "type") {
            lua_pushstring(state, ai_communication_type_to_string(packet->type).c_str());
        }
        else if(field == "broken") {
            lua_pushboolean(state, packet->broken);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_ai_communication_packet__newindex(lua_State *state) noexcept {
        auto *packet = lua_from_meta_object<Engine::AiCommunicationPacket>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key"); 
        } 
        
        std::string field = key;
        if(field == "type") {
            try {
                packet->type = ai_communication_type_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "broken") {
            if(lua_isboolean(state, 3)) {
                packet->broken = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_ai_communication_packet(lua_State *state, Engine::AiCommunicationPacket &packet, bool read_only) noexcept {
        lua_push_meta_object(state, packet, lua_engine_ai_communication_packet__index, lua_engine_ai_communication_packet__newindex, read_only); 
    }

    static int lua_engine_unit_speech__index(lua_State *state) noexcept {
        auto *speech = lua_from_meta_object<Engine::UnitSpeech>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "priority") {
            lua_pushstring(state, unit_speech_priority_to_string(speech->priority).c_str());
        }
        else if(field == "screamType") {
            lua_pushstring(state, unit_scream_type_to_string(speech->scream_type).c_str());
        }
        else if(field == "soundTag") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&speech->sound_tag));
        }
        else if(field == "ticks") {
            lua_pushinteger(state, speech->ticks);
        }
        else if(field == "unknown2") {
            lua_pushinteger(state, speech->unknown2);
        }
        else if(field == "unknown") {
            lua_pushinteger(state, speech->unknown);
        }
        else if(field == "aiCommunicationInfo") {
            lua_push_meta_engine_ai_communication_packet(state, speech->ai_communication_info);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_unit_speech__newindex(lua_State *state) noexcept {
        auto *speech = lua_from_meta_object<Engine::UnitSpeech>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key"); 
        } 
        
        std::string field = key;
        if(field == "priority") {
            try {
                speech->priority = unit_speech_priority_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "screamType") {
            try {
                speech->scream_type = unit_scream_type_from_string(luaL_checkstring(state, 3));
            }
            catch(std::runtime_error &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "soundTag") {
            speech->sound_tag.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "ticks") {
            speech->ticks = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown2") {
            speech->unknown2 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown") {
            speech->unknown = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "aiCommunicationInfo") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_unit_speech(lua_State *state, Engine::UnitSpeech &speech, bool read_only) noexcept {
        lua_push_meta_object(state, speech, lua_engine_unit_speech__index, lua_engine_unit_speech__newindex, read_only); 
    }

    static int lua_engine_unit_speech_data__index(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::UnitSpeechData>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "current") {
            lua_push_meta_engine_unit_speech(state, data->current);
        }
        else if(field == "next") {
            lua_push_meta_engine_unit_speech(state, data->next);
        }
        else if(field == "unknown0") {
            lua_pushinteger(state, data->unknown0);
        }
        else if(field == "unknown1") {
            lua_pushinteger(state, data->unknown1);
        }
        else if(field == "unknown2") {
            lua_pushinteger(state, data->unknown2);
        }
        else if(field == "unknown3") {
            lua_pushinteger(state, data->unknown3);
        }
        else if(field == "unknown4") {
            lua_pushinteger(state, data->unknown4);
        }
        else if(field == "unknown6") {
            lua_pushboolean(state, data->unknown6);
        }
        else if(field == "unknown7") {
            lua_pushboolean(state, data->unknown7);
        }
        else if(field == "unknown8") {
            lua_pushboolean(state, data->unknown8);
        }
        else if(field == "unknown9") {
            lua_pushinteger(state, data->unknown9);
        }
        else if(field == "unknown10") {
            lua_pushinteger(state, data->unknown10);
        }
        else if(field == "unknown11") {
            lua_pushinteger(state, data->unknown11);
        }
        else if(field == "unknown12") {
            lua_pushinteger(state, data->unknown12);
        }
        else if(field == "unknown13") {
            lua_pushinteger(state, data->unknown13);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_unit_speech_data__newindex(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::UnitSpeechData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key"); 
        } 
        
        std::string field = key;
        if(field == "current") {
            lua_push_meta_engine_unit_speech(state, data->current);
        }
        else if(field == "next") {
            lua_push_meta_engine_unit_speech(state, data->next);
        }
        else if(field == "unknown0") {
            data->unknown0 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown1") {
            data->unknown1 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown2") {
            data->unknown2 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown3") {
            data->unknown3 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown4") {
            data->unknown4 = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown6") {
            if(lua_isboolean(state, 3)) {
                data->unknown6 = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "unknown7") {
            if(lua_isboolean(state, 3)) {
                data->unknown7 = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "unknown8") {
            if(lua_isboolean(state, 3)) {
                data->unknown8 = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "unknown9") {
            data->unknown9 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown10") {
            data->unknown10 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown11") {
            data->unknown11 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown12") {
            data->unknown12 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown13") {
            data->unknown13 = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_unit_speech_data(lua_State *state, Engine::UnitSpeechData &data, bool read_only) noexcept {
        lua_push_meta_object(state, data, lua_engine_unit_speech_data__index, lua_engine_unit_speech_data__newindex, read_only); 
    }

    static int lua_engine_unit_control_data__index(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::UnitControlData>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "animationState") {
            lua_pushinteger(state, data->animation_state);
        }
        else if(field == "aimingSpeed") {
            lua_pushinteger(state, data->aiming_speed);
        }
        else if(field == "controlFlags") {
            lua_push_meta_engine_unit_control_flags(state, data->control_flags);
        }
        else if(field == "weaponIndex") {
            lua_pushinteger(state, data->weapon_index);
        }
        else if(field == "grenadeIndex") {
            lua_pushinteger(state, data->grenade_index);
        }
        else if(field == "zoomIndex") {
            lua_pushinteger(state, data->zoom_index);
        }
        else if(field == "throttle") {
            lua_push_meta_engine_vector3_d(state, data->throttle);
        }
        else if(field == "primaryTrigger") {
            lua_pushnumber(state, data->primary_trigger);
        }
        else if(field == "facingVector") {
            lua_push_meta_engine_vector3_d(state, data->facing_vector);
        }
        else if(field == "aimingVector") {
            lua_push_meta_engine_vector3_d(state, data->aiming_vector);
        }
        else if(field == "lookingVector") {
            lua_push_meta_engine_vector3_d(state, data->looking_vector);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_unit_control_data__newindex(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::UnitControlData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isnumber(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "animationState") {
            data->animation_state = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "aimingSpeed") {
            data->aiming_speed = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "controlFlags") {
            lua_push_meta_engine_unit_control_flags(state, data->control_flags);
        }
        else if(field == "weaponIndex") {
            data->weapon_index = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "grenadeIndex") {
            data->grenade_index = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "zoomIndex") {
            data->zoom_index = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "throttle") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "primaryTrigger") {
            data->primary_trigger = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "facingVector") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "aimingVector") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "lookingVector") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_unit_control_data(lua_State *state, Engine::UnitControlData &data, bool read_only) noexcept {
        lua_push_meta_object(state, data, lua_engine_unit_control_data__index, lua_engine_unit_control_data__newindex, read_only); 
    }

    extern std::string lua_engine_damage_effect_category_to_string(Engine::TagDefinitions::DamageEffectCategory value) noexcept; 
    extern Engine::TagDefinitions::DamageEffectCategory lua_engine_damage_effect_category_from_string(std::string str); 

    static int lua_engine_unit_object__index(lua_State *state) noexcept {
        auto *unit = lua_from_meta_object<Engine::UnitObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "actor") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->actor));
        }
        else if(field == "swarmActor") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->swarm.actor));
        }
        else if(field == "swarmNextUnit") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->swarm.next_unit));
        }
        else if(field == "swarmPreviousUnit") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->swarm.previous_unit));
        }
        else if(field == "unitFlags") {
            lua_push_meta_engine_unit_flags(state, unit->unit_flags);
        }
        else if(field == "unitControlFlags") {
            lua_push_meta_engine_unit_control_flags(state, unit->unit_control_flags);
        }
        else if(field == "shieldSnapping") {
            lua_pushinteger(state, unit->shield_snapping);
        }
        else if(field == "baseSeatIndex") {
            lua_pushinteger(state, unit->base_seat_index);
        }
        else if(field == "persistentControlTicksRemaining") {
            lua_pushinteger(state, unit->persistent_control.ticks_remaining);
        }
        else if(field == "persistentControlControlFlags") {
            lua_push_meta_engine_unit_control_flags(state, unit->persistent_control.control_flags);
        }
        else if(field == "controllingPlayer") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->controlling_player));
        }
        else if(field == "aiEffectType") {
            lua_pushinteger(state, unit->ai_effect_type);
        }
        else if(field == "emotionAnimationIndex") {
            lua_pushinteger(state, unit->emotion_animation_index);
        }
        else if(field == "nextAiEffectTick") {
            lua_pushinteger(state, unit->next_ai_effect_tick);
        }
        else if(field == "desiredFacingVector") {
            lua_push_meta_engine_vector3_d(state, unit->desired_facing_vector);
        }
        else if(field == "desiredAimingVector") {
            lua_push_meta_engine_vector3_d(state, unit->desired_aiming_vector);
        }
        else if(field == "aimingVector") {
            lua_push_meta_engine_vector3_d(state, unit->aiming_vector);
        }
        else if(field == "aimingVelocity") {
            lua_push_meta_engine_vector3_d(state, unit->aiming_velocity);
        }
        else if(field == "lookingAngles") {
            lua_push_meta_engine_euler3_d(state, unit->looking_angles);
        }
        else if(field == "lookingVector") {
            lua_push_meta_engine_vector3_d(state, unit->looking_vector);
        }
        else if(field == "lookingVelocity") {
            lua_push_meta_engine_vector3_d(state, unit->looking_velocity);
        }
        else if(field == "throttle") {
            lua_push_meta_engine_vector3_d(state, unit->throttle);
        }
        else if(field == "primaryTrigger") {
            lua_pushnumber(state, unit->primary_trigger);
        }
        else if(field == "aimingSpeed") {
            lua_pushinteger(state, unit->aiming_speed);
        }
        else if(field == "meleeState") {
            lua_pushinteger(state, unit->melee_state);
        }
        else if(field == "meleeTimer") {
            lua_pushinteger(state, unit->melee_timer);
        }
        else if(field == "ticksUntilFlameToDeath") {
            lua_pushinteger(state, unit->ticks_until_flame_to_death);
        }
        else if(field == "pingAnimationTicksLeft") {
            lua_pushinteger(state, unit->ping_animation_ticks_left);
        }
        else if(field == "grenadeState") {
            lua_pushstring(state, unit_throwing_grenade_state_to_string(unit->grenade_state).c_str());
        }
        else if(field == "unknown725") {
            lua_pushinteger(state, unit->unknown_725);
        }
        else if(field == "unknown726") {
            lua_pushinteger(state, unit->unknown_726);
        }
        else if(field == "grenadeProjectile") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->grenade_projectile));
        }
        else if(field == "animation") {
            lua_push_meta_engine_unit_animation_data(state, unit->animation);
        }
        else if(field == "ambient") {
            lua_pushnumber(state, unit->ambient);
        }
        else if(field == "illumination") {
            lua_pushnumber(state, unit->illumination);
        }
        else if(field == "mouthFactor") {
            lua_pushnumber(state, unit->mouth_factor);
        }
        else if(field == "vehicleSeatId") {
            lua_pushinteger(state, unit->vehicle_seat_id);
        }
        else if(field == "currentWeaponId") {
            lua_pushinteger(state, unit->current_weapon_id);
        }
        else if(field == "nextWeaponId") {
            lua_pushinteger(state, unit->next_weapon_id);
        }
        else if(field == "weapons") {
            lua_newtable(state);
            for(std::size_t i = 0; i < 4; ++i) {
                lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->weapons[i]));
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "weaponReadyTicks") {
            lua_newtable(state);
            for(std::size_t i = 0; i < 4; ++i) {
                lua_pushinteger(state, unit->weapon_ready_ticks[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "equipmentHandle") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->equipment_handle));
        }
        else if(field == "currentGrenadeIndex") {
            lua_pushinteger(state, unit->current_grenade_index);
        }
        else if(field == "nextGrenadeIndex") {
            lua_pushinteger(state, unit->next_grenade_index);
        }
        else if(field == "grenadeCounts") {
            lua_newtable(state);
            for(std::size_t i = 0; i < 2; ++i) {
                lua_pushinteger(state, unit->grenade_counts[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "zoomLevel") {
            lua_pushinteger(state, unit->zoom_level);
        }
        else if(field == "desiredZoomLevel") {
            lua_pushinteger(state, unit->desired_zoom_level);
        }
        else if(field == "ticksSinceLastVehicleSpeech") {
            lua_pushinteger(state, unit->ticks_since_last_vehicle_speech);
        }
        else if(field == "aimingChange") {
            lua_pushinteger(state, unit->aiming_change);
        }
        else if(field == "poweredSeatsRiders") {
            lua_newtable(state);
            for(std::size_t i = 0; i < 2; ++i) {
                lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->powered_seats_riders[i]));
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "_unknown22") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->_unknown22));
        }
        else if(field == "_someTickTime") {
            lua_pushinteger(state, unit->_some_tick_time);
        }
        else if(field == "encounterId") {
            lua_pushinteger(state, unit->encounter_id);
        }
        else if(field == "squadId") {
            lua_pushinteger(state, unit->squad_id);
        }
        else if(field == "poweredSeatsPower") {
            lua_newtable(state);
            for(std::size_t i = 0; i < 2; ++i) {
                lua_pushnumber(state, unit->powered_seats_power[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "integratedLightPower") {
            lua_pushnumber(state, unit->integrated_light_power);
        }
        else if(field == "integratedLightTogglePower") {
            lua_pushnumber(state, unit->integrated_light_toggle_power);
        }
        else if(field == "integratedNightVisionTogglePower") {
            lua_pushnumber(state, unit->integrated_night_vision_toggle_power);
        }
        else if(field == "seatRelated") {
            lua_newtable(state);
            for(std::size_t i = 0; i < 4; ++i) {
                lua_push_meta_engine_vector3_d(state, unit->seat_related[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "camoPower") {
            lua_pushnumber(state, unit->camo_power);
        }
        else if(field == "fullSpectrumVisionPower") {
            lua_pushnumber(state, unit->full_spectrum_vision_power);
        }
        else if(field == "dialogueDefinition") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->dialogue_definition));
        }
        else if(field == "speech") {
            lua_push_meta_engine_unit_speech_data(state, unit->speech, false);
        }
        else if(field == "damageResultCategory") {
            lua_pushstring(state, lua_engine_damage_effect_category_to_string(unit->damage_result.category).c_str());
        }
        else if(field == "damageResultAiTicksUntilHandle") {
            lua_pushinteger(state, unit->damage_result.ai_ticks_until_handle);
        }
        else if(field == "damageResultAmount") {
            lua_pushnumber(state, unit->damage_result.amount);
        }
        else if(field == "damageResultResponsibleUnit") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->damage_result.responsible_unit));
        }
        else if(field == "objectFlameCauser") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&unit->object_flame_causer));
        }
        else if(field == "_unknown23") {
            lua_pushnumber(state, unit->_unknown23);
        }
        else if(field == "diedAtTick") {
            lua_pushinteger(state, unit->died_at_tick);
        }
        else if(field == "feignDeathTimer") {
            lua_pushinteger(state, unit->feign_death_timer);
        }
        else if(field == "camoRegrowth") {
            lua_pushboolean(state, unit->camo_regrowth);
        }
        else if(field == "stun") {
            lua_pushnumber(state, unit->stun);
        }
        else if(field == "stunTicks") {
            lua_pushinteger(state, unit->stun_ticks);
        }
        else if(field == "spreeCount") {
            lua_pushinteger(state, unit->spree_count);
        }
        else if(field == "spreeStartingTime") {
            lua_pushinteger(state, unit->spree_starting_time);
        }
        else if(field == "recentDamage") {
            lua_newtable(state);
            for(std::size_t i = 0; i < 4; ++i) {
                lua_push_meta_engine_unit_recent_damager(state, unit->recent_damage[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "opensauceZoomLevel") {
            lua_pushinteger(state, unit->opensauce_zoom_level);
        }
        else if(field == "opensauceDesiredZoomLevel") {
            lua_pushinteger(state, unit->opensauce_desired_zoom_level);
        }
        else if(field == "controlData") {
            lua_push_meta_engine_unit_control_data(state, unit->control_data);
        }
        else if(field == "lastCompletedClientUpdateValid") {
            lua_pushboolean(state, unit->last_completed_client_update_valid);
        }
        else if(field == "lastCompletedClientUpdateId") {
            lua_pushinteger(state, unit->last_completed_client_update_id);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_unit_object__newindex(lua_State *state) noexcept {
        auto *unit = lua_from_meta_object<Engine::UnitObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "actor") {
            unit->actor.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "swarmActor") {
            unit->swarm.actor.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "swarmNextUnir") {
            unit->swarm.next_unit.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "swarmPreviousUnit") {
            unit->swarm.previous_unit.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "unitFlags") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "unitControlFlags") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "ShieldSnapping") {
            unit->shield_snapping = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "BaseSeatIndex") {
            unit->base_seat_index = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "PersistentControlTicksRemaining") {
            unit->persistent_control.ticks_remaining = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "persistentControlControlFlags") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "controllingPlayer") {
            unit->controlling_player.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "aiEffectType") {
            unit->ai_effect_type = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "emotionAnimationIndex") {
            unit->emotion_animation_index = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "nextAiEffectTick") {
            unit->next_ai_effect_tick = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "desiredFacingVector") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "desiredAimingVector") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "aimingVector") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "aimingVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "lookingAngles") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "lookingVector") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "lookingVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "throttle") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "primaryTrigger") {
            unit->primary_trigger = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "aimingSpeed") {
            unit->aiming_speed = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "meleeState") {
            unit->melee_state = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "meleeTimer") {
            unit->melee_timer = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "ticksUntilFlameToDeath") {
            unit->ticks_until_flame_to_death = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "pingAnimationTicksLeft") {
            unit->ping_animation_ticks_left = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "grenadeState") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "unknown725") {
            unit->unknown_725 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown726") {
            unit->unknown_726 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "grenadeProjectile") {
            unit->grenade_projectile.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "animation") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "ambient") {
            unit->ambient = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "illumination") {
            unit->illumination = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "mouthFactor") {
            unit->mouth_factor = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "vehicleSeatId") {
            unit->vehicle_seat_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "currentWeaponId") {
            unit->current_weapon_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "nextWeaponId") {
            unit->next_weapon_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "weapons") {
            for(std::size_t i = 0; i < sizeof(unit->weapons) / sizeof(unit->weapons[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    unit->weapons[i] = Engine::ObjectHandle(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "weaponReadyTicks") {
            for(std::size_t i = 0; i < sizeof(unit->weapon_ready_ticks) / sizeof(unit->weapon_ready_ticks[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    unit->weapon_ready_ticks[i] = static_cast<std::int16_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "equipmentHandle") {
            unit->equipment_handle.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "currentGrenadeIndex") {
            unit->current_grenade_index = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "nextGrenadeIndex") {
            unit->next_grenade_index = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "grenadeCounts") {
            for(std::size_t i = 0; i < sizeof(unit->grenade_counts) / sizeof(unit->grenade_counts[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    unit->grenade_counts[i] = static_cast<std::uint8_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "zoomLevel") {
            unit->zoom_level = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "desiredZoomLevel") {
            unit->desired_zoom_level = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "ticksSinceLastVehicleSpeech") {
            unit->ticks_since_last_vehicle_speech = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "aimingChange") {
            unit->aiming_change = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "poweredSeatsRiders") {
            for(std::size_t i = 0; i < sizeof(unit->powered_seats_riders) / sizeof(unit->powered_seats_riders[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    unit->powered_seats_riders[i] = Engine::ObjectHandle(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "_unknown22") {
            unit->_unknown22.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "_someTickTime") {
            unit->_some_tick_time = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "encounterId") {
            unit->encounter_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "squadId") {
            unit->squad_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "poweredSeatsPower") {
            for(std::size_t i = 0; i < sizeof(unit->powered_seats_power) / sizeof(unit->powered_seats_power[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    unit->powered_seats_power[i] = static_cast<float>(luaL_checknumber(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "integratedLightPower") {
            unit->integrated_light_power = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "integratedLightTogglePower") {
            unit->integrated_light_toggle_power = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "integratedNightVisionTogglePower") {
            unit->integrated_night_vision_toggle_power = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "seatRelated") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "camoPower") {
            unit->camo_power = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "fullSpectrumVisionPower") {
            unit->full_spectrum_vision_power = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "dialogueDefinition") {
            unit->dialogue_definition.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "speech") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "damageResultCatagory") {
            try {
                unit->damage_result.category = lua_engine_damage_effect_category_from_string(luaL_checkstring(state, 3));
            }
            catch(const std::exception &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "damageResultAiTicksUntilHandle") {
            unit->damage_result.ai_ticks_until_handle = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "damageResultAmount") {
            unit->damage_result.amount = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "damageResultResponsibleUnit") {
            unit->damage_result.responsible_unit.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "objectFlameCauser") {
            unit->object_flame_causer.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "_unknown23") {
            unit->_unknown23 = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "diedAtTick") {
            unit->died_at_tick = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "feignDeathTimer") {
            unit->feign_death_timer = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "camoRegrowth") {
            if(lua_isboolean(state, 3)) {
                unit->camo_regrowth = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "stun") {
            unit->stun = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "stunTicks") {
            unit->stun_ticks = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "spreeCount") {
            unit->spree_count = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "spreeStartingTime") {
            unit->spree_starting_time = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "recentDamage") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "opensauceZoomLevel") {
            unit->opensauce_zoom_level = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "opensauceDesiredZoomLevel") {
            unit->opensauce_desired_zoom_level = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "controlData") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "lastCompletedClientUpdateValid") {
            if(lua_isboolean(state, 3)) {
                unit->last_completed_client_update_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "lastCompletedClientUpdateId") {
            unit->last_completed_client_update_id = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_unit_object(lua_State *state, Engine::UnitObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_unit_object__index, lua_engine_unit_object__newindex, read_only);
    }

    static int lua_engine_biped_flags__index(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::BipedFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "airborne") {
            lua_pushboolean(state, flags->airborne);
        }
        else if(field == "slipping") {
            lua_pushboolean(state, flags->slipping);
        }
        else if(field == "absoluteMovement") {
            lua_pushboolean(state, flags->absolute_movement);
        }
        else if(field == "noCollision") {
            lua_pushboolean(state, flags->no_collision);
        }
        else if(field == "passesThroughOtherBipeds") {
            lua_pushboolean(state, flags->passes_through_other_bipeds);
        }
        else if(field == "limping2") {
            lua_pushboolean(state, flags->limping2);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_biped_flags__newindex(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::BipedFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "airborne") {
            flags->airborne = lua_toboolean(state, 3);
        }
        else if(field == "slipping") {
            flags->slipping = lua_toboolean(state, 3);
        }
        else if(field == "absoluteMovement") {
            flags->absolute_movement = lua_toboolean(state, 3);
        }
        else if(field == "noCollision") {
            flags->no_collision = lua_toboolean(state, 3);
        }
        else if(field == "passesThroughOtherBipeds") {
            flags->passes_through_other_bipeds = lua_toboolean(state, 3);
        }
        else if(field == "limping2") {
            flags->limping2 = lua_toboolean(state, 3);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_biped_flags(lua_State *state, Engine::BipedFlags &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_biped_flags__index, lua_engine_biped_flags__newindex, read_only);
    }

    static int lua_engine_biped_network_delta__index(lua_State *state) noexcept {
        auto *delta = lua_from_meta_object<Engine::BipedNetworkDelta>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "grenadeCounts") {
            lua_newtable(state);
            for(std::size_t i = 0; i < 2; ++i) {
                lua_pushinteger(state, delta->grenade_counts[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "bodyVitality") {
            lua_pushnumber(state, delta->body_vitality);
        }
        else if(field == "shieldVitality") {
            lua_pushnumber(state, delta->shield_vitality);
        }
        else if(field == "shieldStunTicksGreaterThanZero") {
            lua_pushboolean(state, delta->shield_stun_ticks_greater_than_zero);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_biped_network_delta__newindex(lua_State *state) noexcept {
        auto *delta = lua_from_meta_object<Engine::BipedNetworkDelta>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "grenadeCounts") {
            for(std::size_t i = 0; i < sizeof(delta->grenade_counts) / sizeof(delta->grenade_counts[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    delta->grenade_counts[i] = static_cast<std::int8_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "bodyVitality") {
            delta->body_vitality = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "shieldVitality") {
            delta->shield_vitality = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "shieldStunTicksGreaterThanZero") {
            if(lua_isboolean(state, 3)) {
                delta->shield_stun_ticks_greater_than_zero = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_biped_network_delta(lua_State *state, Engine::BipedNetworkDelta &delta, bool read_only) noexcept {
        lua_push_meta_object(state, delta, lua_engine_biped_network_delta__index, lua_engine_biped_network_delta__newindex, read_only);
    }

    static int lua_engine_biped_network__index(lua_State *state) noexcept {
        auto *network = lua_from_meta_object<Engine::BipedNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "baselineValid") {
            lua_pushboolean(state, network->baseline_valid);
        }
        else if(field == "baselineId") {
            lua_pushinteger(state, network->baseline_id);
        }
        else if(field == "messageId") {
            lua_pushinteger(state, network->message_id);
        }
        else if(field == "updateBaseline") {
            lua_push_meta_engine_biped_network_delta(state, network->update_baseline);
        }
        else if(field == "deltaValid") {
            lua_pushboolean(state, network->delta_valid);
        }
        else if(field == "updateDelta") {
            lua_push_meta_engine_biped_network_delta(state, network->update_delta);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_biped_network__newindex(lua_State *state) noexcept {
        auto *network = lua_from_meta_object<Engine::BipedNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "baselineValid") {
            if(lua_isboolean(state, 3)) {
                network->baseline_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "baselineId") {
            network->baseline_id = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "messageId") {
            network->message_id = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "updateBaseline") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "deltaValid") {
            if(lua_isboolean(state, 3)) {
                network->delta_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "updateDelta") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_biped_network(lua_State *state, Engine::BipedNetwork &network, bool read_only) noexcept {
        lua_push_meta_object(state, network, lua_engine_biped_network__index, lua_engine_biped_network__newindex, read_only);
    }

    static int lua_engine_biped_object__index(lua_State *state) noexcept {
        auto *biped = lua_from_meta_object<Engine::BipedObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "bipedFlags") {
            lua_push_meta_engine_biped_flags(state, biped->biped_flags);
        }
        else if(field == "landingTimer") {
            lua_pushinteger(state, biped->landing_timer);
        }
        else if(field == "landingForce") {
            lua_pushinteger(state, biped->landing_force);
        }
        else if(field == "movementState") {
            lua_pushstring(state, biped_movement_state_to_string(biped->movement_state).c_str());
        }
        else if(field == "_bipedUnknown3") {
            lua_pushinteger(state, biped->_biped_unknown3);
        }
        else if(field == "actionFlags") {
            lua_pushinteger(state, biped->action_flags);
        }
        else if(field == "_bipedUnknown4") {
            lua_pushinteger(state, biped->_biped_unknown4);
        }
        else if(field == "bipedPosition") {
            lua_push_meta_engine_vector3_d(state, biped->biped_position);
        }
        else if(field == "walkingCounter") {
            lua_pushinteger(state, biped->walking_counter);
        }
        else if(field == "bumpObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&biped->bump_object));
        }
        else if(field == "ticksSinceLastBump") {
            lua_pushinteger(state, biped->ticks_since_last_bump);
        }
        else if(field == "airborneTicks") {
            lua_pushinteger(state, biped->airborne_ticks);
        }
        else if(field == "slippingTicks") {
            lua_pushinteger(state, biped->slipping_ticks);
        }
        else if(field == "digitalThrottle") {
            lua_pushinteger(state, biped->digital_throttle);
        }
        else if(field == "jumpTicks") {
            lua_pushinteger(state, biped->jump_ticks);
        }
        else if(field == "meleeTicks") {
            lua_pushinteger(state, biped->melee_ticks);
        }
        else if(field == "meleeInflictTicks") {
            lua_pushinteger(state, biped->melee_inflict_ticks);
        }
        else if(field == "unknownBiped2") {
            lua_pushinteger(state, biped->unknown_biped2);
        }
        else if(field == "crouchScale") {
            lua_pushnumber(state, biped->crouch_scale);
        }
        else if(field == "unknownBiped1") {
            lua_pushnumber(state, biped->unknown_biped1);
        }
        else if(field == "unknownBipedPhysicsRelated") {
            lua_push_meta_engine_plane3_d(state, biped->unknown_biped_physics_related);
        }
        else if(field == "network") {
            lua_push_meta_engine_biped_network(state, biped->network);
        }
        else {
            return lua_engine_unit_object__index(state);
        }
        return 1;
    }

    static int lua_engine_biped_object__newindex(lua_State *state) noexcept {
        auto *biped = lua_from_meta_object<Engine::BipedObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "bipedFlags") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "landingTimer") {
            biped->landing_timer = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "landingForce") {
            biped->landing_force = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "movementState") {
            try {
                biped->movement_state = biped_movement_state_from_string(luaL_checkstring(state, 3));
            }
            catch(const std::exception &e) {
                return luaL_error(state, e.what());
            }
        }
        else if(field == "_bipedUnknown3") {
            biped->_biped_unknown3 = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "actionFlags") {
            biped->action_flags = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "_bipedUnknown4") {
            biped->_biped_unknown4 = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "bipedPosition") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "walkingCounter") {
            biped->walking_counter = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "bumpObject") {
            biped->bump_object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "ticksSinceLastBump") {
            biped->ticks_since_last_bump = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "airborneTicks") {
            biped->airborne_ticks = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "slippingTicks") {
            biped->slipping_ticks = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "digitalThrottle") {
            biped->digital_throttle = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "jumpTicks") {
            biped->jump_ticks = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "meleeTicks") {
            biped->melee_ticks = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "meleeInflictTicks") {
            biped->melee_inflict_ticks = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknownBiped2") {
            biped->unknown_biped2 = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "crouchScale") {
            biped->crouch_scale = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "unknownBiped1") {
            biped->unknown_biped1 = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "unknownBipedPhysicsRelated") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "network") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return lua_engine_unit_object__newindex(state);
        }
        return 0;
    }

    void lua_push_meta_engine_biped_object(lua_State *state, Engine::BipedObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_biped_object__index, lua_engine_biped_object__newindex, read_only);
    }

    static int lua_engine_vehicle_flags__index(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::VehicleFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "vehicleUnknown0") {
            lua_pushboolean(state, flags->vehicle_unknown0);
        }
        else if(field == "hovering") {
            lua_pushboolean(state, flags->hovering);
        }
        else if(field == "crouched") {
            lua_pushboolean(state, flags->crouched);
        }
        else if(field == "jumping") {
            lua_pushboolean(state, flags->jumping);
        }
        else if(field == "unknownVehicle1") {
            lua_pushboolean(state, flags->unknown_vehicle1);
        }
        else if(field == "unknownVehicle2") {
            lua_pushinteger(state, flags->unknown_vehicle2);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_vehicle_flags__newindex(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::VehicleFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "vehicleUnknown0") {
            flags->vehicle_unknown0 = lua_toboolean(state, 3);
        }
        else if(field == "hovering") {
            flags->hovering = lua_toboolean(state, 3);
        }
        else if(field == "crouched") {
            flags->crouched = lua_toboolean(state, 3);
        }
        else if(field == "jumping") {
            flags->jumping = lua_toboolean(state, 3);
        }
        else if(field == "unknownVehicle1") {
            flags->unknown_vehicle1 = lua_toboolean(state, 3);
        }
        else if(field == "unknownVehicle2") {
            flags->unknown_vehicle2 = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_vehicle_flags(lua_State *state, Engine::VehicleFlags &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_vehicle_flags__index, lua_engine_vehicle_flags__newindex, read_only);
    }

    static int lua_engine_vehicle_network_data__index(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::VehicleNetworkData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "_unknown1") {
            lua_pushboolean(state, data->at_rest);
        }
        else if(field == "_unknown2") {
            lua_push_meta_engine_vector3_d(state, data->position);
        }
        else if(field == "_unknown3") {
            lua_push_meta_engine_vector3_d(state, data->transitional_velocity);
        }
        else if(field == "_unknown4") {
            lua_push_meta_engine_vector3_d(state, data->angular_velocity);
        }
        else if(field == "_unknown5") {
            lua_push_meta_engine_vector3_d(state, data->forward);
        }
        else if(field == "_unknown6") {
            lua_push_meta_engine_vector3_d(state, data->up);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_vehicle_network_data__newindex(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::VehicleNetworkData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "_unknown1") {
            if(lua_isboolean(state, 3)) {
                data->at_rest = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "_unknown2") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "_unknown3") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "_unknown4") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "_unknown5") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "_unknown6") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_vehicle_network_data(lua_State *state, Engine::VehicleNetworkData &data, bool read_only) noexcept {
        lua_push_meta_object(state, data, lua_engine_vehicle_network_data__index, lua_engine_vehicle_network_data__newindex, read_only);
    }

    static int lua_engine_vehicle_network__index(lua_State *state) noexcept {
        auto *network = lua_from_meta_object<Engine::VehicleNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "timeValid") {
            lua_pushboolean(state, network->time_valid);
        }
        else if(field == "baselineValid") {
            lua_pushboolean(state, network->baseline_valid);
        }
        else if(field == "baselineId") {
            lua_pushinteger(state, network->baseline_id);
        }
        else if(field == "messageId") {
            lua_pushinteger(state, network->message_id);
        }
        else if(field == "updateBaseline") {
            lua_push_meta_engine_vehicle_network_data(state, network->update_baseline);
        }
        else if(field == "deltaValid") {
            lua_pushboolean(state, network->delta_valid);
        }
        else if(field == "updateDelta") {
            lua_push_meta_engine_vehicle_network_data(state, network->update_delta);
        }
        else if(field == "lastMovedAtTick") {
            lua_pushinteger(state, network->last_moved_at_tick);
        }
        else if(field == "scenarioRespawnId") {
            lua_pushinteger(state, network->scenario_respawn_id);
        }
        else if(field == "respawnPosition") {
            lua_push_meta_engine_vector3_d(state, network->respawn_position);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_vehicle_network__newindex(lua_State *state) noexcept {
        auto *network = lua_from_meta_object<Engine::VehicleNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "timeValid") {
            if(lua_isboolean(state, 3)) {
                network->time_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "baselineValid") {
            if(lua_isboolean(state, 3)) {
                network->baseline_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "baselineId") {
            network->baseline_id = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "messageId") {
            network->message_id = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "updateBaseline") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "deltaValid") {
            if(lua_isboolean(state, 3)) {
                network->delta_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "updateDelta") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "lastMovedAtTick") {
            network->last_moved_at_tick = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "scenarioRespawnId") {
            network->scenario_respawn_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "respawnPosition") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_vehicle_network(lua_State *state, Engine::VehicleNetwork &network, bool read_only) noexcept {
        lua_push_meta_object(state, network, lua_engine_vehicle_network__index, lua_engine_vehicle_network__newindex, read_only);
    }

    static int lua_engine_vehicle_object__index(lua_State *state) noexcept {
        auto *vehicle = lua_from_meta_object<Engine::VehicleObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "vehicleFlags") {
            lua_push_meta_engine_vehicle_flags(state, vehicle->vehicle_flags);
        }
        else if(field == "speed") {
            lua_pushnumber(state, vehicle->speed);
        }
        else if(field == "slide") {
            lua_pushnumber(state, vehicle->slide);
        }
        else if(field == "turn") {
            lua_pushnumber(state, vehicle->turn);
        }
        else if(field == "tirePosition") {
            lua_pushnumber(state, vehicle->tire_position);
        }
        else if(field == "threadPositionLeft") {
            lua_pushnumber(state, vehicle->thread_position_left);
        }
        else if(field == "threadPositionRight") {
            lua_pushnumber(state, vehicle->thread_position_right);
        }
        else if(field == "hover") {
            lua_pushnumber(state, vehicle->hover);
        }
        else if(field == "thrust") {
            lua_pushnumber(state, vehicle->thrust);
        }
        else if(field == "suspensionStates") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(vehicle->suspension_states) / sizeof(vehicle->suspension_states[0]); i++) {
                lua_pushinteger(state, vehicle->suspension_states[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "hoverPosition") {
            lua_push_meta_engine_vector3_d(state, vehicle->hover_position);
        }
        else if(field == "unknownVehicle3") {
            lua_push_meta_engine_vector3_d(state, vehicle->unknown_vehicle3);
        }
        else if(field == "unknownVehicle4") {
            lua_push_meta_engine_vector3_d(state, vehicle->unknown_vehicle4);
        }
        else if(field == "unknownVehicle5") {
            lua_pushinteger(state, vehicle->unknown_vehicle5);
        }
        else if(field == "network") {
            lua_push_meta_engine_vehicle_network(state, vehicle->network);
        }
        else {
            return lua_engine_unit_object__index(state);
        }
        return 1;
    }

    static int lua_engine_vehicle_object__newindex(lua_State *state) noexcept {
        auto *vehicle = lua_from_meta_object<Engine::VehicleObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "vehicleFlags") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "speed") {
            vehicle->speed = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "slide") {
            vehicle->slide = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "turn") {
            vehicle->turn = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "tirePosition") {
            vehicle->tire_position = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "threadPositionLeft") {
            vehicle->thread_position_left = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "threadPositionRight") {
            vehicle->thread_position_right = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "hover") {
            vehicle->hover = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "thrust") {
            vehicle->thrust = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "suspensionStates") {
            for(std::size_t i = 0; i < sizeof(vehicle->suspension_states) / sizeof(vehicle->suspension_states[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    vehicle->suspension_states[i] = static_cast<std::int8_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "hoverPosition") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "unknownVehicle3") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "unknownVehicle4") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "unknownVehicle5") {
            vehicle->unknown_vehicle5 = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "network") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return lua_engine_unit_object__newindex(state);
        }
        return 0;
    }

    void lua_push_meta_engine_vehicle_object(lua_State *state, Engine::VehicleObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_vehicle_object__index, lua_engine_vehicle_object__newindex, read_only);
    }

    static int lua_engine_item_object__index(lua_State *state) noexcept {
        auto *item = lua_from_meta_object<Engine::ItemObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "flags") {
            lua_pushinteger(state, item->flags);
        }
        else if(field == "ticksUntilDetonation") {
            lua_pushinteger(state, item->ticks_until_detonation);
        }
        else if(field == "bspCollisionSurfaceId") {
            lua_pushinteger(state, item->bsp_collision.surface_id);
        }
        else if(field == "bspCollisionReferenceId") {
            lua_pushinteger(state, item->bsp_collision.reference_id);
        }
        else if(field == "droppedByUnit") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&item->dropped_by_unit));
        }
        else if(field == "lastUpdateTick") {
            lua_pushinteger(state, item->last_update_tick);
        }
        else if(field == "objectCollisionObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&item->object_collision.object));
        }
        else if(field == "objectCollisionObjectPosition") {
            lua_push_meta_engine_vector3_d(state, item->object_collision.object_position);
        }
        else if(field == "unknownCollisionPosition") {
            lua_push_meta_engine_vector3_d(state, item->unknown_collision_position);
        }
        else if(field == "unknownCollisionAngle") {
            lua_push_meta_engine_euler2_d(state, item->unknown_collision_angle);
        }
        else {
            return lua_engine_unit_object__index(state);
        }
        return 1;
    }

    static int lua_engine_item_object__newindex(lua_State *state) noexcept {
        auto *item = lua_from_meta_object<Engine::ItemObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "flags") {
            item->flags = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "ticksUntilDetonation") {
            item->ticks_until_detonation = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "bspCollisionSurfaceId") {
            item->bsp_collision.surface_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "bspCollisionReferenceId") {
            item->bsp_collision.reference_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "droppedByUnit") {
            item->dropped_by_unit.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "lastUpdateTick") {
            item->last_update_tick = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "objectCollisionObject") {
            item->object_collision.object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "objectCollisionObjectPosition") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "unknownCollisionPosition") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "unknownCollisionAngle") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return lua_engine_unit_object__newindex(state);
        }
        return 0;
    }

    void lua_push_meta_engine_item_object(lua_State *state, Engine::ItemObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_item_object__index, lua_engine_item_object__newindex, read_only);
    }

    static int lua_engine_garbage_object__index(lua_State *state) noexcept {
        auto *garbage = lua_from_meta_object<Engine::GarbageObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "ticksUntilGarbageCollection") {
            lua_pushinteger(state, garbage->ticks_until_garbage_collection);
        }
        else {
            return lua_engine_item_object__index(state);
        }
        return 1;
    }

    static int lua_engine_garbage_object__newindex(lua_State *state) noexcept {
        auto *garbage = lua_from_meta_object<Engine::GarbageObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "ticksUntilGarbageCollection") {
            garbage->ticks_until_garbage_collection = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else {
            return lua_engine_item_object__newindex(state);
        }
        return 0;
    }

    void lua_push_meta_engine_garbage_object(lua_State *state, Engine::GarbageObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_garbage_object__index, lua_engine_garbage_object__newindex, read_only);
    }

    static int lua_engine_weapon_trigger__index(lua_State *state) noexcept {
        auto *trigger = lua_from_meta_object<Engine::WeaponTrigger>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "idleTime") {
            lua_pushinteger(state, trigger->idle_time);
        }
        else if(field == "state") {
            lua_pushstring(state, weapon_state_to_string(trigger->state).c_str());
        }
        else if(field == "triggerTime") {
            lua_pushinteger(state, trigger->trigger_time);
        }
        else if(field == "notFiring") {
            lua_pushinteger(state, trigger->not_firing);
        }
        else if(field == "autoReload") {
            lua_pushinteger(state, trigger->auto_reload);
        }
        else if(field == "roundsSinceLastTracer") {
            lua_pushinteger(state, trigger->rounds_since_last_tracer);
        }
        else if(field == "rateOfFire") {
            lua_pushnumber(state, trigger->rate_of_fire);
        }
        else if(field == "ejectionPortRecoveryTime") {
            lua_pushnumber(state, trigger->ejection_port_recovery_time);
        }
        else if(field == "illuminationRecoveryTime") {
            lua_pushnumber(state, trigger->illumination_recovery_time);
        }
        else if(field == "projectileErrorRelated") {
            lua_pushnumber(state, trigger->projectile_error_related);
        }
        else if(field == "charingEffect") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&trigger->charing_effect));
        }
        else if(field == "networkDelayTicks") {
            lua_pushinteger(state, trigger->network_delay_ticks);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_weapon_trigger__newindex(lua_State *state) noexcept {
        auto *trigger = lua_from_meta_object<Engine::WeaponTrigger>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "idleTime") {
            trigger->idle_time = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "state") {
            trigger->state = weapon_state_from_string(luaL_checkstring(state, 3));
        }
        else if(field == "triggerTime") {
            trigger->trigger_time = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "notFiring") {
            trigger->not_firing = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "autoReload") {
            trigger->auto_reload = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "roundsSinceLastTracer") {
            trigger->rounds_since_last_tracer = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "rateOfFire") {
            trigger->rate_of_fire = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "ejectionPortRecoveryTime") {
            trigger->ejection_port_recovery_time = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "illuminationRecoveryTime") {
            trigger->illumination_recovery_time = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "projectileErrorRelated") {
            trigger->projectile_error_related = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "charingEffect") {
            trigger->charing_effect.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "networkDelayTicks") {
            trigger->network_delay_ticks = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_weapon_trigger(lua_State *state, Engine::WeaponTrigger &trigger, bool read_only) noexcept {
        lua_push_meta_object(state, trigger, lua_engine_weapon_trigger__index, lua_engine_weapon_trigger__newindex, read_only);
    }

    static int lua_engine_weapon_magazine__index(lua_State *state) noexcept {
        auto *magazine = lua_from_meta_object<Engine::WeaponMagazine>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "state") {
            lua_pushstring(state, weapon_magazine_state_to_string(magazine->state).c_str());
        }
        else if(field == "reloadTicksRemaining") {
            lua_pushinteger(state, magazine->reload_ticks_remaining);
        }
        else if(field == "reloadTicks") {
            lua_pushinteger(state, magazine->reload_ticks);
        }
        else if(field == "roundsUnloaded") {
            lua_pushinteger(state, magazine->rounds_unloaded);
        }
        else if(field == "roundsLoaded") {
            lua_pushinteger(state, magazine->rounds_loaded);
        }
        else if(field == "roundsLeftToRecharge") {
            lua_pushinteger(state, magazine->rounds_left_to_recharge);
        }
        else if(field == "unknown") {
            lua_pushinteger(state, magazine->unknown);
        }
        else if(field == "unknown2") {
            lua_pushinteger(state, magazine->unknown2);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_weapon_magazine__newindex(lua_State *state) noexcept {
        auto *magazine = lua_from_meta_object<Engine::WeaponMagazine>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }

        std::string field = key;
        if(field == "state") {
            magazine->state = weapon_magazine_state_from_string(luaL_checkstring(state, 3));
        }
        else if(field == "reloadTicksRemaining") {
            magazine->reload_ticks_remaining = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "reloadTicks") {
            magazine->reload_ticks = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "roundsUnloaded") {
            magazine->rounds_unloaded = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "roundsLoaded") {
            magazine->rounds_loaded = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "roundsLeftToRecharge") {
            magazine->rounds_left_to_recharge = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown") {
            magazine->unknown = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "unknown2") {
            magazine->unknown2 = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_weapon_magazine(lua_State *state, Engine::WeaponMagazine &magazine, bool read_only) noexcept {
        lua_push_meta_object(state, magazine, lua_engine_weapon_magazine__index, lua_engine_weapon_magazine__newindex, read_only);
    }

    static int lua_engine_weapon_reload_start_data__index(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::WeaponReloadStartData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "totalRounds") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(data->total_rounds) / sizeof(data->total_rounds[0]); i++) {
                lua_pushinteger(state, data->total_rounds[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "loadedRounds") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(data->loaded_rounds) / sizeof(data->loaded_rounds[0]); i++) {
                lua_pushinteger(state, data->loaded_rounds[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_weapon_reload_start_data__newindex(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::WeaponReloadStartData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "totalRounds") {
            for(std::size_t i = 0; i < sizeof(data->total_rounds) / sizeof(data->total_rounds[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    data->total_rounds[i] = static_cast<std::int16_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "loadedRounds") {
            for(std::size_t i = 0; i < sizeof(data->loaded_rounds) / sizeof(data->loaded_rounds[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    data->loaded_rounds[i] = static_cast<std::int16_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_weapon_reload_start_data(lua_State *state, Engine::WeaponReloadStartData &data, bool read_only) noexcept {
        lua_push_meta_object(state, data, lua_engine_weapon_reload_start_data__index, lua_engine_weapon_reload_start_data__newindex, read_only);
    }

    static int lua_engine_weapon_network_data__index(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::WeaponNetworkData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "position") {
            lua_push_meta_engine_vector3_d(state, data->position);
        } 
        else if(field == "transitionalVelocity") {
            lua_push_meta_engine_vector3_d(state, data->transitional_velocity);
        }
        else if(field == "angularVelocity") {
            lua_push_meta_engine_vector3_d(state, data->angular_velocity);
        }
        else if(field == "magazineRoundsTotal") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(data->magazine_rounds_total) / sizeof(data->magazine_rounds_total[0]); i++) {
                lua_pushinteger(state, data->magazine_rounds_total[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "age") {
            lua_pushnumber(state, data->age);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_weapon_network_data__newindex(lua_State *state) noexcept {
        auto *data = lua_from_meta_object<Engine::WeaponNetworkData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 

        std::string field = key;
        if(field == "position") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "transitionalVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "angularVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "magazineRoundsTotal") {
            for(std::size_t i = 0; i < sizeof(data->magazine_rounds_total) / sizeof(data->magazine_rounds_total[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    data->magazine_rounds_total[i] = static_cast<std::int16_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "age") {
            data->age = static_cast<float>(luaL_checknumber(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_weapon_network_data(lua_State *state, Engine::WeaponNetworkData &data, bool read_only) noexcept {
        lua_push_meta_object(state, data, lua_engine_weapon_network_data__index, lua_engine_weapon_network_data__newindex, read_only);
    }

    static int lua_engine_weapon_network__index(lua_State *state) noexcept {
        auto *network = lua_from_meta_object<Engine::WeaponNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "baselineValid") {
            lua_pushboolean(state, network->baseline_valid);
        }
        else if(field == "baselineIndex") {
            lua_pushinteger(state, network->baseline_index);
        }
        else if(field == "messageIndex") {
            lua_pushinteger(state, network->message_index);
        }
        else if(field == "updateBaseline") {
            lua_push_meta_engine_weapon_network_data(state, network->update_baseline);
        }
        else if(field == "deltaValid") {
            lua_pushboolean(state, network->delta_valid);
        }
        else if(field == "updateDelta") {
            lua_push_meta_engine_weapon_network_data(state, network->update_delta);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_weapon_network__newindex(lua_State *state) noexcept {
        auto *network = lua_from_meta_object<Engine::WeaponNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 

        std::string field = key;
        if(field == "baselineValid") {
            if(lua_isboolean(state, 3)) {
                network->baseline_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "baselineIndex") {
            network->baseline_index = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "messageIndex") {
            network->message_index = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "updateBaseline") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "deltaValid") {
            if(lua_isboolean(state, 3)) {
                network->delta_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "updateDelta") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_weapon_network(lua_State *state, Engine::WeaponNetwork &network, bool read_only) noexcept {
        lua_push_meta_object(state, network, lua_engine_weapon_network__index, lua_engine_weapon_network__newindex, read_only);
    }

    static int lua_engine_weapon_object__index(lua_State *state) noexcept {
        auto *weapon = lua_from_meta_object<Engine::WeaponObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "flags") {
            lua_pushinteger(state, weapon->flags);
        }
        else if(field == "ownerUnitFlags") {
            lua_pushinteger(state, weapon->owner_unit_flags);
        }
        else if(field == "primaryTrigger") {
            lua_pushnumber(state, weapon->primary_trigger);
        }
        else if(field == "weaponState") {
            lua_pushstring(state, weapon_state_to_string(weapon->weapon_state).c_str());
        }
        else if(field == "readyTicks") {
            lua_pushinteger(state, weapon->ready_ticks);
        }
        else if(field == "heat") {
            lua_pushnumber(state, weapon->heat);
        }
        else if(field == "age") {
            lua_pushnumber(state, weapon->age);
        }
        else if(field == "illuminationFraction") {
            lua_pushnumber(state, weapon->illumination_fraction);
        }
        else if(field == "integratedLightPower") {
            lua_pushnumber(state, weapon->integrated_light_power);
        }
        else if(field == "trackedObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&weapon->tracked_object));
        }
        else if(field == "altShotsLoaded") {
            lua_pushinteger(state, weapon->alt_shots_loaded);
        }
        else if(field == "triggers") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(weapon->triggers) / sizeof(weapon->triggers[0]); i++) {
                lua_push_meta_engine_weapon_trigger(state, weapon->triggers[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "magazines") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(weapon->magazines) / sizeof(weapon->magazines[0]); i++) {
                lua_push_meta_engine_weapon_magazine(state, weapon->magazines[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "lastTriggerFireTick") {
            lua_pushinteger(state, weapon->last_trigger_fire_tick);
        }
        else if(field == "reloadStartingPoint") {
            lua_push_meta_engine_weapon_reload_start_data(state, weapon->reload_starting_point);
        }
        else if(field == "network") {
            lua_push_meta_engine_weapon_network(state, weapon->network);
        }
        else {
            return lua_engine_item_object__index(state);
        }
        return 1;
    }

    static int lua_engine_weapon_object__newindex(lua_State *state) noexcept {
        auto *weapon = lua_from_meta_object<Engine::WeaponObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "flags") {
            weapon->flags = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "ownerUnitFlags") {
            weapon->owner_unit_flags = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "primaryTrigger") {
            weapon->primary_trigger = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "weaponState") {
            weapon->weapon_state = weapon_state_from_string(luaL_checkstring(state, 3));
        }
        else if(field == "readyTicks") {
            weapon->ready_ticks = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "heat") {
            weapon->heat = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "age") {
            weapon->age = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "illuminationFraction") {
            weapon->illumination_fraction = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "integratedLightPower") {
            weapon->integrated_light_power = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "trackedObject") {
            weapon->tracked_object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "altShotsLoaded") {
            weapon->alt_shots_loaded = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "triggers") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "magazines") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "lastTriggerFireTick") {
            weapon->last_trigger_fire_tick = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "reloadStartingPoint") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "network") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return lua_engine_item_object__newindex(state);
        }
        return 0;
    }

    void lua_push_meta_engine_weapon_object(lua_State *state, Engine::WeaponObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_weapon_object__index, lua_engine_weapon_object__newindex, read_only);
    }

    static int lua_engine_equipment_network_data__index(lua_State *state) noexcept {
        auto *equipment_network_data = lua_from_meta_object<Engine::EquipmentNetworkData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "position") {
            lua_push_meta_engine_vector3_d(state, equipment_network_data->position);
        }
        else if(field == "transitionalVelocity") {
            lua_push_meta_engine_vector3_d(state, equipment_network_data->transitional_velocity);
        }
        else if(field == "angularVelocity") {
            lua_push_meta_engine_vector3_d(state, equipment_network_data->angular_velocity);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

     static int lua_engine_equipment_network_data__newindex(lua_State *state) noexcept {
        auto *equipment_network_data = lua_from_meta_object<Engine::EquipmentNetworkData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "position") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "transitionalVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "angularVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_equipment_network_data(lua_State *state, Engine::EquipmentNetworkData &data, bool read_only) noexcept {
        lua_push_meta_object(state, data, lua_engine_equipment_network_data__index, lua_engine_equipment_network_data__newindex, read_only);
    }

    static int lua_engine_equipment_network__index(lua_State *state) noexcept {
        auto *equipment_network = lua_from_meta_object<Engine::EquipmentNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "baselineValid") {
            lua_pushboolean(state, equipment_network->baseline_valid);
        }
        else if(field == "baselineIndex") {
            lua_pushinteger(state, equipment_network->baseline_index);
        }
        else if(field == "messageIndex") {
            lua_pushinteger(state, equipment_network->message_index);
        }
        else if(field == "updateBaseline") {
            lua_push_meta_engine_equipment_network_data(state, equipment_network->update_baseline);
        }
        else if(field == "deltaValid") {
            lua_pushboolean(state, equipment_network->delta_valid);
        }
        else if(field == "updateDelta") {
            lua_push_meta_engine_equipment_network_data(state, equipment_network->update_delta);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_equipment_network__newindex(lua_State *state) noexcept {
        auto *equipment_network = lua_from_meta_object<Engine::EquipmentNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        } 
        
        std::string field = key;
        if(field == "baselineValid") {
            if(lua_isboolean(state, 3)) {
                equipment_network->baseline_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "baselineIndex") {
            equipment_network->baseline_index = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "messageIndex") {
            equipment_network->message_index = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "updateBaseline") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "deltaValid") {
            if(lua_isboolean(state, 3)) {
                equipment_network->delta_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "updateDelta") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_equipment_network(lua_State *state, Engine::EquipmentNetwork &network, bool read_only) noexcept {
        lua_push_meta_object(state, network, lua_engine_equipment_network__index, lua_engine_equipment_network__newindex, read_only);
    }

    static int lua_engine_equipment_object__index(lua_State *state) noexcept {
        auto *equipment = lua_from_meta_object<Engine::EquipmentObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return lua_engine_item_object__index(state);
        } 
        
        std::string field = key;
        if(field == "network") {
            lua_push_meta_engine_equipment_network(state, equipment->network);
        }
        else {
            return lua_engine_item_object__index(state);
        }
        return 1;
    }

    static int lua_engine_equipment_object__newindex(lua_State *state) noexcept {
        auto *equipment = lua_from_meta_object<Engine::EquipmentObject>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return lua_engine_item_object__newindex(state);
        } 
        
        std::string field = key;
        if(field == "network") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return lua_engine_item_object__newindex(state);
        }
        return 0;
    }

    void lua_push_meta_engine_equipment_object(lua_State *state, Engine::EquipmentObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_equipment_object__index, lua_engine_equipment_object__newindex, read_only);
    }

    static int lua_engine_projectile_object_flags__index(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::ProjectileObjectFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "tracer") {
            lua_pushboolean(state, flags->tracer);
        }
        else if(field == "projectileUnknownBit") {
            lua_pushboolean(state, flags->projectile_unknown_bit);
        }
        else if(field == "attached") {
            lua_pushboolean(state, flags->attached);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_projectile_object_flags__newindex(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::ProjectileObjectFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "tracer") {
            flags->tracer = lua_toboolean(state, 3);
        }
        else if(field == "projectileUnknownBit") {
            flags->projectile_unknown_bit = lua_toboolean(state, 3);
        }
        else if(field == "attached") {
            flags->attached = lua_toboolean(state, 3);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_projectile_object_flags(lua_State *state, Engine::ProjectileObjectFlags &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_projectile_object_flags__index, lua_engine_projectile_object_flags__newindex, read_only);
    }

    static int lua_engine_projectile_network_data__index(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::ProjectileNetworkData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "position") {
            lua_push_meta_engine_vector3_d(state, flags->position);
        }
        else if(field == "transitionalVelocity") {
            lua_push_meta_engine_vector3_d(state, flags->transitional_velocity);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_projectile_network_data__newindex(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::ProjectileNetworkData>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "position") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "transitionalVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_projectile_network_data(lua_State *state, Engine::ProjectileNetworkData &data, bool read_only) noexcept {
        lua_push_meta_object(state, data, lua_engine_projectile_network_data__index, lua_engine_projectile_network_data__newindex, read_only);
    }

    static int lua_engine_projectile_network__index(lua_State *state) noexcept {
        auto *network = lua_from_meta_object<Engine::ProjectileNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "unknown") {
            lua_pushboolean(state, network->unknown);
        }
        else if(field == "baselineValid") {
            lua_pushboolean(state, network->baseline_valid);
        }
        else if(field == "baselineIndex") {
            lua_pushinteger(state, network->baseline_index);
        }
        else if(field == "messageIndex") {
            lua_pushinteger(state, network->message_index);
        }
        else if(field == "updateBaseline") {
            lua_push_meta_engine_projectile_network_data(state, network->update_baseline);
        }
        else if(field == "deltaValid") {
            lua_pushboolean(state, network->delta_valid);
        }
        else if(field == "updateDelta") {
            lua_push_meta_engine_projectile_network_data(state, network->update_delta);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_projectile_network__newindex(lua_State *state) noexcept {
        auto *network = lua_from_meta_object<Engine::ProjectileNetwork>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "unknown") {
            if(lua_isboolean(state, 3)) {
                network->unknown = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "baselineValid") {
            if(lua_isboolean(state, 3)) {
                network->baseline_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "baselineIndex") {
            network->baseline_index = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "messageIndex") {
            network->message_index = static_cast<std::int8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "updateBaseline") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "deltaValid") {
            if(lua_isboolean(state, 3)) {
                network->delta_valid = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "updateDelta") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_projectile_network(lua_State *state, Engine::ProjectileNetwork &network, bool read_only) noexcept {
        lua_push_meta_object(state, network, lua_engine_projectile_network__index, lua_engine_projectile_network__newindex, read_only);
    }

    static int lua_engine_projectile_object__index(lua_State *state) noexcept {
        auto *projectile = lua_from_meta_object<Engine::ProjectileObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr) {  
            return lua_engine_item_object__index(state);
        }
        
        std::string field = key;
        if(field == "projectileFlags") {
            lua_push_meta_engine_projectile_object_flags(state, projectile->projectile_flags);
        }
        else if(field == "actionEnum") {
            lua_pushinteger(state, projectile->action_enum);
        }
        else if(field == "materialId") {
            lua_pushinteger(state, projectile->material_id);
        }
        else if(field == "sourceUnit") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&projectile->source_unit));
        }
        else if(field == "targetObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&projectile->target_object));
        }
        else if(field == "contrailAttachmentBlockId") {
            lua_pushinteger(state, projectile->contrail_attachment_block_id);
        }
        else if(field == "timeRemaining") {
            lua_pushnumber(state, projectile->time_remaining);
        }
        else if(field == "armingRate") {
            lua_pushnumber(state, projectile->arming_rate);
        }
        else if(field == "unknownProjFloat1") {
            lua_pushnumber(state, projectile->unknown_proj_float1);
        }
        else if(field == "unknownProjFloat2") {
            lua_pushnumber(state, projectile->unknown_proj_float2);
        }
        else if(field == "distanceTravelled") {
            lua_pushnumber(state, projectile->distance_travelled);
        }
        else if(field == "transitionalVelocity") {
            lua_push_meta_engine_vector3_d(state, projectile->transitional_velocity);
        }
        else if(field == "waterDamageUpperBound") {
            lua_pushnumber(state, projectile->water_damage_upper_bound);
        }
        else if(field == "angularVelocity") {
            lua_push_meta_engine_vector3_d(state, projectile->angular_velocity);
        }
        else if(field == "unknownEuler") {
            lua_push_meta_engine_euler2_d(state, projectile->unknown_euler);
        }
        else if(field == "network") {
            lua_push_meta_engine_projectile_network(state, projectile->network);
        }
        else {
            return lua_engine_item_object__index(state);
        }
        return 1;
    }

    static int lua_engine_projectile_object__newindex(lua_State *state) noexcept {
        auto *projectile = lua_from_meta_object<Engine::ProjectileObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr || !lua_istable(state, 3)) {  
            return lua_engine_item_object__newindex(state);
        }
        
        std::string field = key;
        if(field == "projectileFlags") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "actionEnum") {
            projectile->action_enum = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "materialId") {
            projectile->material_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "sourceUnit") {
            projectile->source_unit.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "targetObject") {
            projectile->target_object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "contrailAttachmentBlockId") {
            projectile->contrail_attachment_block_id = static_cast<std::int32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "timeRemaining") {
            projectile->time_remaining = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "armingRate") {
            projectile->arming_rate = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "unknownProjFloat1") {
            projectile->unknown_proj_float1 = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "unknownProjFloat2") {
            projectile->unknown_proj_float2 = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "distanceTravelled") {
            projectile->distance_travelled = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "transitionalVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "waterDamageUpperBound") {
            projectile->water_damage_upper_bound = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "angularVelocity") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "unknownEuler") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "network") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return lua_engine_item_object__newindex(state);
        }
        return 0;
    }

    void lua_push_meta_engine_projectile_object(lua_State *state, Engine::ProjectileObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_projectile_object__index, lua_engine_projectile_object__newindex, read_only);
    }

    static int lua_engine_device_object_state__index(lua_State *state) noexcept {
        auto *state_data = lua_from_meta_object<Engine::DeviceObjectState>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "deviceGroupId") {
            lua_pushinteger(state, state_data->device_group_id);
        }
        else if(field == "value") {
            lua_pushnumber(state, state_data->value);
        }
        else if(field == "change") {
            lua_pushnumber(state, state_data->change);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_device_object_state__newindex(lua_State *state) noexcept {
        auto *state_data = lua_from_meta_object<Engine::DeviceObjectState>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isnumber(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "deviceGroupId") {
            state_data->device_group_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "value") {
            state_data->value = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "change") {
            state_data->change = static_cast<float>(luaL_checknumber(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_device_object_state(lua_State *state, Engine::DeviceObjectState &state_data, bool read_only) noexcept {
        lua_push_meta_object(state, state_data, lua_engine_device_object_state__index, lua_engine_device_object_state__newindex, read_only);
    }

    static int lua_engine_device_object__index(lua_State *state) noexcept {
        auto *device = lua_from_meta_object<Engine::DeviceObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr) {  
            return lua_engine_item_object__index(state);
        }
        
        std::string field = key;
        if(field == "positionReversed") {
            lua_pushboolean(state, device->position_reversed);
        }
        else if(field == "notUsableFromAnySide") {
            lua_pushboolean(state, device->not_usable_from_any_side);
        }
        else if(field == "power") {
            lua_push_meta_engine_device_object_state(state, device->power);
        }
        else if(field == "position") {
            lua_push_meta_engine_device_object_state(state, device->position);
        }
        else if(field == "oneSided") {
            lua_pushboolean(state, device->one_sided);
        }
        else if(field == "operatesAutomatically") {
            lua_pushboolean(state, device->operates_automatically);
        }
        else {
            return lua_engine_item_object__index(state);
        }
        return 1;        
    }

    static int lua_engine_device_object__newindex(lua_State *state) noexcept {
        auto *device = lua_from_meta_object<Engine::DeviceObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr || !lua_istable(state, 3)) {  
            return lua_engine_item_object__newindex(state);
        }
        
        std::string field = key;
        if(field == "positionReversed") {
            if(lua_isboolean(state, 3)) {
                device->position_reversed = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "notUsableFromAnySide") {
            if(lua_isboolean(state, 3)) {
                device->not_usable_from_any_side = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "power") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "position") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "oneSided") {
            if(lua_isboolean(state, 3)) {
                device->one_sided = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "operatesAutomatically") {
            if(lua_isboolean(state, 3)) {
                device->operates_automatically = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else {
            return lua_engine_item_object__newindex(state);
        }
        return 0;        
    }

    void lua_push_meta_engine_device_object(lua_State *state, Engine::DeviceObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_device_object__index, lua_engine_device_object__newindex, read_only);
    }

    static int lua_engine_device_machine_object_flags__index(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::DeviceMachineObjectFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "doesNotOperateAutomatically") {
            lua_pushboolean(state, flags->does_not_operate_automatically);
        }
        else if(field == "machineOneSided") {
            lua_pushboolean(state, flags->machine_one_sided);
        }
        else if(field == "neverAppearsLocked") {
            lua_pushboolean(state, flags->never_appears_locked);
        }
        else if(field == "openedByMeleeAttack") {
            lua_pushboolean(state, flags->opened_by_melee_attack);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_device_machine_object_flags__newindex(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::DeviceMachineObjectFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "doesNotOperateAutomatically") {
            flags->does_not_operate_automatically = lua_toboolean(state, 3);
        }
        else if(field == "machineOneSided") {
            flags->machine_one_sided = lua_toboolean(state, 3);
        }
        else if(field == "neverAppearsLocked") {
            flags->never_appears_locked = lua_toboolean(state, 3);
        }
        else if(field == "openedByMeleeAttack") {
            flags->opened_by_melee_attack = lua_toboolean(state, 3);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_device_machine_object_flags(lua_State *state, Engine::DeviceMachineObjectFlags &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_device_machine_object_flags__index, lua_engine_device_machine_object_flags__newindex, read_only);
    }

    static int lua_engine_device_machine_object__index(lua_State *state) noexcept {
        auto *device = lua_from_meta_object<Engine::DeviceMachineObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr) {  
            return lua_engine_device_object__index(state);
        }
        
        std::string field = key;
        if(field == "deviceFlags") {
            lua_push_meta_engine_device_machine_object_flags(state, device->device_flags);
        }
        else if(field == "ticksSinceStartedOpening") {
            lua_pushinteger(state, device->ticks_since_started_opening);
        }
        else if(field == "elevatorPosition") {
            lua_push_meta_engine_vector3_d(state, device->elevator_position);
        }
        else {
            return lua_engine_device_object__index(state);
        }
        return 1;
    }

    static int lua_engine_device_machine_object__newindex(lua_State *state) noexcept {
        auto *device = lua_from_meta_object<Engine::DeviceMachineObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr || !lua_istable(state, 3)) {  
            return lua_engine_device_object__newindex(state);
        }
        
        std::string field = key;
        if(field == "deviceFlags") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "ticksSinceStartedOpening") {
            device->ticks_since_started_opening = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "elevatorPosition") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return lua_engine_device_object__newindex(state);
        }
        return 0;
    }

    void lua_push_meta_engine_device_machine_object(lua_State *state, Engine::DeviceMachineObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_device_machine_object__index, lua_engine_device_machine_object__newindex, read_only);
    }

    static int lua_engine_device_control_object_flags__index(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::DeviceControlObjectFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "usableFromBothSides") {
            lua_pushboolean(state, flags->usable_from_both_sides);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }
    
    static int lua_engine_device_control_object_flags__newindex(lua_State *state) noexcept {
        auto *flags = lua_from_meta_object<Engine::DeviceControlObjectFlags>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_isboolean(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "usableFromBothSides") {
            flags->usable_from_both_sides = lua_toboolean(state, 3);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_device_control_object_flags(lua_State *state, Engine::DeviceControlObjectFlags &flags, bool read_only) noexcept {
        lua_push_meta_object(state, flags, lua_engine_device_control_object_flags__index, lua_engine_device_control_object_flags__newindex, read_only);
    }

    static int lua_engine_device_control_object__index(lua_State *state) noexcept {
        auto *device = lua_from_meta_object<Engine::DeviceControlObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr) {  
            return lua_engine_device_object__index(state);
        }
        
        std::string field = key;
        if(field == "deviceControlFlags") {
            lua_push_meta_engine_device_control_object_flags(state, device->device_control_flags);
        }
        else if(field == "customNameId") {
            lua_pushinteger(state, device->custom_name_id);
        }
        else {
            return lua_engine_device_object__index(state);
        }
        return 1;
    }

    static int lua_engine_device_control_object__newindex(lua_State *state) noexcept {
        auto *device = lua_from_meta_object<Engine::DeviceControlObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr || !lua_istable(state, 3)) {  
            return lua_engine_device_object__newindex(state);
        }
        
        std::string field = key;
        if(field == "deviceControlFlags") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "customNameId") {
            device->custom_name_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else {
            return lua_engine_device_object__newindex(state);
        }
        return 0;
    }

    void lua_push_meta_engine_device_control_object(lua_State *state, Engine::DeviceControlObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_device_control_object__index, lua_engine_device_control_object__newindex, read_only);
    }

    static int lua_engine_device_light_fixture_object__index(lua_State *state) noexcept {
        auto *device = lua_from_meta_object<Engine::DeviceLightFixtureObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr) {  
            return lua_engine_device_object__index(state);
        }
        
        std::string field = key;
        if(field == "lightColor") {
            lua_push_meta_engine_color_r_g_b(state, device->light_color);
        }
        else if(field == "lightIntensity") {
            lua_pushnumber(state, device->light_intensity);
        }
        else if(field == "lightFalloffAngle") {
            lua_pushnumber(state, device->light_falloff_angle);
        }
        else if(field == "lightCutoffAngle") {
            lua_pushnumber(state, device->light_cutoff_angle);
        }
        else {
            return lua_engine_device_object__index(state);
        }
        return 1;
    }

    static int lua_engine_device_light_fixture_object__newindex(lua_State *state) noexcept {
        auto *device = lua_from_meta_object<Engine::DeviceLightFixtureObject>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr || !lua_istable(state, 3)) {  
            return lua_engine_device_object__newindex(state);
        }
        
        std::string field = key;
        if(field == "light_color") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "lightIntensity") {
            device->light_intensity = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "lightFalloffAngle") {
            device->light_falloff_angle = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "lightCutoffAngle") {
            device->light_cutoff_angle = static_cast<float>(luaL_checknumber(state, 3));
        }
        else {
            return lua_engine_device_object__newindex(state);
        }
        return 0;
    }

    void lua_push_meta_engine_device_light_fixture_object(lua_State *state, Engine::DeviceLightFixtureObject &object, bool read_only) noexcept {
        lua_push_meta_object(state, object, lua_engine_device_light_fixture_object__index, lua_engine_device_light_fixture_object__newindex, read_only);
    }

    static int lua_engine_player_multiplayer_statistics__index(lua_State *state) {
        auto *stats = lua_from_meta_object<Engine::PlayerMultiplayerStatistics>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr) {  
            return lua_engine_device_object__index(state);
        }
        
        std::string field = key;
        if(field == "ctfFlagGrabs") {
            lua_pushinteger(state, stats->ctf.flag_grabs);
        }
        else if(field == "ctfFlagReturns") {
            lua_pushinteger(state, stats->ctf.flag_returns);
        }
        else if(field == "ctfFlagScores") {
            lua_pushinteger(state, stats->ctf.flag_scores);
        }
        else if(field == "oddballTargetKills") {
            lua_pushinteger(state, stats->oddball.target_kills);
        }
        else if(field == "oddballKills") {
            lua_pushinteger(state, stats->oddball.kills);
        }
        else if(field == "oddballUnknown") {
            lua_pushinteger(state, stats->oddball.unknown);
        }
        else if(field == "kingHillScore") {
            lua_pushinteger(state, stats->king.hill_score);
        }
        else if(field == "raceTime") {
            lua_pushinteger(state, stats->race.time);
        }
        else if(field == "raceLaps") {
            lua_pushinteger(state, stats->race.laps);
        }
        else if(field == "raceBestTime") {
            lua_pushinteger(state, stats->race.best_time);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_player_multiplayer_statistics__newindex(lua_State *state) {
        auto *stats = lua_from_meta_object<Engine::PlayerMultiplayerStatistics>(state, 1); 
        auto *key = lua_tostring(state, 2);
        
        if(key == nullptr) {  
            return lua_engine_device_object__index(state);
        }
        
        std::string field = key;
        if(field == "ctfFlagGrabs") {
            stats->ctf.flag_grabs = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "ctfFlagReturns") {
            stats->ctf.flag_returns = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "ctfFlagScores") {
            stats->ctf.flag_scores = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "oddballTargetKills") {
            stats->oddball.target_kills = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "oddballKills") {
            stats->oddball.kills = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "oddballUnknown") {
            stats->oddball.unknown = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "kingHillScore") {
            stats->king.hill_score = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "raceTime") {
            stats->race.time = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "raceLaps") {
            stats->race.laps = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "raceBestTime") {
            stats->race.best_time = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_player_multiplayer_statistics(lua_State *state, Engine::PlayerMultiplayerStatistics &stats, bool read_only) noexcept {
        lua_push_meta_object(state, stats, lua_engine_player_multiplayer_statistics__index, lua_engine_player_multiplayer_statistics__newindex, read_only);
    }

    static int lua_engine_player__index(lua_State *state) noexcept {
        auto *player = lua_from_meta_object<Engine::Player>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "playerId") {
            lua_pushinteger(state, player->player_id);
        }
        else if(field == "localHandle") {
            lua_pushinteger(state, player->local_handle);
        }
        else if(field == "name") {
            lua_pushstring(state, reinterpret_cast<const char *>(player->name));
        }
        else if(field == "unknownHandle") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&player->unknown_handle));
        }
        else if(field == "team") {
            lua_pushinteger(state, player->team);
        }
        else if(field == "interactionObjectHandle") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&player->interaction_object_handle));
        }
        else if(field == "interactionObjectType") {
            lua_pushinteger(state, player->interaction_object_type);
        }
        else if(field == "interactionObjectSeat") {
            lua_pushinteger(state, player->interaction_object_seat);
        }
        else if(field == "respawnTime") {
            lua_pushinteger(state, player->respawn_time);
        }
        else if(field == "respawnTimeGrowth") {
            lua_pushinteger(state, player->respawn_time_growth);
        }
        else if(field == "objectHandle") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&player->object_handle));
        }
        else if(field == "prevObjectHandle") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&player->prev_object_handle));
        }
        else if(field == "bspClusterId") {
            lua_pushinteger(state, player->bsp_cluster_id);
        }
        else if(field == "weaponSwapResult") {
            lua_pushboolean(state, player->weapon_swap_result);
        }
        else if(field == "autoAimTargetObject") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&player->auto_aim_target_object));
        }
        else if(field == "lastFireTime") {
            lua_pushinteger(state, player->last_fire_time);
        }
        else if(field == "name2") {
            lua_pushstring(state, reinterpret_cast<const char *>(player->name2));
        }
        else if(field == "color") {
            lua_pushstring(state, network_color_to_string(player->color).c_str());
        }
        else if(field == "iconIndex") {
            lua_pushinteger(state, player->icon_index);
        }
        else if(field == "machineIndex") {
            lua_pushinteger(state, player->machine_index);
        }
        else if(field == "controllerIndex") {
            lua_pushinteger(state, player->controller_index);
        }
        else if(field == "team2") {
            lua_pushinteger(state, player->team2);
        }
        else if(field == "index") {
            lua_pushinteger(state, player->index);
        }
        else if(field == "invisibilityTime") {
            lua_pushinteger(state, player->invisibility_time);
        }
        else if(field == "otherPowerupTimeLeft") {
            lua_pushinteger(state, player->other_powerup_time_left);
        }
        else if(field == "speed") {
            lua_pushnumber(state, player->speed);
        }
        else if(field == "teleporterFlagHandle") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&player->teleporter_flag_handle));
        }
        else if(field == "objectiveMode") {
            lua_pushstring(state, player_objective_mode_to_string(player->objective_mode).c_str());
        }
        else if(field == "objectivePlayerHandle") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&player->objective_player_handle));
        }
        else if(field == "targetPlayer") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&player->target_player));
        }
        else if(field == "targetTime") {
            lua_pushinteger(state, player->target_time);
        }
        else if(field == "lastDeathTime") {
            lua_pushinteger(state, player->last_death_time);
        }
        else if(field == "slayerTarget") {
            lua_push_engine_resource_handle(state, reinterpret_cast<Engine::ResourceHandle *>(&player->slayer_target));
        }
        else if(field == "oddManOut") {
            lua_pushboolean(state, player->odd_man_out);
        }
        else if(field == "killStreak") {
            lua_pushinteger(state, player->kill_streak);
        }
        else if(field == "multikill") {
            lua_pushinteger(state, player->multikill);
        }
        else if(field == "lastKillTime") {
            lua_pushinteger(state, player->last_kill_time);
        }
        else if(field == "kills") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(player->kills) / sizeof(player->kills[0]); i++) {
                lua_pushinteger(state, player->kills[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "assists") {
            lua_newtable(state);
            for(std::size_t i = 0; i < sizeof(player->assists) / sizeof(player->assists[0]); i++) {
                lua_pushinteger(state, player->assists[i]);
                lua_rawseti(state, -2, i + 1);
            }
        }
        else if(field == "betrays") {
            lua_pushinteger(state, player->betrays);
        }
        else if(field == "deaths") {
            lua_pushinteger(state, player->deaths);
        }
        else if(field == "suicides") {
            lua_pushinteger(state, player->suicides);
        }
        else if(field == "teamKills") {
            lua_pushinteger(state, player->team_kills);
        }
        else if(field == "multiplayerStatistics") {
            lua_push_meta_engine_player_multiplayer_statistics(state, player->multiplayer_statistics);
        }
        else if(field == "telefragTimer") {
            lua_pushinteger(state, player->telefrag_timer);
        }
        else if(field == "quitTime") {
            lua_pushinteger(state, player->quit_time);
        }
        else if(field == "telefragDanger") {
            lua_pushboolean(state, player->telefrag_danger);
        }
        else if(field == "quit") {
            lua_pushboolean(state, player->quit);
        }
        else if(field == "ping") {
            lua_pushinteger(state, player->ping);
        }
        else if(field == "teamKillCount") {
            lua_pushinteger(state, player->team_kill_count);
        }
        else if(field == "teamKillTicksSinceLast") {
            lua_pushinteger(state, player->team_kill_ticks_since_last);
        }
        else if(field == "position") {
            lua_push_meta_engine_point3_d(state, player->position);
        }
        else if(field == "melee") {
            lua_pushboolean(state, player->melee == 1);
        }
        else if(field == "action") {
            lua_pushboolean(state, player->action == 1);
        }
        else if(field == "flashlight") {
            lua_pushboolean(state, player->flashlight == 1);
        }
        else if(field == "reload") {
            lua_pushboolean(state, player->reload == 1);
        }
        else if(field == "baselineUpdateXYAim") {
            lua_pushnumber(state, player->baseline_update_xy_aim);
        }
        else if(field == "baselineUpdateZAim") {
            lua_pushnumber(state, player->baseline_update_z_aim);
        }
        else if(field == "baselineUpdateForward") {
            lua_pushnumber(state, player->baseline_update_forward);
        }
        else if(field == "baselineUpdateLeft") {
            lua_pushnumber(state, player->baseline_update_left);
        }
        else if(field == "baselineUpdateRateOfFire") {
            lua_pushnumber(state, player->baseline_update_rate_of_fire);
        }
        else if(field == "baselineUpdateWeaponSlot") {
            lua_pushinteger(state, player->baseline_update_weapon_slot);
        }
        else if(field == "baselineUpdateGrenadeSlot") {
            lua_pushinteger(state, player->baseline_update_grenade_slot);
        }
        else if(field == "updateAiming") {
            lua_push_meta_engine_point3_d(state, player->update_aiming);
        }
        else if(field == "updatePosition") {
            lua_push_meta_engine_point3_d(state, player->update_position);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_engine_player__newindex(lua_State *state) noexcept {
        auto *player = lua_from_meta_object<Engine::Player>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr || !lua_istable(state, 3)) {  
            return luaL_error(state, "Invalid key type"); 
        }
        
        std::string field = key;
        if(field == "playerId") {
            player->player_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "localHandle") {
            player->local_handle = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "name") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "unknownHandle") {
            player->unknown_handle.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "team") {
            player->team = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "interactionObjectHandle") {
            player->interaction_object_handle.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "interactionObjectType") {
            player->interaction_object_type = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "interactionObjectSeat") {
            player->interaction_object_seat = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "respawnTime") {
            player->respawn_time = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "respawnTimeGrowth") {
            player->respawn_time_growth = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "objectHandle") {
            player->object_handle.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "prevObjectHandle") {
            player->prev_object_handle.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "bspClusterId") {
            player->bsp_cluster_id = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "weaponSwapResult") {
            if(lua_isboolean(state, 3)) {
                player->weapon_swap_result = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "autoAimTargetObject") {
            player->auto_aim_target_object.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "lastFireTime") {
            player->last_fire_time = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "name2") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "color") {
            try {
                player->color = network_color_from_string(luaL_checkstring(state, 3));
            }
            catch(const std::exception &exception) {
                return luaL_error(state, exception.what());
            }
        }
        else if(field == "iconIndex") {
            player->icon_index = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "machineIndex") {
            player->machine_index = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "controllerIndex") {
            player->controller_index = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "team2") {
            player->team2 = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "index") {
            player->index = static_cast<std::uint8_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "invisibilityTime") {
            player->invisibility_time = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "otherPowerupTimeLeft") {
            player->other_powerup_time_left = static_cast<std::int16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "speed") {
            player->speed = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "teleporterFlagHandle") {
            player->teleporter_flag_handle.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "objectiveMode") {
            try {
                player->objective_mode = player_objective_mode_from_string(luaL_checkstring(state, 3));
            }
            catch(const std::exception &exception) {
                return luaL_error(state, exception.what());
            }
        }
        else if(field == "objectivePlayerHandle") {
            player->objective_player_handle.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "targetPlayer") {
            player->target_player.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "targetTime") {
            player->target_time = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "lastDeathTime") {
            player->last_death_time = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "slayerTarget") {
            player->slayer_target.handle = luaL_checkinteger(state, 3);
        }
        else if(field == "oddManOut") {
            if(lua_isboolean(state, 3)) {
                player->odd_man_out = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "killStreak") {
            player->kill_streak = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "multikill") {
            player->multikill = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "lastKillTime") {
            player->last_kill_time = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "kills") {
            for(std::size_t i = 0; i < sizeof(player->kills) / sizeof(player->kills[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    player->kills[i] = static_cast<std::int16_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "assists") {
            for(std::size_t i = 0; i < sizeof(player->assists) / sizeof(player->assists[0]); i++) {
                lua_rawgeti(state, 3, i + 1);
                if(!lua_isnil(state, -1)) {
                    player->assists[i] = static_cast<std::int16_t>(luaL_checkinteger(state, -1));
                }
                lua_pop(state, 1);
            }
        }
        else if(field == "betrays") {
            player->betrays = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "deaths") {
            player->deaths = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "suicides") {
            player->suicides = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "teamKills") {
            player->team_kills = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "multiplayerStatistics") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "telefragTimer") {
            player->telefrag_timer = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "quitTime") {
            player->quit_time = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "telefragDanger") {
            if(lua_isboolean(state, 3)) {
                player->telefrag_danger = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "quit") {
            if(lua_isboolean(state, 3)) {
                player->quit = lua_toboolean(state, 3);
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "ping") {
            player->ping = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "teamKillCount") {
            player->team_kill_count = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "teamKillTicksSinceLast") {
            player->team_kill_ticks_since_last = static_cast<std::uint32_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "position") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "melee") {
            if(lua_isboolean(state, 3)) {
                player->melee = lua_toboolean(state, 3) ? 1 : 0;
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "action") {
            if(lua_isboolean(state, 3)) {
                player->action = lua_toboolean(state, 3) ? 1 : 0;
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "flashlight") {
            if(lua_isboolean(state, 3)) {
                player->flashlight = lua_toboolean(state, 3) ? 1 : 0;
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "reload") {
            if(lua_isboolean(state, 3)) {
                player->reload = lua_toboolean(state, 3) ? 1 : 0;
            }
            else {
                return luaL_error(state, "Invalid value type");
            }
        }
        else if(field == "baselineUpdateXYAim") {
            player->baseline_update_xy_aim = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "baselineUpdateZAim") {
            player->baseline_update_z_aim = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "baselineUpdateForward") {
            player->baseline_update_forward = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "baselineUpdateLeft") {
            player->baseline_update_left = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "baselineUpdateRateOfFire") {
            player->baseline_update_rate_of_fire = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "baselineUpdateWeaponSlot") {
            player->baseline_update_weapon_slot = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "baselineUpdateGrenadeSlot") {
            player->baseline_update_grenade_slot = static_cast<std::uint16_t>(luaL_checkinteger(state, 3));
        }
        else if(field == "updateAiming") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "updatePosition") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_engine_player(lua_State *state, Engine::Player &player, bool read_only) noexcept {
        lua_push_meta_object(state, player, lua_engine_player__index, lua_engine_player__newindex, read_only);
    }

    static int lua_event_widget_render_vertex__index(lua_State *state) noexcept {
        auto *vertex = lua_from_meta_object<Event::UIWidgetRenderVertices::Vertex>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {
            return luaL_error(state, "Invalid key type");
        }
        
        std::string field = key;
        if(field == "x") {
            lua_pushnumber(state, vertex->x);
        }
        else if(field == "y") {
            lua_pushnumber(state, vertex->y);
        }
        else if(field == "z") {
            lua_pushnumber(state, vertex->z);
        }
        else if(field == "rhw") {
            lua_pushnumber(state, vertex->rhw);
        }
        else if(field == "u") {
            lua_pushnumber(state, vertex->u);
        }
        else if(field == "v") {
            lua_pushnumber(state, vertex->v);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_event_widget_render_vertex__newindex(lua_State *state) noexcept {
        auto *vertex = lua_from_meta_object<Event::UIWidgetRenderVertices::Vertex>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {
            return luaL_error(state, "Invalid key type");
        }
        
        std::string field = key;
        if(field == "x") {
            vertex->x = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "y") {
            vertex->y = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "z") {
            vertex->z = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "rhw") {
            vertex->rhw = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "u") {
            vertex->u = static_cast<float>(luaL_checknumber(state, 3));
        }
        else if(field == "v") {
            vertex->v = static_cast<float>(luaL_checknumber(state, 3));
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_event_widget_render_vertex(lua_State *state, Event::UIWidgetRenderVertices::Vertex &vertex, bool read_only) noexcept {
        lua_push_meta_object(state, vertex, lua_event_widget_render_vertex__index, lua_event_widget_render_vertex__newindex, read_only);
    }
    
    static int lua_event_widget_render_vertices__index(lua_State *state) noexcept {
        auto *vertices = lua_from_meta_object<Event::UIWidgetRenderVertices>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {
            return luaL_error(state, "Invalid key type");
        }
        
        std::string field = key;
        if(field == "topLeft") {
            lua_push_meta_event_widget_render_vertex(state, vertices->top_left);
        }
        else if(field == "topRight") {
            lua_push_meta_event_widget_render_vertex(state, vertices->top_right);
        }
        else if(field == "bottomLeft") {
            lua_push_meta_event_widget_render_vertex(state, vertices->bottom_left);
        }
        else if(field == "bottomRight") {
            lua_push_meta_event_widget_render_vertex(state, vertices->bottom_right);
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 1;
    }

    static int lua_event_widget_render_vertices__newindex(lua_State *state) noexcept {
        auto *vertices = lua_from_meta_object<Event::UIWidgetRenderVertices>(state, 1); 
        auto *key = lua_tostring(state, 2);

        if(key == nullptr) {
            return luaL_error(state, "Invalid key type");
        }
        
        std::string field = key;
        if(field == "topLeft") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "topRight") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "bottomLeft") {
            return luaL_error(state, "Invalid operation");
        }
        else if(field == "bottomRight") {
            return luaL_error(state, "Invalid operation");
        }
        else {
            return luaL_error(state, "Invalid key"); 
        }
        return 0;
    }

    void lua_push_meta_event_widget_render_vertices(lua_State *state, Event::UIWidgetRenderVertices &vertices, bool read_only) noexcept {
        lua_push_meta_object(state, vertices, lua_event_widget_render_vertices__index, lua_event_widget_render_vertices__newindex, read_only);
    }
}
