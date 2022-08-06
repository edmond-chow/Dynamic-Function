ifdef Win32
.model flat
option casemap:none
.code
_cr_ptr PROTO
?capture@dyn@@YGXXZ PROC
    jmp ?capture@dyn@@YAXXZ
?capture@dyn@@YGXXZ ENDP
?capture@dyn@@YIXXZ PROC
    jmp ?capture@dyn@@YAXXZ
?capture@dyn@@YIXXZ ENDP
?capture@dyn@@YQXXZ PROC
    jmp ?capture@dyn@@YAXXZ
?capture@dyn@@YQXXZ ENDP
?capture@dyn@@YAXXZ PROC
    push eax
    push edx
    push ecx
    call _cr_ptr
    mov [eax+04h], ebx
    mov [eax+10h], esi
    mov [eax+14h], edi
    mov [eax+20h], ebp
    pop ecx
    mov [eax+08h], ecx
    pop edx
    mov [eax+0Ch], edx
    pop edx
    mov [eax], edx
    pop [eax+18h]
    mov [eax+1Ch], esp
    push [eax+18h]
    push edx
    mov edx, [eax+0Ch]
    pop eax
    ret
?capture@dyn@@YAXXZ ENDP
endif
ifdef x64
option casemap:none
.code
cr_ptr PROTO
?capture@dyn@@YGXXZ PROC
    jmp ?capture@dyn@@YAXXZ
?capture@dyn@@YGXXZ ENDP
?capture@dyn@@YIXXZ PROC
    jmp ?capture@dyn@@YAXXZ
?capture@dyn@@YIXXZ ENDP
?capture@dyn@@YQXXZ PROC
    jmp ?capture@dyn@@YAXXZ
?capture@dyn@@YQXXZ ENDP
?capture@dyn@@YAXXZ PROC
    push rax
    push rdx
    push rcx
    push r8
    push r9
    push r10
    push r11
    call cr_ptr
    mov [rax+08h], rbx
    mov [rax+20h], rsi
    mov [rax+28h], rdi
    mov [rax+40h], rbp
    mov [rax+68h], r12
    mov [rax+70h], r13
    mov [rax+78h], r14
    mov [rax+80h], r15
    pop r11
    mov [rax+60h], r11
    pop r10
    mov [rax+58h], r10
    pop r9
    mov [rax+50h], r9
    pop r8
    mov [rax+48h], r8
    pop rcx
    mov [rax+10h], rcx
    pop rdx
    mov [rax+18h], rdx
    pop rdx
    mov [rax], rdx
    pop [rax+30h]
    mov [rax+38h], rsp
    push [rax+30h]
    push rdx
    mov rdx, [rax+18h]
    pop rax
    ret
?capture@dyn@@YAXXZ ENDP
endif
END
