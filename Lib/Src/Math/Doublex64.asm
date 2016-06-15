; To be used in x64 mode when using class MathFunctions
; 
;

ROUND      EQU "0" 
ROUNDDOWN  EQU "1"
ROUNDUP    EQU "2" 
TRUNCATE   EQU "3" 

setRoundMode MACRO roundMode
	push	0
    fnstcw	WORD PTR[rsp]        ; save FPU ctrlWord in cwSave (=*rsp)
    mov		ax, WORD PTR[rsp]
ifidn     <roundMode>,<ROUND>
   and     ah, 0f3h		         ; clear bit 10,11
elseifidn <roundMode>,<ROUNDDOWN>
    or		ah, 04h              ; set bit 10
    and		ah, 0f7h             ; clear bit 11
elseifidn <roundMode>,<ROUNDUP>
    and     ah, 0fbh             ; clear bit 10
    or      ah, 8h               ; set bit 11
else
    or      ah, 0ch              ; set bit 10, 11
endif
    mov		WORD PTR[rsp+2], ax  ; crtlflags in rsp[2]
    fldcw	WORD PTR[rsp+2]      ; load new ctrlword              ROUNDDOWN
ENDM

restoreRoundMode MACRO
    fldcw	WORD PTR[rsp]        ; restore FPU  ctrlWord (=cwSave)
	add		rsp, 8
ENDM


.DATA

maxI32       dword 7fffffffh
maxI32P1     dword 80000000h
maxI64       qword 7fffffffffffffffh
maxI64P1     qword 8000000000000000h
m2pi2pow60   tbyte 403dc90fdaa22168c235h  ; 2*pi*pow2(60) (=7.244019458077122e+018)

.CODE

;void d64modulus(double &dst, const double &x, const Double80 &y);
d64modulus PROC
    fld		TBYTE PTR [r8]      ;                                                     st0=y
    fabs                        ; y = abs(y)                                          st0=|y|
    fld		QWORD PTR [rdx]     ;                                                     st0=x,st1=|y|
    fldz                        ;                                                     st0=0,st1=x,st2=|y|
    fcomip	st, st(1)           ; compare and pop zero                                st0=x,st1=|y|
	ja		Repeat_Negative_x   ; if st(0) > st(1) (0 > x) goto repeat_negative_x 

Repeat_Positive_x:              ; do {												  st0=x,st1=|y|, x > 0
    fprem                       ;   st0 %= y                                       
    fnstsw	ax
    sahf
	jpe		Repeat_Positive_x   ; } while(statusword.c2 != 0);
    fldz                        ;                                                     st0=0,st1=x,st2=|y|
    fcomip	st, st(1)           ; compare and pop zero
	jbe		pop2                ; if(st(0) <= st(1) (0 <= remainder) goto pop2
    fadd                        ; remainder += y
    fstp	QWORD PTR[rcx]      ; pop result
    ret                         ; return

Repeat_Negative_x:              ; do {                                                st0=x,st=|y|, x < 0
    fprem                       ;    st0 %= y
    fnstsw	ax
    sahf
	jpe		Repeat_Negative_x   ; } while(statusword.c2 != 0)
    fldz
    fcomip	st, st(1)           ; compare and pop zero
	jae		pop2                ; if(st(0) >= st(1) (0 >= remainder) goto pop2
    fsubr                       ; remainder -= y
    fstp	QWORD PTR[rcx]      ; pop result
    ret                         ; return

pop2:                           ;                                                     st0=x%y,st1=y
    fstp	QWORD PTR[rcx]      ; pop result
    fstp	st(0)               ; pop y
	ret
d64modulus ENDP

; ----------------------------------------- Double80 trigonometric functions ----------------------------------------

;void sincos(double &c, double &s);
sincos PROC
    push    rdx
	mov     rdx, rcx
	lea		r8, m2pi2pow60
	call	d64modulus
	pop     rdx
    fld		QWORD PTR[rcx]
    fsincos
    fstp	QWORD PTR [rcx]
    fstp	QWORD PTR [rdx]
    ret
sincos ENDP

END

