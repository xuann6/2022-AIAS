# 2022-Spring-AIAS TA 
[TOC]


## To-Do 
- to-do:
    - [x] function verified
    - [ ] assembly code simplify
    - [ ] add comment 

## Lab5: RISC-V Assembly
- 0809 weekly report
    - [GitHub Lab5 Assembly](https://github.com/xuann6/2022-AIAS/tree/main/5_assembly_code)
    - Practice
        - for loop
            - double for loop
        - recursive
            - factorial version 1
            - factorial version 2

    - Sudoku 
        - sudoku algorithm
            - [assembly code](https://github.com/xuann6/2022-AIAS/blob/main/5_assembly_code/func_assembly/riscv_sudoku.s)
            - [c code](https://github.com/xuann6/2022-AIAS/blob/main/5_assembly_code/soduku_algo_cpp.cpp)

        - function
            - [main.s](https://github.com/xuann6/2022-AIAS/blob/main/5_assembly_code/func_assembly/func_main.s)
            - [is_safe.s](https://github.com/xuann6/2022-AIAS/blob/main/5_assembly_code/func_assembly/func_is_safe.s)
            - [num_unassigned.s](https://github.com/xuann6/2022-AIAS/blob/main/5_assembly_code/func_assembly/func_num_unassign.s)
            - [solve_sudoku.s](https://github.com/xuann6/2022-AIAS/blob/main/5_assembly_code/func_assembly/func_solve_sudoku.s)
            - [print_data.s](https://github.com/xuann6/2022-AIAS/blob/main/5_assembly_code/func_assembly/func_print_data.s)


## Lab7: RISC-V Emulator
- 0809 weekly report
    - SYSCALL function
        - SYSCALL function will detect value in register a0, different value occurs different action.
            - register[a0]==4, system outputs "ERROR".
            - register[a0]==5, system prints all datas in the sudoku matrix. 
            - register[a0]==6, system prints "Execution Done".
    - Some extended functions can make our assembly code more concise
        - REM/ REMU/ DIV/ MUL (in M-standard extension)
        - <font color='#f00'>MUL and DIV for immediate value</font>

- 0726 weekly report
    - c code overview
    - emulator instruction extended 
        - ANDN/ BCLR/ BCLRI/ BEXT
    - extended function verified



