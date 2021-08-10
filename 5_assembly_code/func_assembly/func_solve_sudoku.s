

//s1 for r (row value)
//s2 for c (col value)

solve_sudoku:
mov s1 0 // init s1 <= 0 (row)
mov s2 0 // init s2 <= 0 (column)
mov t0 1 // t0 <= i (initial value 1)
mov t1 base_imm // load matrix base address into t1

addi sp sp -4
sw ra 0(sp)

jal number_unassigned 
bnez a0 i_loop
j ret_one

i_loop:
jal is_safe
beqz a0 end_i_loop // 
mul t2 s1 SIZE
add t2 t2 s2
mov t2 t0
jal solve_sudoku
bnez a0 ret_one
mov t2 0 // can't proceed solution
j end_i_loop

end_i_loop:
addi t0 t0 1
blt t0 SIZE ret_zero
j i_loop 

ret_zero:
mov a0 0
j exit_func

ret_one:
mov a0 1 
j exit_func

exit_func:
lw ra 0(sp)
addi sp sp 4
jalr s0 0(ra)