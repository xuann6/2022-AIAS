
//s1 for r (row value)
//s2 for c (column value) 
//a2 for input n

is_safe: 
mov t1 0 // t1 for i, set t1 =0
mov t2 0 // t2 for j, set t2 =0
mov a3 base_imm

row_loop:
mul a4 s1 SIZE
add a4 a4 t1
ld a5 a3(a4) // might cause error
bnez a5 end_row_loop
mov a2 0 				//a2 is return value for this function 
jalr s0 0(ra)

end_row_loop:
bge t1 SIZE init_column_loop
addi t1 t1 1
j row_loop

init_column_loop:
mov t1 0
j column_loop

column_loop:
mul a4 t1 SIZE
add a4 a4 s2
ld a5 a3(a4) // might cause error
bnez a5 end_column_loop
mov a0 0 				//a2 is return value for this function 
jalr s0 0(ra)

end_column_loop:
bge t1 SIZE init_cell_loop
addi t1 t1 1
j column_loop

init_cell_loop:
div t3 s1 3 
mul t3 t3 3 // t3 <= row_start
div t4 s2 3 
mul t4 t4 3 // t4 <= col_start
mov t1 t3   // i= row_start
mov t2 t4   // j= col_start

addi t5 t1 3 // t5 <= row_start+3, end of i_loop
addi t6 t2 3 // t6 <= col_start+3, end of j_loop

cell_loop_i:
cell_loop_j:
mul a4 t1 SIZE
add a4 a4 t2
ld a5 a3(a4)
bne a5 a2 cell_loop_jpp
mov a0 0
jalr s0 0(ra)

cell_loop_jpp:
addi t2 t2 1
bge t2 t6 end_cell_loop_j
j cell_loop_j


end_cell_loop_j:
addi t1 t1 1
bge t1 t5 end_safe_func
j cell_loop_i

end_safe_func:
mov a0 1
jalr s0 0(ra)