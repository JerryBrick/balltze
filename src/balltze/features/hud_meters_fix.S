;# SPDX-License-Identifier: GPL-3.0-only

.intel_syntax noprefix

.text

.globl _hud_meters_draw_hook
_hud_meters_draw_hook:
    push eax
    mov eax, dword ptr [esp + 0x8]
    mov dword ptr [eax], 0
    
    mov eax, dword ptr [esp + 0x18]
    mov dword ptr [eax], 0xFFFFFFFF

    pop eax

    ret
