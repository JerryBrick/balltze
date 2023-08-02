// SPDX-License-Identifier: GPL-3.0-only

#include <windows.h>
#include <filesystem>
#include <vector>
#include <map>
#include <deque>
#include <memory>
#include <functional>
#include <numeric>
#include <optional>

#include <balltze/engine.hpp>
#include <balltze/utils.hpp>
#include <balltze/event.hpp>
#include <balltze/features.hpp>
#include <balltze/memory.hpp>
#include <balltze/hook.hpp>
#include "../../config/config.hpp"
#include "../../plugins/loader.hpp"
#include "../../logger.hpp"
#include "map.hpp"

namespace Balltze::Features {
    using namespace Engine;
    using namespace Engine::TagDefinitions;

    struct LoadedMap {
        std::string name;
        std::filesystem::path path;
        MapHeader header;
        std::optional<TagDataHeader> tag_data_header;
        std::vector<std::pair<std::string, Engine::TagClassInt>> tags_to_load;
        bool load_all_tags = false;

        std::size_t tag_data_size() const {
            return header.tag_data_size;
        }

        LoadedMap(const std::string name) {
            this->name = name;

            try {
                this->path = path_for_map_local(name.c_str());
            }
            catch (std::runtime_error &e) {
                throw;
            }

            // Read map data
            std::FILE *file = std::fopen(this->path.string().c_str(), "rb");
            std::fread(&header, sizeof(MapHeader), 1, file);
            std::fclose(file);

            if(header.engine_type == Engine::CACHE_FILE_CUSTOM_EDITION_COMPRESSED) {
                throw std::runtime_error("Map file is compressed");
            } 

            if(header.engine_type != Engine::CACHE_FILE_CUSTOM_EDITION) {
                throw std::runtime_error("Map file is not a Halo Custom Edition map");
            }
        }

        LoadedMap(std::filesystem::path path) {
            this->name = path.stem().string();
            this->path = path;

            if(!std::filesystem::exists(path)) {
                throw std::runtime_error("Map file does not exist");
            }

            // Read map header
            std::FILE *file = std::fopen(this->path.string().c_str(), "rb");
            std::fread(&header, sizeof(MapHeader), 1, file);
            std::fclose(file);

            if(header.engine_type == Engine::CACHE_FILE_CUSTOM_EDITION_COMPRESSED) {
                throw std::runtime_error("Map file is compressed");
            } 

            if(header.engine_type != Engine::CACHE_FILE_CUSTOM_EDITION) {
                throw std::runtime_error("Map file is not a Halo Custom Edition map");
            }
        }
    };

    static std::string loaded_map_name;
    static std::unique_ptr<MapHeader> loaded_map_header;
    static std::deque<LoadedMap> secondary_maps;
    static std::vector<Tag> tag_array;
    static std::unique_ptr<std::byte[]> tag_data;
    static std::size_t tag_data_buffer_size;
    static std::size_t tag_data_cursor = 0;
    static bool tag_data_loaded = false;

    extern "C" {
        void on_model_data_buffer_alloc_asm();
    }

    static std::byte *read_tag_data(LoadedMap &map) noexcept {
        std::FILE *file = std::fopen(map.path.string().c_str(), "rb");
        auto *map_tag_data = tag_data.get() + tag_data_cursor;
        std::fseek(file, map.header.tag_data_offset, SEEK_SET);
        tag_data_cursor += std::fread(map_tag_data, 1, map.header.tag_data_size, file);
        std::fclose(file);

        map.tag_data_header = *reinterpret_cast<TagDataHeader *>(map_tag_data);

        return map_tag_data;
    }

