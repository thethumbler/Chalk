
section .data
	msg db "Hi" , 0xA , 0x0

section .text

global main
extern op_push
extern new_var
extern vm_set_call
extern vm_op_capi
extern pop_val
extern init_runtime
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
	call 	init_runtime

	mov edi , 0x3
	mov rsi , msg
	call new_var
	mov edi , DWORD [rax]
	mov rsi , QWORD [rax + 0x8]
	call op_push

	MOV edi , 0x1
	call vm_set_call
	call vm_op_capi


    mov 	eax , 1
    mov 	ebx , 0
    int 	0x80

