//s1 <= n
//s2 <= output n!

main_init:
addi s1 s0 7 
addi t0 s0 1 //set t0=1, push n, n-1, ..., 1 into stack
addi t3 s1 0 //set t3=s0 

factorial_push: 
addi sp sp -4
sw t3 0(sp)
beq t3 t0 pop_init
addi t3 t3 -1
j factorial_push 


pop_init:
addi s2 s0 1 //set s2=1, since output value is at least one
addi t2 s0 0 //set t2 =0 to count how many stacks we have doned
j factorial_pop

factorial_pop:
lw t1 0(sp) 
addi t2 t2 1
mul s2 s2 t1
addi sp sp 4
beq s1 t2 end
j factorial_pop

end:
hlt