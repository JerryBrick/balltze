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
        float field90_0x74;
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
        bool FUN_0051c060(short texture_stage, short bitmap_data_type, short bitmap_data_index, short unknown);
        void FUN_00543250 (void *texture_animation, float param_1, float param_2, float param_3, float param_4, float param_5, float param_6);

        void rasterizer_transparent_geometry_group_draw(TransparentGeometryGroup *group, std::uint32_t *param_2);
    
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

            if(flags < 0 && transparent_geometry_group->field90_0x74 != 0.0f && shader_data->maps.count > 0) {
                auto *bitmap_tag = Engine::get_tag(shader_data->maps.offset[0].map.tag_handle);
                if(bitmap_tag) {
                    auto *bitmap_data = reinterpret_cast<Bitmap *>(bitmap_tag->data);
                    auto bitmap_count = bitmap_data->bitmap_data.count;
                    auto extra_flags = *reinterpret_cast<std::uint32_t *>(&shader_data->extra_flags);
                    if(extra_flags & 2 == 0) {
                        float numeric_count_limit = shader_data->numeric_counter_limit;
                    }
                }
            }

            // if ((*(byte *)((int)fVar2 + 0x60) & 2) == 0)
            //     {
            //         iVar4 = (int)(short)(ushort) * (byte *)((int)fVar2 + 0x28);
            //         fVar11 = (float10)FUN_005c8b40();
            //         fStack244 = (float)fVar11;
            //         if ((int)ROUND(fStack244) < 0)
            //         {
            //             iVar4 = 0;
            //         }
            //         else
            //         {
            //             fVar11 = (float10)FUN_005c8b40();
            //             fStack244 = (float)fVar11;
            //             if ((int)ROUND(fStack244) <= iVar4)
            //             {
            //                 fVar11 = (float10)FUN_005c8b40();
            //                 fStack244 = (float)fVar11;
            //                 iVar4 = (int)ROUND(fStack244);
            //             }
            //         }
            //         local_f0 = (short)iVar4;
            //         if (0 < (short)*(ushort *)(param_1 + 4))
            //         {
            //             uVar8 = (uint) * (ushort *)(param_1 + 4);
            //             do
            //             {
            //                 iVar4 = (int)(short)iVar4 / (int)stage;
            //                 local_f0 = (short)iVar4;
            //                 uVar8 = uVar8 - 1;
            //             } while (uVar8 != 0);
            //         }
            //         local_f0 = local_f0 % stage;
            //     }
            //     else
            //     {
            //         local_f0 = FUN_005434c0();
            //     }
            // }

            texture_stage = 0;
            do {
                short uVar8 = texture_stage;
                auto sampler_stage_index = (int)texture_stage;
                if ((int)sampler_stage_index < *(int *)((int)shader_data + 0x54)) {
                    auto uVar2 = *(unsigned short *)((int)shader_data + 0x2a);
                    auto fStack244 = (float)(unsigned int)uVar2;
                    auto *pbVar10 = (std::uint8_t *)(uVar8 * 0xdc + *(int *)((int)shader_data + 0x58));
                    short bitmap_type;
                    if (texture_stage == 0) {
                        bitmap_type = bitmap_type & 0xffff0000 | (unsigned int) * (unsigned short *)(0x005fc9d0 + (short)uVar2 * 2);
                    }
                    else {
                        bitmap_type = 0;
                    }
                    FUN_0051c060(texture_stage, bitmap_type, 0, local_f0);

                    unsigned int Value;
                    if ((bitmap_type == 0) && ((*pbVar10 & 4) != 0)) {
                        uVar8 = 3;
                        
                        if ((*pbVar10 & 8) == 0) {
                            if (texture_stage == 0) {
                                Value = *(unsigned int *)(0x005fc9d8 + (short)uVar2 * 4);
                            }
                            else {
                                Value = 1;
                            }
                        }
                        else {
                            Value = 3;
                        }
                    }
                    else {
                        if (texture_stage == 0) {
                            uVar8 = *(unsigned int *)(0x005fc9d8 + (short)uVar2 * 4);
                        }
                        else {
                            uVar8 = 1;
                        }
                        
                        if (bitmap_type == 0) {
                            if ((*pbVar10 & 8) == 0) {
                                if (texture_stage == 0) {
                                    Value = *(unsigned int *)(0x005fc9d8 + (short)uVar2 * 4);
                                }
                                else {
                                    Value = 1;
                                }
                            }
                            else {
                                Value = 3;
                            }
                        }
                        else {
                            if (texture_stage == 0) {
                                Value = *(unsigned int *)(0x005fc9d8 + (short)uVar2 * 4);
                            }
                            else {
                                Value = 1;
                            }
                        }
                    }
                      
                    device->SetSamplerState(sampler_stage_index, D3DSAMP_ADDRESSU, uVar8);
                    device->SetSamplerState(sampler_stage_index, D3DSAMP_ADDRESSV, Value);
                    if (texture_stage == 0) {
                        uVar8 = *(unsigned int *)(0x005fc9d8 + (short)uVar2 * 4);
                    }
                    else {
                        uVar8 = 1;
                    }
                     
                    device->SetSamplerState(sampler_stage_index, D3DSAMP_ADDRESSW, uVar8);
                    device->SetSamplerState(sampler_stage_index, D3DSAMP_MAGFILTER, 2);
                    device->SetSamplerState(sampler_stage_index, D3DSAMP_MINFILTER, 2 - (((*pbVar10 & 1) != 0) ? 1 : 0));
                    device->SetSamplerState(sampler_stage_index, D3DSAMP_MIPFILTER, 2 - (((*pbVar10 & 1) != 0) ? 1 : 0));
                }
                  
                if (sampler_stage_index < *(int *)((int)shader_data + 0x54)) {
                    if ((texture_stage < 1) && (*(short *)((int)shader_data + 0x2a) != 0) && !((*(int *)((int)shader_data + 0x54) <= (int)sampler_stage_index) || ((*(byte *)((int)shader_data + 0x29) & 8) == 0))) {
                        vertex_shader_constants[sampler_stage_index * 8] = *reinterpret_cast<float *>(0x0075c624);
                        vertex_shader_constants[sampler_stage_index * 8 + 1] = *reinterpret_cast<float *>(0x0075c628);
                        vertex_shader_constants[sampler_stage_index * 8 + 2] = *reinterpret_cast<float *>(0x0075c62c);
                        vertex_shader_constants[sampler_stage_index * 8 + 3] = 0.0f;
                        vertex_shader_constants[sampler_stage_index * 8 + 4] = *reinterpret_cast<float *>(0x0075c630);
                        vertex_shader_constants[sampler_stage_index * 8 + 5] = *reinterpret_cast<float *>(0x0075c634);
                        vertex_shader_constants[sampler_stage_index * 8 + 6] = *reinterpret_cast<float *>(0x0075c638);
                        vertex_shader_constants[sampler_stage_index * 8 + 7] = 0.0f;
                    }

                    float fStack228 = *(float *)(sampler_stage_index * 0xdc + 0x58 + *(int *)((int)shader_data + 0x58));
                    int iVar6 = sampler_stage_index * 0xdc + *(int *)((int)shader_data + 0x58);
                    float fStack232 = *(float *)(iVar6 + 0x54);
                    if (texture_stage == 0) {
                        if ((*(byte *)((int)shader_data + 0x29) & 0x40) != 0) {
                            fStack232 = -(fStack232 * transparent_geometry_group[0x1e]);
                            fStack228 = -(fStack228 * transparent_geometry_group[0x1e]);
                        }
                     
                        if ((*(byte *)((int)shader_data + 0x29) & 8) == 0) {
                            fStack232 = fStack232 * transparent_geometry_group[0xf];
                            fStack228 = fStack228 * transparent_geometry_group[0x10];
                        }
                    }
                    else if(texture_stage >= 1 || (*(byte *)((int)shader_data + 0x29) & 8) == 0) {
                        fStack232 = fStack232 * transparent_geometry_group[0xf];
                        fStack228 = fStack228 * transparent_geometry_group[0x10];
                    }

                    FUN_00543250(fStack232, fStack228, *(undefined4 *)(iVar6 + 0x5c), *(undefined4 *)(iVar6 + 0x60));
                }
            //       else
            //       {
            //           afStack172[sampler_stage_index * 8] = 1.0;
            //           afStack172[sampler_stage_index * 8 + 1] = 0.0;
            //           afStack172[sampler_stage_index * 8 + 2] = 0.0;
            //           afStack172[sampler_stage_index * 8 + 3] = 0.0;
            //           afStack172[sampler_stage_index * 8 + 4] = 0.0;
            //           afStack172[sampler_stage_index * 8 + 5] = 1.0;
            //           afStack172[sampler_stage_index * 8 + 6] = 0.0;
            //           afStack172[sampler_stage_index * 8 + 7] = 0.0;
            //       }
                texture_stage = texture_stage + 1;
            } while (texture_stage < 4);
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
