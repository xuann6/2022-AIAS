#init set t6 = 9 (constant SIZE).
.data
base_imm:   .byte     0 2 0 4 5 0 7 8 9
            .byte     4 5 6 7 8 9 0 2 3
            .byte     7 8 0 0 2 3 4 5 6
            .byte     0 1 4 3 0 5 8 0 7
            .byte     3 6 5 0 0 7 2 1 4
            .byte     8 9 7 2 1 4 3 6 5
            .byte     0 0 1 6 4 2 0 0 8
            .byte     6 4 2 9 7 8 5 3 1
            .byte     0 0 8 5 3 1 6 4 2
.text
main:
la a7 base_imm
addi t6 x0 9 
jal print_init
jal solve_sudoku
beqz a0 no_solution
jal print_finish
hcf

no_solution:
addi a0 x0 4
syscall
hcf

solve_sudoku:
addi s1 x0 0
addi s2 x0 0
addi t0 x0 1

addi sp sp 4
sw ra 0(sp)
jal number_unassigned
bnez a0 ss_i_loop
j ret_one

ss_i_loop:
add a2 x0 t0
jal is_safe
beqz a0 end_i_loop
mul t2 s1 t6
add t2 t2 s2
add t3 t2 a7
sb a2 0(t3)
jal solve_sudoku
bnez a0 ret_one
sb x0 0(t3)
j end_i_loop

end_i_loop:
addi t0 t0 1 
blt t0 t6 ss_i_loop
beq t0 t6 ss_i_loop
j ret_zero

ret_zero:
addi a0 x0 0
j exit_func

ret_one:
addi a0 x0 1 
j exit_func

exit_func:
lw ra 0(sp)
addi sp sp -4
ret

is_safe: 
addi t1 x0 0
addi t2 x0 0

row_loop:
mul t3 s1 t6
add t3 t3 t1
add t4 t3 a7
lb t5 0(t4)
bne t5 a2 end_row_loop
addi a0 x0 0
ret

end_row_loop:
addi t1 t1 1
bge t1 t6 init_column_loop
j row_loop

init_column_loop:
addi t1 x0 0
j column_loop

column_loop:
mul t3 t1 t6
add t3 t3 s2
add t4 t3 a7
lb t5 0(t4)
bne t5 a2 end_column_loop
addi a0 x0 0
ret

end_column_loop:
addi t1 t1 1
bge t1 t6 init_cell_loop
j column_loop

init_cell_loop:
addi t5 x0 3
div t3 s1 t5
mul a3 t3 t5
div t4 s2 t5
mul a4 t4 t5
add t1 x0 a3
add t2 x0 a4

addi a5 t1 3
addi a6 t2 3

cell_loop_i:
cell_loop_j:
mul t3 t1 t6
add t3 t3 t2
add t4 t3 a7
lb t5 0(t4)
bne t5 a2 cell_loop_jpp
addi a0 x0 0
ret

cell_loop_jpp:
addi t2 t2 1
bge t2 a6 end_cell_loop_j
j cell_loop_j


end_cell_loop_j:
addi t1 t1 1
add t2 x0 a4
bge t1 a5 end_safe_func
j cell_loop_i

end_safe_func:
addi a0 x0 1
ret

number_unassigned:
addi a2 x0 0
addi a3 x0 0
addi a4 x0 0

nu_i_loop:
nu_j_loop:
mul t1 a3 t6 
add t1 t1 a4
add t2 t1 a7
lb t3 0(t2)

bnez t3 jpp

add s1 x0 a3
add s2 x0 a4
addi a2 x0 1
add a0 x0 a2
ret

jpp:
addi a4 a4 1
bge a4 t6 end_nu_j_loop
j nu_j_loop

end_nu_j_loop:
addi a4 x0 0
addi a3 a3 1
bge a3 t6 end_num_func
j nu_i_loop

end_num_func:
add a0 x0 a2
ret

#*****************************
#                            *
#     Print Init Function    *
#							 *
#*****************************

print_init:
addi a0 x0 5 
j print

print_finish:
addi a0 x0 6
syscall
addi a0 x0 5
j print

print:
addi t0 x0 0
addi t1 x0 0
j print_cell

print_cell:
print_i_loop:
print_j_loop:
mul t2 t0 t6
add t3 t2 t1
add t3 t3 a7
lb a1 0(t3)
syscall
j print_jpp

print_jpp:
addi t1 t1 1
bge t1 t6 end_print_j_loop
j print_j_loop

end_print_j_loop:
addi t1 x0 0
addi t0 t0 1
bge t0 t6 end_print_func
j print_i_loop

end_print_func:
ret