    static void load_tag_data() noexcept {
        if(secondary_maps.empty()) {
            return;
        }
        
        // Allocate buffer for maps tag data
        auto buffer_size = std::transform_reduce(secondary_maps.begin(), secondary_maps.end(), 0, std::plus<>(), std::mem_fn(LoadedMap::tag_data_size));
        if(buffer_size > tag_data_buffer_size) {
            tag_data = std::make_unique<std::byte[]>(buffer_size);
            tag_data_buffer_size = buffer_size;
        }
        tag_data_cursor = 0;

        // Read tag data header for main map
        auto &tag_data_header = get_tag_data_header();
        auto *tag_data_address = Engine::get_tag_data_address();
        auto data_base_offset = loaded_map_header->file_size;
        auto model_data_base_offset = tag_data_header.model_data_size;

        // Initialize tag array
        auto *tag_array_raw = reinterpret_cast<const Tag *>(tag_data_address + sizeof(TagDataHeader));
        tag_array = std::vector<Tag>(tag_array_raw, tag_array_raw + tag_data_header.tag_count);

        for(auto &map : secondary_maps) {
            const auto *map_tag_data = read_tag_data(map);
            const auto &map_tag_data_header = map.tag_data_header;

            std::map<TagHandle, TagHandle> tags_directory;
            auto tag_array_raw = reinterpret_cast<const Tag *>(map_tag_data + sizeof(TagDataHeader));
            auto tag_data_base_address_disp = reinterpret_cast<std::uint32_t>(tag_data_address) - reinterpret_cast<std::uint32_t>(map_tag_data);

            auto is_supported_tag = [](TagClassInt tag_class) {
                static TagClassInt unsupportedTags[] = {
                    TAG_CLASS_GLOBALS,
                    TAG_CLASS_HUD_GLOBALS,
                    TAG_CLASS_METER,
                    TAG_CLASS_SCENARIO_STRUCTURE_BSP,
                    TAG_CLASS_SCENARIO,
                };

                for(auto &i : unsupportedTags) {
                    if(i == tag_class) {
                        return false;
                    }
                }
                return true;
            };

            auto get_tag_from_secondary_map = [&map_tag_data_header, &tag_array_raw](TagHandle tag_handle) -> const Tag * {
                for(std::size_t i = 0; i < map_tag_data_header->tag_count; i++) {
                    if(tag_array_raw[i].handle == tag_handle) {
                        return &tag_array_raw[i];
                    }
                }
                return nullptr;
            };

            auto translate_address = [&tag_data_base_address_disp](auto address) {
                if(address != 0) {
                    address = reinterpret_cast<decltype(address)>(reinterpret_cast<std::uint32_t>(address) - tag_data_base_address_disp);
                }
                return address;
            };

            std::function<TagHandle (const Tag *, bool)> load_tag = [&](const Tag *tag, bool required) -> TagHandle {
                // Check if current tag class is supported
                if(!is_supported_tag(tag->primary_class)) {
                    if(required) {
                        show_error_box("Unsupported tag class %.*s", 4, reinterpret_cast<const char *>(&tag->primary_class));
                        std::exit(EXIT_FAILURE);
                    }
                    return TagHandle::null();
                }

                // Check if we've already loaded this tag
                if(tags_directory.find(tag->handle) != tags_directory.end()) {
                    return tags_directory.find(tag->handle)->second;
                }

                // Set up new tag entry
                auto &new_tag_entry = tag_array.emplace_back(*tag);
                new_tag_entry.path = translate_address(new_tag_entry.path);
                new_tag_entry.handle.index = tag_data_header.tag_count;
                new_tag_entry.handle.id = tag_data_header.tags_literal + tag_data_header.tag_count;
                tags_directory.insert_or_assign(tag->handle, new_tag_entry.handle);
                tag_data_header.tag_count++;

                // if current tag are indexed or if tags are already fixed, we can continue
                if(tag->indexed) {
                    if(new_tag_entry.primary_class == TAG_CLASS_SOUND) {
                        new_tag_entry.data = translate_address(new_tag_entry.data);
                    }
                    return new_tag_entry.handle;
                }

                // There's no data loaded to fix... yet
                if(tag->primary_class == TAG_CLASS_SCENARIO_STRUCTURE_BSP) {
                    return new_tag_entry.handle;
                }

                new_tag_entry.fix_data_offsets(translate_address(new_tag_entry.data), [&](std::uint32_t offset) -> std::uint32_t {
                    return new_tag_entry.indexed ? offset : offset + data_base_offset;
                });

                new_tag_entry.fix_dependencies([&](std::variant<TagDependency, TagHandle> elem) -> std::variant<TagDependency, TagHandle> {
                    if(std::holds_alternative<TagDependency>(elem)) {
                        auto &tag_dependency = std::get<TagDependency>(elem);
                        if(tag_dependency.tag_handle != -1) {
                            tag_dependency.path_pointer = translate_address(tag_dependency.path_pointer); 
                            auto *broken_tag = get_tag_from_secondary_map(tag_dependency.tag_handle);
                            auto tag_handle = load_tag(broken_tag, true); 
                            tag_dependency.tag_handle = tag_handle; 
                        }
                        return tag_dependency;
                    }
                    else {
                        auto &tag_handle = std::get<TagHandle>(elem);
                        auto *broken_tag = get_tag_from_secondary_map(tag_handle);
                        if(broken_tag) {
                            auto new_tag_handle = load_tag(broken_tag, true);
                            return new_tag_handle;
                        }
                        else {
                            if(tag_handle != TagHandle::null()) {
                                logger.debug("Cannot resolve tag {} in map {}", tag_handle.handle, map.name);
                            }
                            return tag_handle;
                        }
                    }
                });

                switch(new_tag_entry.primary_class) {
                    case TAG_CLASS_BITMAP: {
                        Bitmap *bitmap = reinterpret_cast<Bitmap *>(new_tag_entry.data);
                        if(bitmap->bitmap_data.count > 0) {
                            for(std::size_t j = 0; j < bitmap->bitmap_data.count; j++) {
                                bitmap->bitmap_data.offset[j].pixel_data_offset += data_base_offset; 
                            }
                        }
                        break;
                    }

                    case TAG_CLASS_FONT: {
                        auto *font = reinterpret_cast<Font *>(new_tag_entry.data);
                        for(std::size_t i = 0; i < font->characters.count; i++) {
                            font->characters.offset[i].pixels_offset += data_base_offset;
                        }
                        break;
                    }

                    case TAG_CLASS_GBXMODEL: {
                        auto *gbxmodel = reinterpret_cast<Gbxmodel *>(new_tag_entry.data);
                        for(std::size_t i = 0; i < gbxmodel->geometries.count; i++) {
                            for(std::size_t j = 0; j < gbxmodel->geometries.offset[i].parts.count; j++) {
                                gbxmodel->geometries.offset[i].parts.offset[j].vertex_offset += model_data_base_offset;
                                gbxmodel->geometries.offset[i].parts.offset[j].triangle_offset += model_data_base_offset - tag_data_header.vertex_size + map_tag_data_header->vertex_size;
                                gbxmodel->geometries.offset[i].parts.offset[j].triangle_offset_2 += model_data_base_offset - tag_data_header.vertex_size + map_tag_data_header->vertex_size;
                            }                               
                        }
                        break;
                    }

                    default: {
                        break;
                    }
                }

                return new_tag_entry.handle;
            };

            // Reserve space for tags to AVOID REALLOCATIONS
            tag_array.reserve(tag_array.size() + map_tag_data_header->tag_count);

            if(map.load_all_tags) {
                // Load all tags
                for(std::size_t i = 0; i < map_tag_data_header->tag_count; i++) {
                    load_tag(tag_array_raw + i, false);
                }
            }
            else {
                // Load tags
                for(auto &tag : map.tags_to_load) {
                    bool tag_found = false;
                    for(std::size_t i = 0; i < map_tag_data_header->tag_count; i++) {
                        std::string path = translate_address(tag_array_raw[i].path);
                        if(path == tag.first && tag_array_raw[i].primary_class == tag.second) {
                            if(load_tag(tag_array_raw + i, false) != TagHandle::null()) {
                                tag_found = true;
                            }
                            break;
                        }
                    }
                    if(!tag_found) {
                        logger.warning("Tag {} of class {} not found in map {}", tag.first, Engine::tag_class_to_string(tag.second), map.name);
                    }
                }
            }
        
            data_base_offset += map.header.file_size;
            model_data_base_offset += map_tag_data_header->model_data_size;
        }

        tag_data_header.tag_array = tag_array.data();
    }

