;# SPDX-License-Identifier: GPL-3.0-only

.intel_syntax noprefix

.data
create_widget_function_address: .long 0
open_widget_function_address: .long 0
close_widget_function_address: .long 0
find_widget_function_address: .long 0
focus_widget_function_address: .long 0
get_widget_list_item_index_function_address: .long 0
free_widget_memory_function_address: .long 0
draw_hud_bitmap_function: .long 0
get_button_name_function: .long 0
play_sound_function_address: .long 0
draw_bitmap_in_react_function_address: .long 0

.text
create_widget_function_sig: .string "widget_create_function"
open_widget_function_sig: .string "widget_open_function"
close_widget_function_sig: .string "widget_back_function"
find_widget_function_sig: .string "widget_find_function"
focus_widget_function_sig: .string "widget_focus_function"
get_widget_list_item_index_function_sig: .string "widget_list_item_index_function"
free_widget_memory_function_sig: .string "widget_memory_release_function"
draw_hud_bitmap_function_sig: .string "draw_hud_bitmap_function"
get_button_name_function_sig: .string "get_button_name_function"
play_sound_function_sig: .string "play_sound_function"
draw_bitmap_in_react_function_sig: .string "draw_bitmap_in_rect_funcion"

.globl _create_widget_asm
_create_widget_asm:
    ;# Get open widget function address
    cmp dword ptr create_widget_function_address, 0
    jnz do_create_widget
    lea eax, byte ptr create_widget_function_sig
    push eax
    call _get_address_for_signature
    add esp, 4
    mov create_widget_function_address, eax

    do_create_widget:
    mov eax, [esp + 4] 
    mov ecx, [esp + 8] 

    ;# Null values
    push -1
    push -1
    push -1
    push 0

    ;# Widget instance to replace
    push ecx

    ;# Widget tag handle
    push eax

    ;# Widget tag path (null because we are using the tag handle parameter)
    push 0

    call dword ptr create_widget_function_address
    add esp, 28

    ret

.globl _open_widget_asm
_open_widget_asm:
    cmp dword ptr open_widget_function_address, 0
    jnz do_open_widget
    lea eax, byte ptr open_widget_function_sig
    push eax
    call _get_address_for_signature
    add esp, 4
    mov open_widget_function_address, eax

    do_open_widget:
    mov eax, [esp + 4] ;# Current widget root instance
    mov edx, [esp + 8] ;# New widget tag handle
    push edx
    push eax
    call dword ptr open_widget_function_address
    add esp, 8

    ret

.globl _close_widget_asm
_close_widget_asm:
    cmp dword ptr close_widget_function_address, 0
    jnz do_back_to_previous_widget
    lea eax, byte ptr close_widget_function_sig
    push eax
    call _get_address_for_signature
    add esp, 4
    mov close_widget_function_address, eax

    do_back_to_previous_widget:
    mov eax, [esp + 4] ;# Current root widget

    call dword ptr close_widget_function_address

    ret

.globl _find_widget_asm
_find_widget_asm:
    cmp dword ptr find_widget_function_address, 0
    jnz do_find_widget
    lea eax, byte ptr find_widget_function_sig
    push eax
    call _get_address_for_signature
    add esp, 4
    mov find_widget_function_address, eax

    do_find_widget:
    mov eax, [esp + 4] ;# Widget where to search
    mov ecx, [esp + 8] ;# Tag handle of the widget definition

    push ecx
    push eax
    call dword ptr find_widget_function_address
    add esp, 8

    ret

.globl _focus_widget_asm
_focus_widget_asm:
    cmp dword ptr focus_widget_function_address, 0
    jnz do_focus_widget
    lea eax, byte ptr focus_widget_function_sig
    push eax
    call _get_address_for_signature
    add esp, 4
    mov focus_widget_function_address, eax

    do_focus_widget:
    ;# Idk why, but the function reads it's arguments from EAX and ECX registers.
    mov eax, [esp + 4] ;# Root widget
    mov ecx, [esp + 8] ;# Widget to focus

    call dword ptr focus_widget_function_address

    ret

.globl _get_widget_list_item_index_asm
_get_widget_list_item_index_asm:
    cmp dword ptr get_widget_list_item_index_function_address, 0
    jnz do_get_widget_list_item_index
    lea eax, byte ptr get_widget_list_item_index_function_sig
    push eax
    call _get_address_for_signature
    add esp, 4
    mov get_widget_list_item_index_function_address, eax

    do_get_widget_list_item_index:
    push esi
    mov esi, [esp + 8] ;# Widget 
    call dword ptr get_widget_list_item_index_function_address
    pop esi

    ret

