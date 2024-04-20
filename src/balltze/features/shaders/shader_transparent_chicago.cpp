// SPDX-License-Identifier: GPL-3.0-only

#include <cstdint>
#include <cmath>
#include <balltze/engine/renderer.hpp>
#include <balltze/engine/tag.hpp>
#include <balltze/engine/tag_definitions.hpp>
#include <balltze/event.hpp>
#include <balltze/memory.hpp>
#include <balltze/hook.hpp>
#include <balltze/command.hpp>
#include "../../logger.hpp"

namespace Balltze::Features {
    using namespace Engine::TagDefinitions;

    struct TransparentGeometryGroup {
        std::uint32_t field0_0x0;
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
        float map_u_coord;
        float map_v_coord;
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
        float distance;
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

        void FUN_00536550(TransparentGeometryGroup *group, bool param_2);
    
        void draw_shader_transparent_chicago(TransparentGeometryGroup *transparent_geometry_group, std::uint32_t *param_2) {
            if(!device) {
                return;
            }

            auto *shader_data = reinterpret_cast<ShaderTransparentChicago *>(transparent_geometry_group->shader_tag_data);
            short unknown_type = FUN_00543160(shader_data);

            int map_index = -1;
            if(transparent_geometry_group->field65_0x58 == nullptr) {
                if(transparent_geometry_group->stage != -1) {
                    map_index = reinterpret_cast<short *>(0x00674908)[transparent_geometry_group->stage * 8];
                }
            }
            else {
                map_index = *transparent_geometry_group->field65_0x58;
            }

            short bitmap_data_index = transparent_geometry_group->field7_0x10;
            
            auto *maps_elements = shader_data->maps.offset;
            if(maps_elements == nullptr) {
                return;
            }
            if(maps_elements->map.path == 0) {
                return;
            }

            auto **vertex_shaders = reinterpret_cast<IDirect3DVertexShader9 **>(0x00639248);
            auto *unknown_vertex_shader_addr = reinterpret_cast<std::byte *>(0x00639450);
            auto **vertex_declarations = reinterpret_cast<IDirect3DVertexDeclaration9 **>(0x0067ca50);
            device->SetVertexShader(vertex_shaders[(*reinterpret_cast<short *>(unknown_vertex_shader_addr + (unknown_type + map_index * 6) * 2) * 2)]);
            device->SetVertexDeclaration(vertex_declarations[map_index * 3]);
            device->SetPixelShader(nullptr);
            map_index = 0;

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

            auto &flags = shader_data->shader_transparent_chicago_flags;
            D3DCULL render_cullmode;
            if(!flags.decal && !flags.two_sided) {
                render_cullmode = D3DCULL_CCW;
            }
            else if(flags.decal && !flags.two_sided) {
                render_cullmode = D3DCULL_CW;
            }
            else {
                render_cullmode = D3DCULL_NONE;
            }
            device->SetRenderState(D3DRS_CULLMODE, render_cullmode);
            device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
            device->SetRenderState(D3DRS_ALPHABLENDENABLE, D3DBLENDOP_ADD);
            device->SetRenderState(D3DRS_ALPHATESTENABLE, flags.alpha_tested ? TRUE : FALSE);
            device->SetRenderState(D3DRS_ALPHAREF, 0x0000007F);
            device->SetRenderState(D3DRS_FOGENABLE, FALSE);

            FUN_0051be80(shader_data->framebuffer_blend_function);

            if(flags.numeric && transparent_geometry_group->field90_0x74 != nullptr && shader_data->maps.count > 0) {
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
                        bitmap_data_index = unknown_bitmap_val % bitmap_count;
                    }
                    else {
                        bitmap_data_index = FUN_005434c0(transparent_geometry_group->field7_0x10);
                    }
                }
            }

            float vertex_shader_constants[8 * 4];

