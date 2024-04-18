// SPDX-License-Identifier: GPL-3.0-only

#include <cstdint>
#include <balltze/engine/renderer.hpp>
#include <balltze/engine/tag.hpp>
#include <balltze/engine/tag_definitions.hpp>
#include <balltze/event.hpp>
#include <balltze/memory.hpp>
#include <balltze/hook.hpp>
#include <balltze/command.hpp>
#include "../../logger.hpp"

namespace Balltze::Features {
    using Engine::TagDefinitions::ShaderTransparentChicago;
    using Engine::TagDefinitions::Bitmap;
    using Engine::TagDefinitions::BitmapDataType;

    struct TransparentGeometryGroup {
        float field0_0x0;
        struct TransparentGeometryGroup *field0_0x4;
        char field2_0x8;
        char field3_0x9;
        char field4_0xa;
        char field5_0xb;
        std::byte *shader_tag_data;
        unsigned short field7_0x10;
        char field8_0x12;
        char field9_0x13;
        short field10_0x14;
        char field11_0x16;
        char field12_0x17;
        float field13_0x18;
        char field14_0x1c;
        char field15_0x1d;
        char field16_0x1e;
        char field17_0x1f;
        char field18_0x20;
        char field19_0x21;
        char field20_0x22;
        char field21_0x23;
        char field22_0x24;
        char field23_0x25;
        char field24_0x26;
        char field25_0x27;
        char field26_0x28;
        char field27_0x29;
        char field28_0x2a;
        char field29_0x2b;
        char field30_0x2c;
        char field31_0x2d;
        char field32_0x2e;
        char field33_0x2f;
        char field34_0x30;
        char field35_0x31;
        char field36_0x32;
        char field37_0x33;
        char field38_0x34;
        char field39_0x35;
        char field40_0x36;
        char field41_0x37;
        char field42_0x38;
        char field43_0x39;
        char field44_0x3a;
        char field45_0x3b;
        float field46_0x3c;
        float field47_0x40;
        char field48_0x44;
        char field49_0x45;
        char field50_0x46;
        char field51_0x47;
        char field52_0x48;
        char field53_0x49;
        char field54_0x4a;
        char field55_0x4b;
        char field56_0x4c;
        char field57_0x4d;
        char field58_0x4e;
        char field59_0x4f;
        char field60_0x50;
        char field61_0x51;
        char field62_0x52;
        char field63_0x53;
        int stage; 
        short *field65_0x58;
        char field66_0x5c;
        char field67_0x5d;
        char field68_0x5e;
        char field69_0x5f;
        char field70_0x60;
        char field71_0x61;
        char field72_0x62;
        char field73_0x63;
        char field74_0x64;
        char field75_0x65;
        char field76_0x66;
        char field77_0x67;
        char field78_0x68;
        char field79_0x69;
        char field80_0x6a;
        char field81_0x6b;
        char field82_0x6c;
        char field83_0x6d;
        char field84_0x6e;
        char field85_0x6f;
        char field86_0x70;
        char field87_0x71;
        char field88_0x72;
        char field89_0x73;
        int *field90_0x74;
        float field91_0x78;
        char field92_0x79[44];
    };
    static_assert(sizeof(TransparentGeometryGroup) == 0xA8);

    static IDirect3DDevice9 *device = nullptr;
    static auto *transparent_geometry_group_count = reinterpret_cast<std::uint32_t *>(0x00df5fa4);
    static auto *transparent_geometry_groups = reinterpret_cast<TransparentGeometryGroup *>(0x00df5f9c);