.globl _free_widget_asm
_free_widget_asm:
    cmp dword ptr free_widget_memory_function_address, 0
    jnz do_free_widget
    lea eax, byte ptr free_widget_memory_function_sig
    push eax
    call _get_address_for_signature
    add esp, 4
    mov free_widget_memory_function_address, eax

    do_free_widget:
    push [esp + 4] ;# Widget 
    call dword ptr free_widget_memory_function_address
    add esp, 4

    ret

.globl _hud_draw_bitmap_sprite
_hud_draw_bitmap_sprite:
    pushad
    pushfd

    cmp dword ptr draw_hud_bitmap_function, 0
    jnz hud_draw_bitmap_sprite_function_found
    lea eax, draw_hud_bitmap_function_sig
    push eax
    call _get_address_for_signature
    mov draw_hud_bitmap_function, eax
    add esp, 4
    hud_draw_bitmap_sprite_function_found:

    ;# Get arguments
    lea eax, [esp + 0x28] ;# position
    mov ebx, [esp + 0x2C] ;# color
    mov ecx, [esp + 0x30] ;# bitmap_data
    mov edx, [esp + 0x34] ;# sprite

    push ebx ;# color
    push 0
    push 0x3F800000
    push eax ;# position
    push 2

    lea eax, [edx + 0x8]
    mov edi, eax ;# position
    mov edx, ecx ;# bitmap_data
    mov ebp, ecx ;# bitmap_data
    mov ecx, 0

    call dword ptr draw_hud_bitmap_function
    add esp, 0x14

    popfd
    popad

    ret

.globl _get_string_name_for_button
_get_string_name_for_button:
    pushad
    pushfd
    cmp dword ptr get_button_name_function, 0
    jnz get_string_name_for_button_function_found
    lea eax, get_button_name_function_sig
    push eax
    call _get_address_for_signature
    mov get_button_name_function, eax
    add esp, 4
    get_string_name_for_button_function_found:
    popfd
    popad

    mov ecx, [esp + 0xC]
    mov eax, [esp + 0x4]
    mov edx, [esp + 0x8]
    shl edx, 0x10
    mov dword ptr [esp + 0x8], edx

    push edx
    push eax
    lea eax, [esp]
    call dword ptr get_button_name_function
    add esp, 0x8
    ret

.globl _play_sound_asm
_play_sound_asm:
    pushad
    pushfd

    cmp dword ptr play_sound_function_address, 0
    jnz found
    lea eax, play_sound_function_sig
    push eax
    call _get_address_for_signature
    mov play_sound_function_address, eax
    add esp, 4
    found:

    ;# Get sound tag handle
    mov eax, [esp + 0x28]

    ;# Look for a valid tag id
    cmp eax, -1
    je return

    ;# Play it
    sub esp, 0xC
    push 0
    push 0
    push 0
    push 0
    push -1
    lea ecx, dword ptr [esp + 0x14]
    push ecx
    push eax
    mov word ptr [esp + 0x1C], 0
    mov dword ptr [esp + 0x20], 0x3F800000
    mov dword ptr [esp + 0x24], 0x3F800000
    call dword ptr play_sound_function_address
    add esp, 0x28

    return:
    popfd
    popad
    ret

.globl _draw_bitmap_in_rect_asm
_draw_bitmap_in_rect_asm:
    cmp dword ptr draw_bitmap_in_react_function_address, 0
    jnz found_draw_bitmap_in_react_function_sig
    lea eax, draw_bitmap_in_react_function_sig
    push eax
    call _get_address_for_signature
    mov draw_bitmap_in_react_function_address, eax
    add esp, 4
    found_draw_bitmap_in_react_function_sig:

    pushad
    pushfd

    mov eax, [esp + 0x28] ;# bitmap data
    mov ebx, [esp + 0x2C] ;# rect
    mov ecx, [esp + 0x30] ;# bounds
    mov edx, [esp + 0x34] ;# frame
    mov esi, [esp + 0x38] ;# color mask

    push esi
    push edx 
    push eax
    mov eax, ecx
    mov ecx, ebx
    call dword ptr draw_bitmap_in_react_function_address
    add esp, 0x0C

    popfd
    popad

    ret