            for(int i = 0; i < 4; i++) {
                map_index = i;
                if(shader_data->maps.count > map_index) {
                    float first_map_type = shader_data->first_map_type;
                    auto *map = shader_data->maps.offset + map_index;
                    BitmapDataType bitmap_data_type;
                    if(map_index == 0) {
                        bitmap_data_type = reinterpret_cast<BitmapDataType *>(0x005fc9d0)[shader_data->first_map_type];
                    }
                    else {
                        bitmap_data_type = BitmapDataType::BITMAP_DATA_TYPE_2D_TEXTURE;
                    }

                    rasterizer_set_texture_bitmap(map_index, bitmap_data_type, 0, bitmap_data_index, map->map.tag_handle);

                    D3DTEXTUREADDRESS u_texture_mode;
                    D3DTEXTUREADDRESS v_texture_mode;
                    D3DTEXTUREADDRESS w_texture_mode;
                    D3DTEXTUREADDRESS alternative_mode = reinterpret_cast<D3DTEXTUREADDRESS *>(0x005fc9d8)[shader_data->first_map_type];
                    if(bitmap_data_type == BitmapDataType::BITMAP_DATA_TYPE_2D_TEXTURE && map->flags.u_clamped) {
                        u_texture_mode = D3DTADDRESS_CLAMP;
                        if(map->flags.v_clamped) {
                            v_texture_mode = D3DTADDRESS_CLAMP;
                        }
                        else {
                            if(map_index == 0) {
                                v_texture_mode = alternative_mode;
                            }
                            else {
                                v_texture_mode = D3DTADDRESS_WRAP;
                            }
                        }
                    } 
                    else {
                        if(map_index == 0) {
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
                                if(map_index == 0) {
                                    v_texture_mode = alternative_mode;
                                }
                                else {
                                    v_texture_mode = D3DTADDRESS_WRAP;
                                }
                            }
                        }
                        else {
                            if(map_index == 0) {
                                v_texture_mode = alternative_mode;
                            }
                            else {
                                v_texture_mode = D3DTADDRESS_WRAP;
                            }
                        }
                    }
                    if(map_index == 0) {
                        w_texture_mode = alternative_mode;
                    }
                    else {
                        w_texture_mode = D3DTADDRESS_WRAP;
                    }

                    device->SetSamplerState(map_index, D3DSAMP_ADDRESSU, u_texture_mode);
                    device->SetSamplerState(map_index, D3DSAMP_ADDRESSV, v_texture_mode);
                    device->SetSamplerState(map_index, D3DSAMP_ADDRESSW, w_texture_mode);
                    device->SetSamplerState(map_index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                    device->SetSamplerState(map_index, D3DSAMP_MINFILTER, map->flags.unfiltered ? D3DTEXF_POINT : D3DTEXF_LINEAR);
                    device->SetSamplerState(map_index, D3DSAMP_MIPFILTER, map->flags.unfiltered ? D3DTEXF_POINT : D3DTEXF_LINEAR);
                }

                auto maps_count = shader_data->maps.count;
                if(map_index < maps_count) {
                    if(i < 1 && shader_data->first_map_type != 0) {
                        if(maps_count <= map_index || shader_data->shader_transparent_chicago_flags.first_map_is_in_screenspace == 0) {
                            vertex_shader_constants[map_index * 8 + 0] = 1.0;
                            vertex_shader_constants[map_index * 8 + 1] = 0.0;
                            vertex_shader_constants[map_index * 8 + 2] = 0.0;
                            vertex_shader_constants[map_index * 8 + 3] = 0.0;
                            vertex_shader_constants[map_index * 8 + 4] = 0.0;
                            vertex_shader_constants[map_index * 8 + 5] = 1.0;
                            vertex_shader_constants[map_index * 8 + 6] = 0.0;
                            vertex_shader_constants[map_index * 8 + 7] = 0.0;
                        } 
                        else {
                            vertex_shader_constants[map_index * 8 + 0] = *reinterpret_cast<float *>(0x0075c624);
                            vertex_shader_constants[map_index * 8 + 1] = *reinterpret_cast<float *>(0x0075c628);
                            vertex_shader_constants[map_index * 8 + 2] = *reinterpret_cast<float *>(0x0075c62c);
                            vertex_shader_constants[map_index * 8 + 3] = 0.0;
                            vertex_shader_constants[map_index * 8 + 4] = *reinterpret_cast<float *>(0x0075c630);
                            vertex_shader_constants[map_index * 8 + 5] = *reinterpret_cast<float *>(0x0075c634);
                            vertex_shader_constants[map_index * 8 + 6] = *reinterpret_cast<float *>(0x0075c638);
                            vertex_shader_constants[map_index * 8 + 7] = 0.0;
                        }
                    }
                    else {
                        auto *map_elements = shader_data->maps.offset;
                        auto map_v_scale = map_elements[map_index].map_v_scale * transparent_geometry_group->map_v_coord;
                        auto map_u_scale = map_elements[map_index].map_u_scale * transparent_geometry_group->map_u_coord;
                        auto &map = map_elements[map_index];
                        auto *animation_data = reinterpret_cast<std::byte *>(&map.u_animation_source);
                        auto map_u_offset = map.map_u_offset;
                        auto map_v_offset = map.map_v_offset;
                        auto map_rotation = map.map_rotation;
                        FUN_00543250(animation_data, 
                                    map_u_scale, 
                                    map_v_scale, 
                                    map_u_offset,
                                    map_v_offset, 
                                    map_rotation, 
                                    *reinterpret_cast<float *>(0x0075c570), 
                                    reinterpret_cast<unsigned int>(transparent_geometry_group->field90_0x74),
                                    &vertex_shader_constants[map_index * 8 + 0], 
                                    &vertex_shader_constants[map_index * 8 + 4]);
                    }
                }
                else {
                    vertex_shader_constants[map_index * 8 + 0] = 1.0;
                    vertex_shader_constants[map_index * 8 + 1] = 0.0;
                    vertex_shader_constants[map_index * 8 + 2] = 0.0;
                    vertex_shader_constants[map_index * 8 + 3] = 0.0;
                    vertex_shader_constants[map_index * 8 + 4] = 0.0;
                    vertex_shader_constants[map_index * 8 + 5] = 1.0;
                    vertex_shader_constants[map_index * 8 + 6] = 0.0;
                    vertex_shader_constants[map_index * 8 + 7] = 0.0;
                }
            }

