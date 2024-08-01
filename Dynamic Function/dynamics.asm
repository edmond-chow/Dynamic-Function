 ;
 ;   Copyright 2022 Edmond Chow
 ;
 ;   Licensed under the Apache License, Version 2.0 (the "License");
 ;   you may not use this file except in compliance with the License.
 ;   You may obtain a copy of the License at
 ;
 ;       http://www.apache.org/licenses/LICENSE-2.0
 ;
 ;   Unless required by applicable law or agreed to in writing, software
 ;   distributed under the License is distributed on an "AS IS" BASIS,
 ;   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ;   See the License for the specific language governing permissions and
 ;   limitations under the License.
 ;
ifdef Win32
.model flat
option casemap:none
.code
_captured PROTO
_capture_registers PROC
    push eax
    push edx
    push ecx
    call _captured
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
_capture_registers ENDP
endif
ifdef x64
option casemap:none
.code
captured PROTO
capture_registers PROC
    push rax
    push rdx
    push rcx
    push r8
    push r9
    push r10
    push r11
    call captured
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
capture_registers ENDP
endif
END
