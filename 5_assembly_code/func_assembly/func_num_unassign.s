


mov s1 0 // s1 <= 0 (row)
mov s2 0 // s2 <= 0 (column)


number_unassigned:
mov a2 0 // a2 <= num_unassign return value
mov a3 0 // a3 <= i
mov a4 0 // a4 <= j
mov a5, base_imm //load a5 with matrix_base_addr

i_loop:
j_loop:

mul a6, a3, SIZE 
add a6, a6, a4	// get address of data matrix[i][j]
ld a7, a5, a6   //load matrix[i][j] to  

bnez a7, jpp //matrix[i][j]!=0, pass operation inside for loop and keep going

//line17-line22
mov s1, a3 //*row = i;
mov s2, a4 //*col = j;
mov a2, 1  // num_unassign = 1 
mov a0, a2
jalr s0 0(ra) //return ra, return value store in a0

jpp:
addi a4, a4, 1 //j++
bge a4, SIZE, end_j_loop // j >= SIZE
j j_loop

end_j_loop:
add a3, a3, 1 // i++
bge a3, SIZE, end_num_func // i >=SIZE
j i_loop:

end_num_func:
mov a0, a2
jalr s0 0(ra) //return ra, return value store in a0