    void import_tag_from_map(std::string map_name, std::string tag_path, Engine::TagClassInt tag_class) {
        try {
            for(auto &map : secondary_maps) {
                if(map.name == map_name) {
                    for(auto &tag : map.tags_to_load) {
                        if(tag.first == tag_path && tag.second == tag_class) {
                            return;
                        }
                    }
                    map.tags_to_load.emplace_back(tag_path, tag_class);
                    return;
                }
            }
            auto &map = secondary_maps.emplace_back(map_name);
            map.tags_to_load.emplace_back(tag_path, tag_class);
        }
        catch(std::exception &e) {
            logger.error("Failed to import tag {} from map {}: {}", tag_path, map_name, e.what());
        }
    }

    void import_tag_from_map(std::filesystem::path map_file, std::string tag_path, Engine::TagClassInt tag_class) {
        try {
            for(auto &map : secondary_maps) {
                if(map.path == map_file) {
                    for(auto &tag : map.tags_to_load) {
                        if(tag.first == tag_path && tag.second == tag_class) {
                            return;
                        }
                    }
                    map.tags_to_load.emplace_back(tag_path, tag_class);
                    return;
                }
            }
            auto &map = secondary_maps.emplace_back(map_file);
            map.tags_to_load.emplace_back(tag_path, tag_class);
        }
        catch(std::exception &e) {
            logger.debug("Failed to import tag {} from map {}: {}", tag_path, map_file.string(), e.what());
            throw;
        }
    }

