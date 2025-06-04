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

fact:
	addi $sp, $sp, -8
	sw $fp, 0($sp)
	sw $ra, 4($sp)
	move $fp, $sp
	addi $sp, $sp, -12
	lw $t0, 8($fp)
	li $t1, 1
	bne $t0, $t1, label1
	lw $ra, 4($fp)
	addi $sp, $fp, 8
	lw $t0, 8($fp)
	lw $fp, 0($fp)
	move $v0, $t0
	jr $ra
	j label2
label1:
	lw $t0, -4($fp)
	lw $t1, 8($fp)
	li $t2, 1
	sub $t0, $t1, $t2
	sw $t0, -4($fp)
	addi $sp, $sp, -4
	lw $t0, -4($fp)
	sw $t0, 0($sp)
	jal fact
	addi $sp, $sp, 4
	lw $t0, -8($fp)
	move $t0, $v0
	sw $t0, -8($fp)
	lw $t0, -12($fp)
	lw $t1, 8($fp)
	lw $t2, -8($fp)
	mul $t0, $t1, $t2
	sw $t0, -12($fp)
	lw $ra, 4($fp)
	addi $sp, $fp, 8
	lw $t0, -12($fp)
	lw $fp, 0($fp)
	move $v0, $t0
	jr $ra
label2:
main:
	addi $sp, $sp, -8
	sw $fp, 0($sp)
	sw $ra, 4($sp)
	move $fp, $sp
	addi $sp, $sp, -20
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -4($fp)
	move $t0, $v0
	sw $t0, -4($fp)
	lw $t0, -4($fp)
	lw $t1, -8($fp)
	move $t1, $t0
	sw $t1, -8($fp)
	lw $t0, -8($fp)
	li $t1, 1
	ble $t0, $t1, label3
	addi $sp, $sp, -4
	lw $t0, -8($fp)
	sw $t0, 0($sp)
	jal fact
	addi $sp, $sp, 4
	lw $t0, -12($fp)
	move $t0, $v0
	sw $t0, -12($fp)
	lw $t0, -12($fp)
	lw $t1, -16($fp)
	move $t1, $t0
	sw $t1, -16($fp)
	j label4
label3:
	li $t0, 1
	lw $t1, -16($fp)
	move $t1, $t0
	sw $t1, -16($fp)
label4:
	lw $t0, -16($fp)
	move $a0, $t0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	li $t0, 0
	lw $t1, -20($fp)
	move $t1, $t0
	sw $t1, -20($fp)
	lw $ra, 4($fp)
	addi $sp, $fp, 8
	li $t0, 0
	lw $fp, 0($fp)
	move $v0, $t0
	jr $ra