            auto res = device->SetVertexShaderConstantF(13, vertex_shader_constants, 8);
            if(res == D3D_OK) {
                FUN_0053ae90(reinterpret_cast<std::byte *>(transparent_geometry_group->shader_tag_data));
            }

            auto maps_count = shader_data->maps.count;
            if(transparent_geometry_group->field0_0x0 & 0x10 != 0 && shader_data->framebuffer_blend_function == FRAMEBUFFER_BLEND_FUNCTION_ALPHA_BLEND) {
                device->SetTexture(maps_count, nullptr);
                device->SetTextureStageState(maps_count, D3DTSS_COLOROP, D3DTOP_DISABLE);
                device->SetTextureStageState(maps_count, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                FUN_00536550(transparent_geometry_group, false);
                device->SetRenderState(D3DRS_BLENDOP, D3DTOP_DISABLE);
                return;
            }

            float vertex_shader_constants_2[3 * 4];
            vertex_shader_constants_2[0] = 0.0f;
            vertex_shader_constants_2[1] = 0.0f;
            vertex_shader_constants_2[2] = 0.0f;
            vertex_shader_constants_2[3] = 0.0f;
            vertex_shader_constants_2[4] = 0.0f;
            vertex_shader_constants_2[5] = 0.0f;
            vertex_shader_constants_2[6] = 0.0f;
            vertex_shader_constants_2[7] = 0.0f;
            vertex_shader_constants_2[8] = 0.0f;
            vertex_shader_constants_2[9] = 0.0f;
            vertex_shader_constants_2[10] = 1.0f;
            vertex_shader_constants_2[11] = 0.0f;

            auto framebuffer_fade_source = shader_data->framebuffer_fade_source;
            if(transparent_geometry_group->field10_0x14 == 1 && shader_data->extra_flags.dont_fade_active_camouflage == 0) {
                vertex_shader_constants_2[10] = 1.0f - transparent_geometry_group->field13_0x18;
                if(vertex_shader_constants_2[10] < 0.0f == std::isnan(vertex_shader_constants_2[10])) {
                    if(vertex_shader_constants_2[10] > 1.0f) {
                        vertex_shader_constants_2[10] = 1.0f;
                    }
                }
                else {
                    vertex_shader_constants_2[10] = 0.0f;
                }
            }

            if(framebuffer_fade_source > 0 && transparent_geometry_group->field90_0x74 != nullptr && transparent_geometry_group->field90_0x74[1] != 0) {
                auto unknown_val = *reinterpret_cast<float *>(transparent_geometry_group->field90_0x74[1] - 4 + framebuffer_fade_source * 4);
                if(std::isnan(unknown_val) != unknown_val == 0.0f && *reinterpret_cast<std::uint32_t *>(0x0075c4ec) < 0xFFFF0101) {
                    return;
                }
                vertex_shader_constants_2[10] = vertex_shader_constants_2[10] * unknown_val;
            }

            device->SetVertexShaderConstantF(10, vertex_shader_constants_2, 3);

            int tss_option_argument;
            if(framebuffer_fade_source == 0) {
                tss_option_argument = D3DTA_ALPHAREPLICATE;
            }
            else if(framebuffer_fade_source == 1) {
                tss_option_argument = D3DTA_ALPHAREPLICATE | D3DTA_SPECULAR;
            }
            else {
                tss_option_argument = D3DTA_SPECULAR;
            }

            auto *unknown_val = reinterpret_cast<std::uint32_t *>(0x0075c4b8);
            switch(shader_data->framebuffer_blend_function) {
                case FRAMEBUFFER_BLEND_FUNCTION_ALPHA_BLEND: {
                    if(*unknown_val == 2 && shader_data->maps.count > 1) {
                        map_index = shader_data->maps.count - 1;
                        if(map_index < 2) {
                            map_index = 1;
                        }
                        device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_TFACTOR);
                        device->SetTextureStageState(0, D3DTSS_ALPHAARG2, tss_option_argument);
                        break;
                    }
                    map_index = shader_data->maps.count;
                    device->SetTextureStageState(map_index, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                    device->SetTextureStageState(map_index, D3DTSS_COLORARG1, D3DTA_CURRENT);
                    device->SetTextureStageState(map_index, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                    device->SetTextureStageState(map_index, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
                    device->SetTextureStageState(map_index, D3DTSS_ALPHAARG2, tss_option_argument);
                    break;
                }

                case FRAMEBUFFER_BLEND_FUNCTION_MULTIPLY:
                case FRAMEBUFFER_BLEND_FUNCTION_COMPONENT_MIN: {
                    if(*unknown_val < 3) {
                        map_index = shader_data->maps.count - 1;
                        if(map_index < 2) {
                            map_index = 1;
                        }
                    }
                    else {
                        map_index = shader_data->maps.count;
                    }

                    device->SetTextureStageState(map_index, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD);
                    device->SetTextureStageState(map_index, D3DTSS_COLORARG1, tss_option_argument | D3DTOP_BLENDCURRENTALPHA);
                    device->SetTextureStageState(map_index, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    device->SetTextureStageState(map_index, D3DTSS_COLORARG0, tss_option_argument);
                    device->SetTextureStageState(map_index, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                    device->SetTextureStageState(map_index, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
                    break;
                }

                case FRAMEBUFFER_BLEND_FUNCTION_DOUBLE_MULTIPLY: {
                    if(*unknown_val < 3) {
                        map_index = shader_data->maps.count - 1;
                        if(map_index < 2) {
                            map_index = 1;
                        }
                    }
                    else {
                        map_index = shader_data->maps.count;
                    }

                    device->SetRenderState(D3DRS_TEXTUREFACTOR, 0x7f7f7f7f);
                    device->SetTextureStageState(map_index, D3DTSS_COLOROP, D3DTOP_LERP);
                    device->SetTextureStageState(map_index, D3DTSS_COLORARG1, tss_option_argument);
                    device->SetTextureStageState(map_index, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    device->SetTextureStageState(map_index, D3DTSS_COLORARG0, D3DTA_TFACTOR);
                    device->SetTextureStageState(map_index, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                    device->SetTextureStageState(map_index, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
                    break;
                }

                case FRAMEBUFFER_BLEND_FUNCTION_ADD:
                case FRAMEBUFFER_BLEND_FUNCTION_SUBTRACT:
                case FRAMEBUFFER_BLEND_FUNCTION_COMPONENT_MAX: {
                    if(*unknown_val != 2 || shader_data->maps.count < 2) {
                        map_index = shader_data->maps.count;
                        device->SetTextureStageState(map_index, D3DTSS_COLOROP, D3DTOP_MODULATE);
                        device->SetTextureStageState(map_index, D3DTSS_COLORARG1, D3DTA_CURRENT);
                        device->SetTextureStageState(map_index, D3DTSS_COLORARG2, tss_option_argument);
                        device->SetTextureStageState(map_index, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                        device->SetTextureStageState(map_index, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
                        break;
                    }

                    auto stage = shader_data->maps.count - 1;
                    if(stage < 2) {
                        stage = 1;
                    }
                    device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                    device->SetTextureStageState(0, D3DTSS_COLORARG2, tss_option_argument);
                    break;
                }

                case FRAMEBUFFER_BLEND_FUNCTION_ALPHA_MULTIPLY_ADD: {
                    if(*unknown_val == 2 && shader_data->maps.count > 1) {
                        map_index = shader_data->maps.count - 1;
                        if(map_index < 2) {
                            map_index = 1;
                        }
                        device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                        device->SetTextureStageState(0, D3DTSS_COLORARG2, tss_option_argument);
                        device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                    }
                    else {
                        map_index = shader_data->maps.count;
                        device->SetTextureStageState(map_index, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                        device->SetTextureStageState(map_index, D3DTSS_COLORARG1, D3DTA_CURRENT);
                        device->SetTextureStageState(map_index, D3DTSS_COLORARG2, tss_option_argument);
                        device->SetTextureStageState(map_index, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                        device->SetTextureStageState(map_index, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
                    }
                    device->SetTextureStageState(0, D3DTSS_ALPHAARG2, tss_option_argument);
                    break;
                }

                default: {
                    device->SetTexture(map_index, nullptr);
                    device->SetTextureStageState(map_index, D3DTSS_COLOROP, D3DTA_CURRENT);
                    device->SetTextureStageState(map_index, D3DTSS_ALPHAOP, D3DTA_CURRENT);
                    FUN_00536550(transparent_geometry_group, false);
                    device->SetRenderState(D3DRS_BLENDOP, D3DSHADE_FLAT);
                    return;
                }
            }

            map_index = map_index + 1;
            device->SetTexture(map_index, nullptr);
            device->SetTextureStageState(map_index, D3DTSS_COLOROP, D3DTA_CURRENT);
            device->SetTextureStageState(map_index, D3DTSS_ALPHAOP, D3DTA_CURRENT);
            FUN_00536550(transparent_geometry_group, false);
            device->SetRenderState(D3DRS_BLENDOP, D3DSHADE_FLAT);
            return;
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

        Event::D3D9EndSceneEvent::subscribe([](Event::D3D9EndSceneEvent &event) {
            if(!device) {
                logger.debug("D3D9EndSceneEvent: device set");
                device = event.args.device;
            }
        });
    }
}