    extern "C" {
        bool use_shader_transparent_chicago_reimplementation = true;
        bool draw_shader_transparent_chicago_hook();
        int FUN_00543160(ShaderTransparentChicago *shader_data);
        int FUN_0051be80(short param_1);
        double FUN_005c8b40(double param_1);
        int FUN_005434c0(int param_1);

        bool FUN_0051c060(short texture_stage, short bitmap_data_type, short bitmap_data_index, short unknown);
        
        void FUN_00543250(std::byte* tex_anim, float map_v_scale, float map_v_scalez, float map_u_offset, float map_v_offset, float map_rotation, float param_7, unsigned int param_8, float * param_9, float * param_10);

        void rasterizer_transparent_geometry_group_draw(TransparentGeometryGroup *group, std::uint32_t *param_2);
        void rasterizer_set_texture_bitmap(short texture_stage, BitmapDataType bitmap_type, short bitmap_data_index, short param_4, Engine::TagHandle bitmap_tag_handle);

        void FUN_0053ae90(std::byte *shader_data);
    
        void draw_shader_transparent_chicago(TransparentGeometryGroup *transparent_geometry_group, std::uint32_t *param_2) {
            if(!device) {
                return;
            }
            
            auto *shader_data = reinterpret_cast<ShaderTransparentChicago *>(transparent_geometry_group->shader_tag_data);
            short unknown_type = FUN_00543160(shader_data);
            
            short texture_stage = -1;
            if(transparent_geometry_group->field65_0x58 == nullptr) {
                if(transparent_geometry_group->stage != -1) {
                    texture_stage = reinterpret_cast<short *>(0x00674908)[transparent_geometry_group->stage * 8];
                }
            }
            else {
                texture_stage = *transparent_geometry_group->field65_0x58;
            }

            short local_f0 = transparent_geometry_group->field7_0x10;
            
            auto *maps_elements = shader_data->maps.offset;
            if(maps_elements == nullptr) {
                return;
            }
            if(maps_elements->map.path == 0) {
                return;
            }

            device->SetVertexShader((IDirect3DVertexShader9 *)(0x00639248 + (*(short *)(0x00639450 + (unknown_type + texture_stage * 6) * 2) * 2)));
            device->SetVertexDeclaration((IDirect3DVertexDeclaration9 *)(0x0067ca50 + (texture_stage * 3)));
            device->SetPixelShader(nullptr);
            texture_stage = 0;

            for(int i = 0; i < shader_data->extra_layers.count; i++) {
                TransparentGeometryGroup group_copy;
                auto *extra_layer_elements = shader_data->extra_layers.offset;
                memcpy(&group_copy, transparent_geometry_group, sizeof(TransparentGeometryGroup));
                auto *extra_layer_shader_tag = Engine::get_tag(extra_layer_elements[i].shader.tag_handle);
                if(extra_layer_shader_tag) {
                    group_copy.shader_tag_data = extra_layer_shader_tag->data;
                    rasterizer_transparent_geometry_group_draw(&group_copy, param_2);
                }
            }

            auto flags = *reinterpret_cast<std::uint32_t *>(&shader_data->shader_transparent_chicago_flags);

            device->SetRenderState(D3DRS_CULLMODE, ~(std::uint32_t)(flags >> 1) & 2 | 1);
            device->SetRenderState(D3DRS_COLORWRITEENABLE, 7);
            device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
            device->SetRenderState(D3DRS_ALPHATESTENABLE, flags & 1);
            device->SetRenderState(D3DRS_ALPHAREF, 0x7f);
            device->SetRenderState(D3DRS_FOGENABLE, 0);

            FUN_0051be80(shader_data->framebuffer_blend_function);

            int unknown_bitmap_data_val;
            if(flags < 0 && transparent_geometry_group->field90_0x74 != nullptr && shader_data->maps.count > 0) {
                auto *bitmap_tag = Engine::get_tag(shader_data->maps.offset[0].map.tag_handle);
                if(bitmap_tag) {
                    auto *bitmap_data = reinterpret_cast<Bitmap *>(bitmap_tag->data);
                    auto bitmap_count = bitmap_data->bitmap_data.count;
                    auto extra_flags = *reinterpret_cast<std::uint32_t *>(&shader_data->extra_flags);
                    if(extra_flags & 2 == 0) {
                        float numeric_count_limit = shader_data->numeric_counter_limit;
                        short unknown_val = (((bitmap_count != 8 ? 1 : 0) - 1) & 3) * 4; // weird
                        float res = FUN_005c8b40(numeric_count_limit * static_cast<float>(transparent_geometry_group->field90_0x74[1] + unknown_val) + 0.5f);
                        if(round(res) < 0) {
                            res = 0;
                        }
                        else {
                            res = FUN_005c8b40(numeric_count_limit * static_cast<float>(transparent_geometry_group->field90_0x74[1] + unknown_val) + 0.5f);
                            if(round(res) <= numeric_count_limit) {
                                res = FUN_005c8b40(numeric_count_limit * static_cast<float>(transparent_geometry_group->field90_0x74[1] + unknown_val) + 0.5f);
                                numeric_count_limit = round(res);
                            }
                        }

                        short unknown_bitmap_val = numeric_count_limit;
                        if(transparent_geometry_group->field7_0x10 > 0) {
                            auto i = transparent_geometry_group->field7_0x10;
                            do {
                                numeric_count_limit = numeric_count_limit / bitmap_count;
                                unknown_bitmap_val = numeric_count_limit;
                                i--;
                            } while(i != 0);
                        }
                        unknown_bitmap_data_val = unknown_bitmap_val % bitmap_count;
                    }
                    else {
                        unknown_bitmap_data_val = FUN_005434c0(transparent_geometry_group->field7_0x10);
                    }
                }
            }

            float vertex_shader_constants[38];

            for(int i = 0; i < 4; i++) {
                int sampler_stage_index = i;
                if(shader_data->maps.count > sampler_stage_index) {
                    float first_map_type = shader_data->first_map_type;
                    auto *map = shader_data->maps.offset + sampler_stage_index;
                    BitmapDataType bitmap_data_type;
                    if(i == 0) {
                        bitmap_data_type = *reinterpret_cast<BitmapDataType *>(0x005fc9d0 + static_cast<int>(round(first_map_type * 2)));
                    }
                    else {
                        bitmap_data_type = BitmapDataType::BITMAP_DATA_TYPE_2D_TEXTURE;
                    }

                    rasterizer_set_texture_bitmap(i, bitmap_data_type, 0, unknown_bitmap_data_val, map->map.tag_handle);

                    D3DTEXTUREADDRESS u_texture_mode;
                    D3DTEXTUREADDRESS v_texture_mode;
                    D3DTEXTUREADDRESS alternative_mode = *reinterpret_cast<D3DTEXTUREADDRESS *>(0x005fc9d8 + static_cast<int>(round(first_map_type * 4)));
                    if(bitmap_data_type == BitmapDataType::BITMAP_DATA_TYPE_2D_TEXTURE && map->flags.u_clamped) {
                        u_texture_mode = D3DTADDRESS_CLAMP;
                        if(map->flags.v_clamped) {
                            v_texture_mode = D3DTADDRESS_CLAMP;
                        }
                        else {
                            if(i == 0) {
                                v_texture_mode = alternative_mode;
                            }
                            else {
                                v_texture_mode = D3DTADDRESS_WRAP;
                            }
                        }
                    } 
                    else {
                        if(i == 0) {
                            u_texture_mode = alternative_mode;
                        }
                        else {
                            u_texture_mode = D3DTADDRESS_WRAP;
                        }

                        if(bitmap_data_type == BitmapDataType::BITMAP_DATA_TYPE_2D_TEXTURE) {
                            if(map->flags.v_clamped) {
                                v_texture_mode = D3DTADDRESS_CLAMP;
                            }
                            else {
                                if(i == 0) {
                                    v_texture_mode = alternative_mode;
                                }
                                else {
                                    v_texture_mode = D3DTADDRESS_WRAP;
                                }
                            }
                        }
                        else {
                            if(i == 0) {
                                v_texture_mode = alternative_mode;
                            }
                            else {
                                v_texture_mode = D3DTADDRESS_WRAP;
                            }
                        }
                    }

                    device->SetSamplerState(sampler_stage_index, D3DSAMP_ADDRESSU, u_texture_mode);
                    device->SetSamplerState(sampler_stage_index, D3DSAMP_ADDRESSV, v_texture_mode);

                    if(i == 0) {
                        u_texture_mode = alternative_mode;
                    }
                    else {
                        alternative_mode = D3DTADDRESS_WRAP;
                    }

                    auto map_flags = *reinterpret_cast<std::uint32_t *>(&map->flags);

                    device->SetSamplerState(sampler_stage_index, D3DSAMP_ADDRESSW, u_texture_mode);
                    device->SetSamplerState(sampler_stage_index, D3DSAMP_MAGFILTER, 2);
                    device->SetSamplerState(sampler_stage_index, D3DSAMP_MINFILTER, 2 - map->flags.unfiltered);
                    device->SetSamplerState(sampler_stage_index, D3DSAMP_MIPFILTER, 2 - map->flags.unfiltered);
                }

                auto maps_count = shader_data->maps.count;
                if(sampler_stage_index < maps_count) {
                    if(i < 1 && shader_data->first_map_type != 0) {
                        if(maps_count <= sampler_stage_index || shader_data->shader_transparent_chicago_flags.first_map_is_in_screenspace == 0) {
                            vertex_shader_constants[sampler_stage_index * 8] = 1.0;
                            vertex_shader_constants[sampler_stage_index * 8 + 1] = 0.0;
                            vertex_shader_constants[sampler_stage_index * 8 + 2] = 0.0;
                            vertex_shader_constants[sampler_stage_index * 8 + 4] = 0.0;
                            vertex_shader_constants[sampler_stage_index * 8 + 5] = 1.0;
                            vertex_shader_constants[sampler_stage_index * 8 + 6] = 0.0;
                            vertex_shader_constants[sampler_stage_index * 8 + 3] = 0.0;
                            vertex_shader_constants[sampler_stage_index * 8 + 7] = 0.0;
                        } 
                        else {
                            vertex_shader_constants[sampler_stage_index * 8] = *reinterpret_cast<float *>(0x0075c624);
                            vertex_shader_constants[sampler_stage_index * 8 + 1] = *reinterpret_cast<float *>(0x0075c628);
                            vertex_shader_constants[sampler_stage_index * 8 + 2] = *reinterpret_cast<float *>(0x0075c62c);
                            vertex_shader_constants[sampler_stage_index * 8 + 4] = *reinterpret_cast<float *>(0x0075c630);
                            vertex_shader_constants[sampler_stage_index * 8 + 5] = *reinterpret_cast<float *>(0x0075c634);
                            vertex_shader_constants[sampler_stage_index * 8 + 6] = *reinterpret_cast<float *>(0x0075c638);
                            vertex_shader_constants[sampler_stage_index * 8 + 3] = 0.0;
                            vertex_shader_constants[sampler_stage_index * 8 + 7] = 0.0;
                        }
                    }
                    else {
                        auto *map_elements = shader_data->maps.offset;
                        auto map_v_scale = map_elements[sampler_stage_index].map_v_scale;
                        auto map_u_scale = map_elements[sampler_stage_index].map_u_scale;
                        if(i == 0) {
                            if(shader_data->shader_transparent_chicago_flags.scale_first_map_with_distance) {
                                map_u_scale = -(map_u_scale * transparent_geometry_group->field91_0x78);
                                map_v_scale = -(map_v_scale * transparent_geometry_group->field91_0x78);
                            }

                            if(shader_data->shader_transparent_chicago_flags.first_map_is_in_screenspace) {
                                map_u_scale = map_u_scale * transparent_geometry_group->field46_0x3c;
                                map_v_scale = map_v_scale * transparent_geometry_group->field47_0x40;
                            }
                        }
                        else {
                            if(i >= 1 || shader_data->shader_transparent_chicago_flags.first_map_is_in_screenspace) {
                                map_u_scale = map_u_scale * transparent_geometry_group->field46_0x3c;
                                map_v_scale = map_v_scale * transparent_geometry_group->field47_0x40;
                            }
                        }

                        FUN_00543250(reinterpret_cast<std::byte *>(&map_elements[sampler_stage_index].u_animation_source), map_u_scale, map_v_scale, map_elements[sampler_stage_index].map_u_offset, map_elements[sampler_stage_index].map_v_offset, map_elements[sampler_stage_index].map_rotation, 0x0075c570, static_cast<unsigned int>(*transparent_geometry_group->field90_0x74), vertex_shader_constants + sampler_stage_index * 8, vertex_shader_constants + sampler_stage_index * 8 + 4);
                    }
                }
                else {
                    vertex_shader_constants[sampler_stage_index * 8] = 1.0;
                    vertex_shader_constants[sampler_stage_index * 8 + 1] = 0.0;
                    vertex_shader_constants[sampler_stage_index * 8 + 2] = 0.0;
                    vertex_shader_constants[sampler_stage_index * 8 + 4] = 0.0;
                    vertex_shader_constants[sampler_stage_index * 8 + 5] = 1.0;
                    vertex_shader_constants[sampler_stage_index * 8 + 6] = 0.0;
                    
                    vertex_shader_constants[sampler_stage_index * 8 + 3] = 0.0;
                    vertex_shader_constants[sampler_stage_index * 8 + 7] = 0.0;
                }
            }

            auto res = device->SetVertexShaderConstantF(13, vertex_shader_constants, 8);
            if(res != D3D_OK) {
                FUN_0053ae90(reinterpret_cast<std::byte *>(transparent_geometry_group->shader_tag_data));
            }
        }
    }

