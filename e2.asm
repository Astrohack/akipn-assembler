; Patryk Krawczyk 251563 
                .386p
                .MODEL  tiny                ; TINY for COM exec

; ------------ code segment
Kod     SEGMENT USE16
                ORG    100h                 ; reserve space for PSP
                ASSUME  DS:Kod, CS:Kod      ; assign program segments

start:
    
    mov     di, OFFSET values
    call    get_value
    call    string_to_u2
    call    store_value

    mov     di, OFFSET values + 4
    call    get_value
    call    string_to_u2
    call    store_value

    call    print_newline

    call    sum
    mov     bx, 10
    call    print_u2

    call    exit

print_minus:
    push    ax
    mov     dx, 2Dh
    mov     ah, 02h
    int     21h
    pop     ax
    ret
sum:
    mov     eax, values
    mov     ebx, values + 4
    add     eax, ebx
    ret
store_value:
    mov     [di], eax
    ret
print_newline: 
    push    ax
    MOV     dl, 10
    MOV     ah, 02h
    INT     21h
    MOV     dl, 13
    MOV     ah, 02h
    INT     21h
    pop     ax
    ret
string_to_u2:                   ; EAX: result value 
    mov     cl, [buffer + 1]        ; set counter to string len
    mov     si, OFFSET buffer + 2; set counter to string len

    xor     bx, bx
    xor     eax, eax

    cmp     [buffer + 2], 2Dh       ; is negative
    jne     convert_even                 

    convert_odd:
        inc     si
        dec     cx
        call    convert_even        ; convert like it was even 
        neg     eax                 ; then negate
        ret

    convert_even:
        mov     bx, 000Ah
        mul     bx

        mov     dl, [si]
        sub     dl, 30h

        add     eax, edx

        inc     si
        loop    convert_even
        ret
get_value:                      ; Returns buffer at DX:
    mov     ah, 0Ah
    mov     dx, OFFSET buffer
    int     21h
    call    print_newline
    ret
print_u2:                       ; AX: value to print
    push    eax
    mov     cx, 0               ; reset loop counter
    ;mov     bx, 10              ; set static conversion divisor, 10h - hex, 10 - dec, 2 - bin, etc...
    test    eax, eax
    jns     is_positive 
    neg     eax
    call    print_minus
    
    is_positive:
    jmp     push_values

    print:
        mov     ah, 02h
        pop     dx

        call    number_to_char

        int     21h
        loop    print               ; print charcters
        pop     eax                 ; retrieve original value
        call    print_newline
        ret

        number_to_char:
            add     dx, 30h
            cmp     dx, 3Ah
            ja      letters
            ret

            letters:                ; A - F
                add     dx, 07h
                ret

    push_values:                 ; AX:  put numbers from value on
        mov     dx, 0            ;      stack for further conversion
        div     bx

        push    dx               ; put reminder on stack

        inc     cx               ; increase counter of values on stack 

        cmp     ax, 0            ; repeat until dividet value becomes 0
        jne     push_values      ; NO ? 
        jmp     print            ; YES ?

exit:                        ; gracefuly exit program
    mov     ax, 4C00h
    int     21h

buffer  DB 100, ?, 10 DUP(0)
values  DD ?, ?

Kod     ENDS

END	start






