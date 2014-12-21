
section .text

global main
extern op_push
extern new_var
extern op_capi
extern pop_val
extern init
extern op_llt
extern op_slv
extern op_slv
extern op_llv
extern op_llv
extern op_add
extern op_sub
extern op_mul
extern op_call

main:
	push 	rbp
	mov 	rbp , rsp
	call 	init

l0:
	JMP l15
l1:
	MOV edi , 0x1
	call op_llt
l2:
	MOV edi , 0x0
	CALL op_slv
l3:
	MOV edi , 0x0
	CALL op_llv
l4:
	CALL pop_val
	cmp rax , 0x0
	je exit
l5:
	MOV edi , 0x0
	CALL op_llv
l6:
	MOV edi , 0x0
	CALL op_llv
l7:
	mov edi , 0x1
	mov rsi , 0x1
	call new_var
	mov edi , DWORD [rax]
	mov rsi , QWORD [rax + 0x8]
	call op_push
l8:
	CALL op_sub
l9:
	CALL l1
l10:
	CALL op_mul
l11:
	RET
l12:
	mov edi , 0x1
	mov rsi , 0x1
	call new_var
	mov edi , DWORD [rax]
	mov rsi , QWORD [rax + 0x8]
	call op_push
l13:
	RET
l14:
	RET
l15:
	mov edi , 0x1
	mov rsi , 0x6
	call new_var
	mov edi , DWORD [rax]
	mov rsi , QWORD [rax + 0x8]
	call op_push
l16:
	CALL l1
l17:
	MOV edi , 0x1
	call op_capi
l18:
	JMP exit
	
exit:
    mov 	eax , 1
    mov 	ebx , 0
    int 	0x80

