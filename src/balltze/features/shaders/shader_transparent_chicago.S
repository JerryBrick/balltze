;# SPDX-License-Identifier: GPL-3.0-only

.intel_syntax noprefix

.data
FUN_00543160: .long 0x00543160
FUN_0051be80: .long 0x0051be80
FUN_005c8b40: .long 0x005c8b40
FUN_0051c060: .long 0x0051c060

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

