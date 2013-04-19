; Use nasm and AsmToArr to get a thunk code

CODE16
; This simulate a stub for 16-bit functions that call a 32-bit function

; Builtin variables :
; 1111h = nParams
; 2222h = nParams * sizeof(DWORD) (we'll call it wBytesParams)
; 3333h = fFarReturn
; 6666h = low order of our 32-bit function
; 9999h = high order of our 32-bit function
; 2:3 = CallProc32W

push	bp
mov		bp, sp
call	2:3			; EnterWin16Lock
leave

xor		ax, ax

push	9999h		; high order of the function here
push	6666h		; low order of the function here
push	ax
push	ax
push	ax
push	1111h		; nParams
call	3:4			; CallProc32W
push	eax
call	4:5			; LeaveWin16Lock
pop		eax

mov		bx, 3333h

test	bx, bx
jnz		_retfar

retn	2222h		; retn wBytesParams

_retfar:
retf	2222h		; retf wBytesParams
