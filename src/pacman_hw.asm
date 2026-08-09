; Pac-Man hardware shim for the Stellar Evolution simulator.
; These routines intentionally stay tiny: C owns gameplay, assembly owns
; memory-mapped input, tile clearing, text placement, and simple sound latches.

SECTION code_user

PUBLIC _hw_init
PUBLIC _hw_read_inputs
PUBLIC _hw_clear_screen
PUBLIC _hw_print
PUBLIC _hw_print_number
PUBLIC _hw_play_event

VIDEO_BASE  EQU $4000
COLOR_BASE  EQU $4400
IN0         EQU $5000
SOUND_LATCH EQU $5040

_hw_init:
    call _hw_clear_screen
    ret

_hw_read_inputs:
    ld a,(IN0)
    cpl
    and $1f
    ld l,a
    ld h,0
    ret

_hw_clear_screen:
    ld hl,VIDEO_BASE
    ld bc,$0400
.clear_loop:
    ld (hl),$20
    inc hl
    dec bc
    ld a,b
    or c
    jr nz,.clear_loop
    ret

; z88dk callee stack: x, y, text pointer are read from the caller stack.
_hw_print:
    pop ix
    pop hl              ; x in L
    pop de              ; y in E
    pop bc              ; string pointer
    push bc
    push de
    push hl
    push ix
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    add a,a             ; y * 32
    add a,l
    ld l,a
    ld h,0
    ld de,VIDEO_BASE
    add hl,de
.text_loop:
    ld a,(bc)
    or a
    ret z
    ld (hl),a
    inc hl
    inc bc
    jr .text_loop

_hw_print_number:
    ; Minimal binary-visible placeholder for cabinet bring-up. The C game still
    ; keeps the authoritative value; production ROMs can replace this with BCD.
    pop ix
    pop hl
    pop de
    pop bc
    push bc
    push de
    push hl
    push ix
    ld a,e
    add a,a
    add a,a
    add a,a
    add a,a
    add a,a
    add a,l
    ld l,a
    ld h,0
    ld de,VIDEO_BASE
    add hl,de
    ld (hl),'#'
    ret

_hw_play_event:
    pop ix
    pop hl
    push hl
    push ix
    ld a,l
    ld (SOUND_LATCH),a
    ret
