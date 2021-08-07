// R0 <= row
// R1 <= col
// R2 <= return value
int number_unassigned(int *row , int *col)
{
int num_unassign = 0; //R3
int i j; // R4 <= i, R5 <=j
for(i=0;i<SIZE;i++)
{
for(j=0;j<SIZE;j++)
{
//cell is unassigned
if(matrix[i][j] == 0)//access data memory R6 <= matrix[i][j]
{
//changing the values of row and col
*row = i;
*col = j;
//there is one or more unassigned cells
num_unassign = 1;
return num_unassign;
}
}
}
return num_unassign;
}


 mov R3, #0 // int num_unassign = 0;
mov R4, #0 // i=0
mov R5, #0 // j=0

number_unassigned:
mov R7, #base_imm //load R7 with matrix_base_addr

i_loop:

j_loop:

 mul R8, R4, #SIZE
add R8, R8, R5
ld R6, R7, R8 // R7<= base address register, R8<= offset register // load matrix[i][j] to R6
bneq R6, j_loop
//line17-line22
mov R0, R4 //*row = i;
mov R1, R5 //*col = j;
mov R2, R3

 add R5, R5, #1 //j++
bge R5, #SIZE, end_j_loop // j >= SIZE
b j_loop
end_j_loop:
add R4, R4, #1 // i++
bge R4, #SIZE, end // i >=SIZE
b i_loop:
end:

//n!
int factorial(n){
if(n==1)
return 1;
else
factorial(n-1);

}