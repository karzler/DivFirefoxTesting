/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Platform specific code to invoke XPCOM methods on native objects */
        .global XPTC_InvokeByIndex
/*
    XPTC_InvokeByIndex(nsISupports* that, uint32_t methodIndex,
                   uint32_t paramCount, nsXPTCVariant* params);
    
*/
XPTC_InvokeByIndex:
        save    %sp,-(64 + 16),%sp   ! room for the register window and
                                    ! struct pointer, rounded up to 0 % 16
        mov     %i2,%o0             ! paramCount
        call    invoke_count_words  ! returns the required stack size in %o0
        mov     %i3,%o1             ! params
        
	sll     %o0,2,%l0           ! number of bytes
        sub     %sp,%l0,%sp         ! create the additional stack space
            
        mov     %sp,%o0             ! pointer for copied args
        add     %o0,72,%o0          ! step past the register window, the
                                    ! struct result pointer and the 'this' slot
        mov     %i2,%o1             ! paramCount
        call    invoke_copy_to_stack
        mov     %i3,%o2             ! params
!
!   calculate the target address from the vtable
!
	add	%i1,1,%i1	    ! vTable is zero-based, index is 1 based (?)	
	ld	[%i0],%l1	    ! *that --> vTable
	sll	%i1,3,%i1
	add	%i1,%l1,%l1	    ! vTable[index * 8], l1 now points to vTable entry
	lduh	[%l1],%l0	    ! this adjustor
	sll	%l0,16,%l0          ! sign extend to 32 bits
	sra     %l0,16,%l0
	add     %l0,%i0,%i0         ! adjust this
        ld      [%l1 + 4],%l0       ! target address

.L5:    ld      [%sp + 88],%o5
.L4:	ld	[%sp + 84],%o4
.L3:	ld	[%sp + 80],%o3
.L2:	ld	[%sp + 76],%o2
.L1:	ld	[%sp + 72],%o1
.L0:
        jmpl    %l0,%o7             ! call the routine
! always have a 'this', from the incoming 'that'
	mov	%i0,%o0
        
	mov     %o0,%i0             ! propagate return value
        ret
        restore
