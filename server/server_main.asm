global _start

%define ERROR -1

%define AF_INET 2
%define SOCK_STREAM 1
%define ADDR_SIZE 16
%define PORT 0x901f
%define INADDR_ANY 0
%define MAX_CONNS 5
%define REQUEST_LEN 1024
%define PATH_LEN 124

%macro exit 1
	mov rax, 60
	mov rdi, %1
	syscall
%endmacro

%macro print 2
	mov rax, 1
	mov rdi, 1
	mov rsi, %1
	mov rdx, %2
	syscall
%endmacro

%macro sys_socket 3
	mov rax, 41
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
	syscall
%endmacro

%macro bind 3
	mov rax, 49
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
	syscall
%endmacro

%macro listen 2
	mov rax, 50
	mov rdi, %1
	mov rsi, %2
	syscall
%endmacro

%macro accept 1
	mov rax, 43
	mov rdi, %1
	mov rsi, 0
	mov rdx, 0
	syscall
%endmacro

%macro close 1
	mov rax, 3
	mov rdi, %1
	syscall
%endmacro

%macro write 3
	mov rax, 1
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
	syscall
%endmacro

%macro open 2
	mov rax, 2
	mov rdi, %1
	xor rsi, rsi
	mov rdx, %2
	syscall
%endmacro

%macro stat 2
	mov rax, 4
	mov rdi, %1
	mov rsi, %2
	syscall
%endmacro

%macro send_file 3
	mov rax, 40
	mov rdi, %1
	mov rsi, %2
	mov rdx, 0
	mov r10, %3
	syscall
%endmacro

%macro recv 3
	mov rax, 45
	mov rdi, %1
	mov rsi, %2
	mov rdx, %3
	mov r10, 0
	mov r8, 0
	mov r9, 0
	syscall
%endmacro

%macro print_newline 0
	print newline, 1
%endmacro

section .data
	newline db 10

	Success db "[!] Success!", 10, 0
	SuccessLen equ $ - Success

	SocketExceptionMsg db "[X] Socket could not be created!", 10, 0
	SocketExceptionMsgLen equ $ - SocketExceptionMsg

	BindExceptionMsg db "[X] Binding failed!", 10, 0
	BindExceptionMsgLen equ $ - BindExceptionMsg

	ListenExceptionMsg db "[X] Listening failed!", 10, 0
	ListenExceptionMsgLen equ $ - ListenExceptionMsg

	AcceptExceptionMsg db "[X] Accepting failed!", 10, 0
	AcceptExceptionMsgLen equ $ - AcceptExceptionMsg

	OpenExceptionMsg db "[X] Opening file failed!", 10, 0
	OpenExceptionMsgLen equ $ - OpenExceptionMsg

	SendFileExceptionMsg db "[X] Sending file failed!", 10, 0
	SendFileExceptionMsgLen equ $ - SendFileExceptionMsg

	address dw AF_INET
			dw 0x901f
			dd 0
			dq 0

	socketfd db 0
	clientfd db 0

	response db "HTTP/1.1 200 OK", 13, 10
			 db "Content-Type: text/html; charset=utf-8", 10
			 db "Connection: keep-alive", 13, 10
			 db  13, 10
;			 db "<head><title>hallo</title></head><h1> omh </h1>", 10
	response_len equ $ - response
	index_file_name db "index.html", 0
	open_error_file db "open_error.html", 0
	file_path_indicator db "ET /", 0
	file_request: times REQUEST_LEN db 0
	requested_file_path: times PATH_LEN db 0
section .bss
section .text
_start:
	sys_socket AF_INET, SOCK_STREAM, 0
	mov r12, rax
	cmp r12, ERROR
	jle SocketException

	bind r12, address, ADDR_SIZE
	cmp rax, ERROR
	jle BindException

	listen r12, MAX_CONNS
	cmp rax, ERROR
	jle ListenException

accept_loop:
	accept r12
	mov r13, rax
	cmp r13, ERROR
	jle AcceptException

	recv r13, file_request, REQUEST_LEN
	cmp rax, ERROR
	jle SendFileException

	print file_request, REQUEST_LEN

	call extract_file_name

	mov al, 0
	lea rdi, [requested_file_path]
	mov rcx, 31
	rep stosq

	mov rbx, rsi
	mov rsi, file_request
	add rsi, rbx
	mov rdi, requested_file_path
.move_path:
	cmp byte [rsi], 32
	je .open_file
	movsb
	jmp .move_path
.open_file:
	print_newline
	print requested_file_path, 124
	print_newline

	cmp byte [requested_file_path], 0
	jne .open_path
	open index_file_name, 0x09	
	jmp .check_open
.open_path:
	open requested_file_path, 0x09
.check_open:
	mov r14, rax
	cmp r14, ERROR
	jle .open_error_file
	stat requested_file_path, rsp
	jmp .file_stat
.open_error_file:
	open open_error_file, 0x09
	mov r14, rax
	stat open_error_file, rsp
.file_stat:
	mov rbx, [rsp + 48]

	write r13, response, response_len

	send_file r13, r14, rbx 
	cmp rax, ERROR
	jle SendFileException

	print Success, SuccessLen
	close r13
	jmp accept_loop

	close r12
	exit 0

extract_file_name:
	xor rsi, rsi
.find_g:
	xor rdi, rdi
	cmp byte [file_request + rsi], 71
	je .find_indicator
	inc rsi
	jmp .find_g

.find_indicator:
	inc rsi
	cmp byte [file_request + rsi], 0
	je .not_found
	mov dl, byte [file_path_indicator + rdi]
	cmp dl, 0
	je .found
	cmp byte [file_request + rsi], dl
	jne .find_g
	inc rdi
	jmp .find_indicator

.found:
	ret
.not_found:
	xor rsi, rsi
	ret

SocketException:
	print SocketExceptionMsg, SocketExceptionMsgLen
	exit 1

BindException:
	print BindExceptionMsg, BindExceptionMsgLen
	close r12
	exit 1

ListenException:
	print ListenExceptionMsg, ListenExceptionMsgLen
	close r12
	exit 1

AcceptException:
	print AcceptExceptionMsg, AcceptExceptionMsgLen
	close r12
	exit 1

OpenException:
	print OpenExceptionMsg, OpenExceptionMsgLen
	close r12
	exit 1

SendFileException:
	print SendFileExceptionMsg, SendFileExceptionMsgLen
	close r12
	close r14
	exit 1
