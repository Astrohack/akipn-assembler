.MODEL  SMALL, C
.386p

public first
public second

.data
    result dd ?       ; Wynik

.CODE
first:
    push    bp
    mov     bp, sp
    
    mov     si, [bp + 4]
    mov     cx, [bp + 6]
    fldz
    call    sum

    fild     word ptr [bp + 6]
    fdiv

    pop     bp
    ret    

    sum:
        fld     DWORD PTR [si]
        fadd
        add     si, 4
        loop    sum
        ret
    
second:
    push    bp

    arg     pstring: word, character: byte
    mov     bp, sp
    
    xor     ax, ax
    xor     bx, bx
    xor     cx, cx
    xor     dx, dx

    mov     dl, character
    mov     si, pstring

    call    count    
    
    pop     bp
    ret

    count:
        mov     ax, '$'
        cmp     [si], ax
        je      return
        
        cmp     [si], dl
        jne     skip_character
        inc     bx;
        skip_character:
        inc     si

        loop    count

        return:
        mov     ax, bx
        ret


.STACK 1024h


END