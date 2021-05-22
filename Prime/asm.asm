PUBLIC disable_interrupts
PUBLIC enable_interrupts
PUBLIC invalidate_tlb

.code _text
 
disable_interrupts PROC PUBLIC
 
cli
 
ret
disable_interrupts ENDP

enable_interrupts PROC PUBLIC
 
sti
 
ret
enable_interrupts ENDP

invalidate_tlb PROC PUBLIC
 
invlpg [rcx]
 
ret
invalidate_tlb ENDP
END
 
