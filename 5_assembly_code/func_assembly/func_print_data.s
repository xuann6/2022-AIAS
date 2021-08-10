print_init:
addi t0 s0 0 // t0 <= i
addi t1 s0 0 // t1 <= j
addi a0 s0 5
la a2 base_imm 
j print_cell

print_cell:

print_i_loop:
print_j_loop:
addi t6 s0 9
mul t1 s1 t6
add t1 t1 s2
lw a1 t1(base_imm) // a1 <= value of the matrix
syscall            // syscall, for a0=5, print register a1 value
j print_jpp

print_jpp:
addi s2 s2 1
bge s2 t6 end_print_j_loop
j print_j_loop

end_print_j_loop:
addi s1 s1 1
bge s1 t6 end_print_func
j print_i_loop

end_print_func:
jalr s0 0(ra)
