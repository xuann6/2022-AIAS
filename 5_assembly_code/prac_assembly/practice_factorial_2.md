start:
    li a0, 5
    jal fact
    hlt

fact:
    addi  sp, sp, -8
    sw ra, 0(sp) 
    li t0, 2
    blt a0, t0, ret_one 
    sw a0, 4(sp)   
    addi a0, a0, -1
    jal fact       
    lw t0, 4(sp)   
    mul a1, t0, a1 
    j done

ret_one:
    li a1, 1

done:
    lw ra, 0(sp)
    addi sp, sp, 8 
    jr ra     
