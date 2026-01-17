.export _check_collision

.segment "CODE"

PTR2 = TEMP+2 ;and TEMP+3

;unsigned char __fastcall__ check_collision(void * object1, void * object2);
_check_collision: ; Thank you, Doug Fraker: https://github.com/nesdoug/08_Pads/blob/0cba54d5a8b36968f5786028d68550d1d80a7725/LIB/nesdoug.s#L147
	; sprite object collision code
	; this would work with any size struct, as long as the first 4 bytes are...
	; x, y, width, height
	; note PTR is the same as TEMP and TEMP+1

	sta PTR
	stx PTR+1 ;set up a pointer to the first object
	jsr popax
	sta PTR2
	stx PTR2+1 ;set up a pointer to the second object
	

	ldy #0
	lda (PTR),y
	sta TEMP+4  	;X 1
	lda (PTR2), y
	sta TEMP+6		;X 2
	iny
	iny
	lda (PTR),y
	sta TEMP+5 		;width1
	lda (PTR2), y
	sta TEMP+7		;width2

	
;see if they are colliding x
	
;first check if obj1 R (obj1 x + width) < obj2 L
	
	lda TEMP+4 ;X 1
	clc
	adc TEMP+5 ;width 1
	cmp TEMP+6 ;X 2
	bcc @no
	
;now check if obj1 L > obj2 R (obj2 x + width)

	lda TEMP+6 ;X 2
	clc
	adc TEMP+7 ;width 2
	cmp TEMP+4 ;X 1
	bcc @no


;repeat process with y
	ldy #1
	lda (PTR),y
	sta TEMP+4  	;Y 1
	lda (PTR2), y
	sta TEMP+6		;Y 2
	iny
	iny
	lda (PTR),y
	sta TEMP+5 		;height1
	lda (PTR2), y
	sta TEMP+7		;height2

;see if they are colliding y
	
;first check if obj1 Bottom (obj1 y + height) < obj2 Top
	
	lda TEMP+4 ;Y 1
	clc
	adc TEMP+5 ;height 1
	cmp TEMP+6 ;Y 2
	bcc @no
	
;now check if obj1 Top > obj2 Bottom (obj2 y + height)

	lda TEMP+6 ;Y 2
	clc
	adc TEMP+7 ;height 2
	cmp TEMP+4 ;Y 1
	bcc @no


@yes:
	lda #1
	ldx #0
	rts
	
@no:
	lda #0
	tax ;ldx #0
	rts
