;# SPDX-License-Identifier: GPL-3.0-only

.intel_syntax noprefix

.data
FUN_00543160: .long 0x00543160
FUN_0051be80: .long 0x0051be80
FUN_005c8b40: .long 0x005c8b40
FUN_0051c060: .long 0x0051c060
FUN_00536740: .long 0x00536740
FUN_005434c0: .long 0x005434c0
FUN_00543250: .long 0x00543250
FUN_0053ae90: .long 0x0053ae90

.text

.globl _draw_shader_transparent_chicago_hook
_draw_shader_transparent_chicago_hook:
    cmp byte ptr _use_shader_transparent_chicago_reimplementation, 0
    jz skip
    push eax
    push ebx
    call _draw_shader_transparent_chicago
    add esp, 8
    mov eax, 1
    ret
    skip:
    mov eax, 0
    ret
.globl _FUN_00543160
_FUN_00543160:
    mov ecx, [esp + 4]
    call dword ptr FUN_00543160
    ret

.globl _FUN_0051be80
_FUN_0051be80:
    mov ecx, [esp + 4]
    call dword ptr FUN_0051be80
    ret

.globl _FUN_005c8b40
_FUN_005c8b40:
    fstp qword ptr [esp + 4]
    call dword ptr FUN_005c8b40
    add esp, 8
    ret

.globl _FUN_0051c060
_FUN_0051c060:
    push [esp + 0x10]
    push [esp + 0x10]
    push [esp + 0x10]
    push [esp + 0x10]
    call dword ptr FUN_0051c060
    add esp, 0x10
    ret

.globl _rasterizer_transparent_geometry_group_draw
_rasterizer_transparent_geometry_group_draw:
    push [esp + 0x8]
    push [esp + 0x8]
    call dword ptr FUN_00536740
    add esp, 8
    ret

.globl _FUN_005434c0
_FUN_005434c0:
    mov ecx, [esp + 4]
    call dword ptr FUN_005434c0
    ret

.globl _FUN_00543250
_FUN_00543250:
    pushad
    pushfd
    add ebp, 4
    mov edi, [ebp + 0x28]
    mov ebx, [ebp + 0x24]
    mov ecx, [ebp + 0x20]
    push [ebp + 0x1C]
    push [ebp + 0x18]
    push [ebp + 0x14]
    push [ebp + 0x10]
    push [ebp + 0xC]
    push [ebp + 0x8]
    mov esi, [ebp + 0x4]
    call dword ptr FUN_00543250
    add esp, 0x18
    popfd
    popad
    ret

.globl _FUN_0053ae90
_FUN_0053ae90:
    mov ebx, [esp + 4]
    call dword ptr FUN_0053ae90
    ret
