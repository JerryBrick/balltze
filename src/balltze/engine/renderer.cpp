// SPDX-License-Identifier: GPL-3.0-only

#include <optional>
#include <balltze/engine/renderer.hpp>

namespace Balltze::Engine {
    WindowGlobals *get_window_globals() {
        static std::optional<WindowGlobals *> window_globals;
        if(!window_globals.has_value()) {
            auto *window_globals_sig = Memory::get_signature("window_globals");
            if(!window_globals_sig) {
                throw std::runtime_error("window_globals signature not found");
            }
            window_globals = *reinterpret_cast<WindowGlobals **>(window_globals_sig->data());
        }
        return *window_globals;
    }
    
    Resolution &get_resolution() noexcept {
        static auto *resolution_sig = Memory::get_signature("resolution");
        static Resolution *resolution = *reinterpret_cast<Resolution **>(resolution_sig->data());
        return *resolution;
    }

    IDirect3DDevice9 **get_d3d9_device() noexcept {
        static auto *device_sig = Memory::get_signature("d3d9_device_pointer");
        static IDirect3DDevice9 **device = *reinterpret_cast<IDirect3DDevice9 ***>(device_sig->data());
        return device;
    }

    VertexShader *get_vertex_shader(const char *name) {
        
    }

    extern "C" {
        void *load_bitmap_function_address = nullptr;
        IDirect3DTexture9 *load_bitmap_asm(TagDefinitions::BitmapData *bitmap_data, bool immediate, bool force_pixels_read);
    }

    IDirect3DTexture9 *load_bitmap(TagDefinitions::BitmapData *bitmap_data, bool immediate, bool force_pixels_read) {
        static auto *load_bitmap_sig = Memory::get_signature("load_bitmap_function");
        if(!load_bitmap_sig) {
            throw std::runtime_error("Could not find signature for bitmap load function");
        }
        if(!load_bitmap_function_address) {
            load_bitmap_function_address = reinterpret_cast<void *>(load_bitmap_sig->data());
        }
        return load_bitmap_asm(bitmap_data, immediate, force_pixels_read);
    }

    void set_bitmap_texture(IDirect3DDevice9 *device, std::size_t stage, TagDefinitions::BitmapData *bitmap_data) {
        if(bitmap_data->texture_cache_id == 0xFFFFFFFF) {
            try {
                load_bitmap(bitmap_data, true, true);
            }
            catch(...) {
                throw;
            }
        }
        device->SetTexture(stage, reinterpret_cast<IDirect3DBaseTexture9 *>(bitmap_data->texture));
    }

    RenderTarget *get_render_target(std::size_t index) {
        static auto *render_targets_sig = Memory::get_signature("d3d9_render_targets");
        if(!render_targets_sig) {
            throw std::runtime_error("Could not find signature for render targets");
        }
        if(index > 8) {
            throw std::runtime_error("Index out of bounds");
        }
        return *reinterpret_cast<RenderTarget **>(render_targets_sig->data()) + index;
    }

    extern "C" {
        void *render_ui_function_address = nullptr;
        void render_ui_asm(std::uint32_t param_1);
    }

    void render_ui(std::uint32_t param_1) {
        static auto *render_ui_function_sig = Memory::get_signature("render_ui_function");
        if(!render_ui_function_sig) {
            throw std::runtime_error("Could not find signature for UI render function");
        }
        if(!render_ui_function_address) {
            render_ui_function_address = reinterpret_cast<void *>(render_ui_function_sig->data());
        }
        render_ui_asm(param_1);
    }

    void render_hud() {
        static auto *render_hud_function_sig = Memory::get_signature("render_hud_function");
        if(!render_hud_function_sig) {
            throw std::runtime_error("Could not find signature for HUD render function");
        }
        auto *render_hud_function = reinterpret_cast<void(*)()>(render_hud_function_sig->data());
        render_hud_function();
    }

    void render_post_carnage_report() {
        static auto *render_post_carnage_report_function_sig = Memory::get_signature("render_post_carnage_report_function");
        if(!render_post_carnage_report_function_sig) {
            throw std::runtime_error("Could not find signature for post carnage report render function");
        }
        auto *render_post_carnage_report_function = reinterpret_cast<void(*)()>(render_post_carnage_report_function_sig->data());
        render_post_carnage_report_function();
    }
}
