.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
	li $v0, 4
	la $a0, _prompt
	syscall
	li $v0, 5
	syscall
	jr $ra

write:
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, _ret
	syscall
	move $v0, $0
	jr $ra

main:
	addi $sp, $sp, -8
	sw $fp, 0($sp)
	sw $ra, 4($sp)
	move $fp, $sp
	addi $sp, $sp, -36
	li $t0, 0
	lw $t1, -4($fp)
	move $t1, $t0
	sw $t1, -4($fp)
	li $t0, 1
	lw $t1, -8($fp)
	move $t1, $t0
	sw $t1, -8($fp)
	li $t0, 0
	lw $t1, -12($fp)
	move $t1, $t0
	sw $t1, -12($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -16($fp)
	move $t0, $v0
	sw $t0, -16($fp)
	lw $t0, -16($fp)
	lw $t1, -20($fp)
	move $t1, $t0
	sw $t1, -20($fp)
label1:
	lw $t0, -12($fp)
	lw $t1, -20($fp)
	bge $t0, $t1, label2
	lw $t0, -24($fp)
	lw $t1, -4($fp)
	lw $t2, -8($fp)
	add $t0, $t1, $t2
	sw $t0, -24($fp)
	lw $t0, -24($fp)
	lw $t1, -28($fp)
	move $t1, $t0
	sw $t1, -28($fp)
	lw $t0, -8($fp)
	move $a0, $t0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	li $t0, 0
	lw $t1, -32($fp)
	move $t1, $t0
	sw $t1, -32($fp)
	lw $t0, -8($fp)
	lw $t1, -4($fp)
	move $t1, $t0
	sw $t1, -4($fp)
	lw $t0, -28($fp)
	lw $t1, -8($fp)
	move $t1, $t0
	sw $t1, -8($fp)
	lw $t0, -36($fp)
	lw $t1, -12($fp)
	li $t2, 1
	add $t0, $t1, $t2
	sw $t0, -36($fp)
	lw $t0, -36($fp)
	lw $t1, -12($fp)
	move $t1, $t0
	sw $t1, -12($fp)
	j label1
label2:
	lw $ra, 4($fp)
	addi $sp, $fp, 8
	li $t0, 0
	lw $fp, 0($fp)
	move $v0, $t0
	jr $ra
