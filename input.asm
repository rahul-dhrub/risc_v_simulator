.data
var: .word 7

.text
lw x10 var 
jal x1 fact
beq x0 x0 exit

fact:
addi sp sp -8
sw x10 4(sp)
sw x1 0(sp)
addi x5 x0 1
blt x5 x10 label
addi x10 x0 1
addi sp sp 8
jalr x3 0(x1)

label:
addi x10 x10 -1
jal x1 fact
addi x6 x10 0
lw x10 4(sp)
lw x1 0(sp)
mul x10 x10 x6
addi sp sp 8
jalr x3 0(x1)

exit:
fall_through 