    void import_tags_from_map(std::filesystem::path map_file) {
        try {
            for(auto &map : secondary_maps) {
                if(map.path == map_file) {
                    return;
                }
            }
            auto &map = secondary_maps.emplace_back(map_file);
            map.load_all_tags = true;
        }
        catch(std::runtime_error &e) {
            logger.debug("Failed to import tags from map {}: {}", map_file.string(), e.what());
            throw;
        }
    }

    void on_map_file_load(Event::MapFileLoadEvent const &event) {
        if(event.time == Event::EVENT_TIME_BEFORE) {
            // Read map data
            loaded_map_name = event.args.map_name;
            auto map_path = path_for_map_local(event.args.map_name.c_str());
            loaded_map_header = std::make_unique<MapHeader>();
            std::FILE *file = std::fopen(map_path.string().c_str(), "rb");
            std::fread(loaded_map_header.get(), sizeof(MapHeader), 1, file);
            std::fclose(file);

            if(loaded_map_header->head != MapHeader::HEAD_LITERAL || loaded_map_header->foot != MapHeader::FOOT_LITERAL) {
                throw std::runtime_error("Map file is corrupted");
            }

            // Clear secondary maps
            secondary_maps.clear();
            tag_data_loaded = false;
        }
    }

    void on_read_map_file_data(Event::MapFileDataReadEvent &event) {      
        if(secondary_maps.empty()) {
            return;
        }

        const HANDLE &file_descriptor = event.args.file_handle; 
        std::byte *output = event.args.output_buffer;
        const std::size_t &size = event.args.size;
        std::size_t file_offset = event.args.overlapped->Offset;

        // Get the name of the file we're reading from
        char file_path_chars[MAX_PATH + 1] = {};
        GetFinalPathNameByHandle(file_descriptor, file_path_chars, sizeof(file_path_chars) - 1, VOLUME_NAME_NONE);
        auto file_path = std::filesystem::path(file_path_chars);

        // Check if we're reading from the current loaded map
        auto file_name = file_path.filename();
        if(file_name.stem().string() != loaded_map_name) {
            return;
        }

        if(event.time == Event::EVENT_TIME_BEFORE) {
            if(output == get_tag_data_address() || !tag_data_loaded) {
                return;
            }

            // Look for secondary maps offset displacement
            auto offset_acc = loaded_map_header->file_size;
            if(file_offset > offset_acc) {
                for(auto &map : secondary_maps) {
                    auto map_file_size = map.header.file_size;
                    if(file_offset <= offset_acc + map_file_size) {
                        std::FILE *file = std::fopen(map.path.string().c_str(), "rb");
                        std::fseek(file, file_offset - offset_acc, SEEK_SET);
                        std::fread(output, 1, size, file);
                        std::fclose(file);
                        event.args.size = 0;
                        return;
                    }
                    offset_acc += map_file_size;
                }
                show_error_box("File offset %zu is out of bounds! Map file size: %zu", file_offset, offset_acc);
            }

            // Read model data ONLY for secondary maps
            const auto &tag_data_header = Engine::get_tag_data_header();
            if(tag_data_header.model_data_file_offset == file_offset && tag_data_header.model_data_size == size) {
                std::size_t buffer_cursor = tag_data_header.model_data_size;
                for(auto &map : secondary_maps) {
                    const auto map_tag_data_header = map.tag_data_header;
                    std::FILE *file = std::fopen(map.path.string().c_str(), "rb");
                    std::fseek(file, map_tag_data_header->model_data_file_offset, SEEK_SET);
                    std::fread(output + buffer_cursor, 1, map_tag_data_header->model_data_size, file);
                    std::fclose(file);
                    buffer_cursor += map_tag_data_header->model_data_size;
                }
            }
        }
        else {
            if(event.args.output_buffer == get_tag_data_address()) {
                load_tag_data();
                logger.debug("Tag data from secondary maps loaded");
                tag_data_loaded = true;
            }
        }
    }

    extern "C" void on_model_data_buffer_alloc(std::size_t *size) {
        for(auto &map : secondary_maps) {
            *size += map.tag_data_header->model_data_size;
        }
    }

    void set_up_tag_data_importing() {
        Event::MapFileLoadEvent::subscribe_const(on_map_file_load);
        Event::MapFileDataReadEvent::subscribe(on_read_map_file_data);

        auto *model_data_buffer_alloc_sig = Memory::get_signature("model_data_buffer_alloc");
        auto *model_data_buffer_alloc_hook = Memory::hook_function(model_data_buffer_alloc_sig->data(), on_model_data_buffer_alloc_asm);
    }
}