    static bool switch_command(int arg_count, const char **args) {
        if(arg_count == 1) {
            bool new_setting = STR_TO_BOOL(args[0]);
            use_shader_transparent_chicago_reimplementation = new_setting;
        }
        logger.info("use_shader_transparent_chicago_reimplementation: {}", use_shader_transparent_chicago_reimplementation);
        Engine::console_printf("use_shader_transparent_chicago_reimplementation: %s", BOOL_TO_STR(use_shader_transparent_chicago_reimplementation));
        return true;
    }

    void set_up_shader_transparent_chicago_impl() {
        static auto *sig = Memory::get_signature("draw_shader_transparent_chicago_function_call");
        if(!sig) {
            throw std::runtime_error("Failed to set up shader transparent chicago reimplementation: signature not found");
        }
        
        try {
            std::function<bool()> function_hook = draw_shader_transparent_chicago_hook;
            Memory::hook_function(sig->data(), function_hook);   
        } 
        catch(std::runtime_error &e) {
            throw std::runtime_error("Failed to set up shader transparent chicago reimplementation: " + std::string(e.what()));
        }

        // Set up switch command
        register_command("use_shader_transparent_chicago_reimplementation", "features", "Use the reimplementation of the shader transparent chicago", std::nullopt, switch_command, true, 0, 1);

        Event::D3D9BeginSceneEvent::subscribe([](Event::D3D9BeginSceneEvent &event) {
            if(event.time == Event::EVENT_TIME_AFTER) {
                device = event.args.device;
            }
        });
    }
}
