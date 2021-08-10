main:
jalr solve_sudoku
beqz a0 no_solution
hlt // use syscall to print matrix 

no_solution:
hcf // use syscall to print error
