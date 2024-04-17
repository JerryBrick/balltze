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

    #pragma pack(push)
    #pragma pack(1)

    struct TransparentGeometryGroup {
        std::uint16_t flags;
        PADDING(0xA);
        ShaderTransparentChicago *shader_data;
        std::uint16_t model_effect_type;
        PADDING(0x78);
    };
    static_assert(sizeof(TransparentGeometryGroup) == 0x8A);

    #pragma pack(pop)

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
    
        void draw_shader_transparent_chicago(TransparentGeometryGroup *transparent_geometry_group, std::uint32_t *param_2) {
            if(!device) {
                return;
            }
            
            auto *shader_data = transparent_geometry_group->shader_data;
            short sVar5 = FUN_00543160(shader_data);
            short texture_stage = -1;
            if(*(short **)(transparent_geometry_group + 0x16) == nullptr) {
                if(*(int *)(transparent_geometry_group + 0x15) != -1) {
                    texture_stage = *(short *)(0x00674908 + *(int *)(transparent_geometry_group + 0x15) * 0x10);
                }
            }
            else {
                texture_stage = **(short **)(transparent_geometry_group + 0x16);
            }
            short local_f0 = *(short *)(transparent_geometry_group + 0x4);
            if(*(int *)((int)shader_data + 0x58) == 0) {
                return;
            }
            if(*(int *)(*(int *)((int)shader_data + 0x58) + 0x70) == 0) {
                return;
            }

            device->SetVertexShader((IDirect3DVertexShader9 *)(0x00639248 + (*(short *)(0x00639450 + ((int)sVar5 + texture_stage * 6) * 2) * 2)));
            device->SetVertexDeclaration((IDirect3DVertexDeclaration9 *)(0x0067ca50 + (texture_stage * 3)));
            device->SetPixelShader(nullptr);

            float vertex_shader_constants[38];
            
            texture_stage = 0;
            if (0 < *(int *)((int)shader_data + 0x48)) { // Untested block
                int iVar6 = 0;
                do {
                    int iVar4 = *(int *)((int)shader_data + 0x4c);
                    float *pfVar10 = transparent_geometry_group;
                    float *pfVar12 = vertex_shader_constants;
                    for(int iVar7 = 0x2a; iVar7 != 0; iVar7 = iVar7 + -1) {
                        *pfVar12 = *pfVar10;
                        pfVar10 = pfVar10 + 1;
                        pfVar12 = pfVar12 + 1;
                    }
                    int uStack20 = -1;
                    vertex_shader_constants[3] = *(float *)((*(unsigned int *)(iVar6 * 0x10 + 0xc + iVar4) & 0xffff) * 0x20 + 0x14 + *reinterpret_cast<std::uint32_t *>(0x00817144));
                    // FUN_00536740();
                    texture_stage = texture_stage + 1;
                    iVar6 = (int)texture_stage;
                } while (iVar6 < *(int *)((int)shader_data + 0x48));
            }

            device->SetRenderState(D3DRS_CULLMODE, ~(unsigned int)(*(byte *)((int)shader_data + 0x29) >> 1) & 2 | 1);
            device->SetRenderState(D3DRS_COLORWRITEENABLE, 7);
            device->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
            device->SetRenderState(D3DRS_ALPHATESTENABLE, *(byte *)((int)shader_data + 0x29) & 1);
            device->SetRenderState(D3DRS_ALPHAREF, 0x7f);
            device->SetRenderState(D3DRS_FOGENABLE, 0);

            FUN_0051be80(*reinterpret_cast<short *>(*reinterpret_cast<float **>(shader_data) + 0xB));

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
