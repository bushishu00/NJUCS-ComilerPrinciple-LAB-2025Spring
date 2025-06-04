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

dist:
	addi $sp, $sp, -8
	sw $fp, 0($sp)
	sw $ra, 4($sp)
	move $fp, $sp
	addi $sp, $sp, -28
	lw $t0, -4($fp)
	lw $t1, 16($fp)
	lw $t2, 8($fp)
	sub $t0, $t1, $t2
	sw $t0, -4($fp)
	lw $t0, -8($fp)
	lw $t1, 16($fp)
	lw $t2, 8($fp)
	sub $t0, $t1, $t2
	sw $t0, -8($fp)
	lw $t0, -12($fp)
	lw $t1, -4($fp)
	lw $t2, -8($fp)
	mul $t0, $t1, $t2
	sw $t0, -12($fp)
	lw $t0, -16($fp)
	lw $t1, 20($fp)
	lw $t2, 12($fp)
	sub $t0, $t1, $t2
	sw $t0, -16($fp)
	lw $t0, -20($fp)
	lw $t1, 20($fp)
	lw $t2, 12($fp)
	sub $t0, $t1, $t2
	sw $t0, -20($fp)
	lw $t0, -24($fp)
	lw $t1, -16($fp)
	lw $t2, -20($fp)
	mul $t0, $t1, $t2
	sw $t0, -24($fp)
	lw $t0, -28($fp)
	lw $t1, -12($fp)
	lw $t2, -24($fp)
	add $t0, $t1, $t2
	sw $t0, -28($fp)
	lw $ra, 4($fp)
	addi $sp, $fp, 8
	lw $t0, -28($fp)
	lw $fp, 0($fp)
	move $v0, $t0
	jr $ra
check:
	addi $sp, $sp, -8
	sw $fp, 0($sp)
	sw $ra, 4($sp)
	move $fp, $sp
	addi $sp, $sp, -40
	li $t0, 0
	lw $t1, -4($fp)
	move $t1, $t0
	sw $t1, -4($fp)
	addi $sp, $sp, -4
	lw $t0, 20($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 16($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 12($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 8($fp)
	sw $t0, 0($sp)
	jal dist
	addi $sp, $sp, 16
	lw $t0, -8($fp)
	move $t0, $v0
	sw $t0, -8($fp)
	lw $t0, -8($fp)
	li $t1, 0
	ble $t0, $t1, label2
	addi $sp, $sp, -4
	lw $t0, 20($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 16($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 12($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 8($fp)
	sw $t0, 0($sp)
	jal dist
	addi $sp, $sp, 16
	lw $t0, -12($fp)
	move $t0, $v0
	sw $t0, -12($fp)
	addi $sp, $sp, -4
	lw $t0, 28($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 24($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 20($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 16($fp)
	sw $t0, 0($sp)
	jal dist
	addi $sp, $sp, 16
	lw $t0, -16($fp)
	move $t0, $v0
	sw $t0, -16($fp)
	lw $t0, -12($fp)
	lw $t1, -16($fp)
	bne $t0, $t1, label2
	addi $sp, $sp, -4
	lw $t0, 28($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 24($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 20($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 16($fp)
	sw $t0, 0($sp)
	jal dist
	addi $sp, $sp, 16
	lw $t0, -20($fp)
	move $t0, $v0
	sw $t0, -20($fp)
	addi $sp, $sp, -4
	lw $t0, 36($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 32($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 28($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 24($fp)
	sw $t0, 0($sp)
	jal dist
	addi $sp, $sp, 16
	lw $t0, -24($fp)
	move $t0, $v0
	sw $t0, -24($fp)
	lw $t0, -20($fp)
	lw $t1, -24($fp)
	bne $t0, $t1, label2
	addi $sp, $sp, -4
	lw $t0, 36($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 32($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 28($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 24($fp)
	sw $t0, 0($sp)
	jal dist
	addi $sp, $sp, 16
	lw $t0, -28($fp)
	move $t0, $v0
	sw $t0, -28($fp)
	addi $sp, $sp, -4
	lw $t0, 12($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 8($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 36($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 32($fp)
	sw $t0, 0($sp)
	jal dist
	addi $sp, $sp, 16
	lw $t0, -32($fp)
	move $t0, $v0
	sw $t0, -32($fp)
	lw $t0, -28($fp)
	lw $t1, -32($fp)
	bne $t0, $t1, label2
	addi $sp, $sp, -4
	lw $t0, 28($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 24($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 12($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 8($fp)
	sw $t0, 0($sp)
	jal dist
	addi $sp, $sp, 16
	lw $t0, -36($fp)
	move $t0, $v0
	sw $t0, -36($fp)
	addi $sp, $sp, -4
	lw $t0, 36($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 32($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 20($fp)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, 16($fp)
	sw $t0, 0($sp)
	jal dist
	addi $sp, $sp, 16
	lw $t0, -40($fp)
	move $t0, $v0
	sw $t0, -40($fp)
	lw $t0, -36($fp)
	lw $t1, -40($fp)
	beq $t0, $t1, label1
	j label2
label1:
	li $t0, 1
	lw $t1, -4($fp)
	move $t1, $t0
	sw $t1, -4($fp)
label2:
	lw $ra, 4($fp)
	addi $sp, $fp, 8
	lw $t0, -4($fp)
	lw $fp, 0($fp)
	move $v0, $t0
	jr $ra
main:
	addi $sp, $sp, -8
	sw $fp, 0($sp)
	sw $ra, 4($sp)
	move $fp, $sp
	addi $sp, $sp, -352
	lw $t0, -36($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -36($fp)
	lw $t0, -40($fp)
	addi $t1, $fp, -8
	lw $t2, -36($fp)
	add $t0, $t1, $t2
	sw $t0, -40($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -44($fp)
	move $t0, $v0
	sw $t0, -44($fp)
	lw $t0, -44($fp)
lw $t7, -40($fp)
sw $t0, 0($t7)
	lw $t0, -48($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -48($fp)
	lw $t0, -52($fp)
	addi $t1, $fp, -8
	lw $t2, -48($fp)
	add $t0, $t1, $t2
	sw $t0, -52($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -56($fp)
	move $t0, $v0
	sw $t0, -56($fp)
	lw $t0, -56($fp)
lw $t7, -52($fp)
sw $t0, 0($t7)
	lw $t0, -60($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -60($fp)
	lw $t0, -64($fp)
	addi $t1, $fp, -16
	lw $t2, -60($fp)
	add $t0, $t1, $t2
	sw $t0, -64($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -68($fp)
	move $t0, $v0
	sw $t0, -68($fp)
	lw $t0, -68($fp)
lw $t7, -64($fp)
sw $t0, 0($t7)
	lw $t0, -72($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -72($fp)
	lw $t0, -76($fp)
	addi $t1, $fp, -16
	lw $t2, -72($fp)
	add $t0, $t1, $t2
	sw $t0, -76($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -80($fp)
	move $t0, $v0
	sw $t0, -80($fp)
	lw $t0, -80($fp)
lw $t7, -76($fp)
sw $t0, 0($t7)
	lw $t0, -84($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -84($fp)
	lw $t0, -88($fp)
	addi $t1, $fp, -24
	lw $t2, -84($fp)
	add $t0, $t1, $t2
	sw $t0, -88($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -92($fp)
	move $t0, $v0
	sw $t0, -92($fp)
	lw $t0, -92($fp)
lw $t7, -88($fp)
sw $t0, 0($t7)
	lw $t0, -96($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -96($fp)
	lw $t0, -100($fp)
	addi $t1, $fp, -24
	lw $t2, -96($fp)
	add $t0, $t1, $t2
	sw $t0, -100($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -104($fp)
	move $t0, $v0
	sw $t0, -104($fp)
	lw $t0, -104($fp)
lw $t7, -100($fp)
sw $t0, 0($t7)
	lw $t0, -108($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -108($fp)
	lw $t0, -112($fp)
	addi $t1, $fp, -32
	lw $t2, -108($fp)
	add $t0, $t1, $t2
	sw $t0, -112($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -116($fp)
	move $t0, $v0
	sw $t0, -116($fp)
	lw $t0, -116($fp)
lw $t7, -112($fp)
sw $t0, 0($t7)
	lw $t0, -120($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -120($fp)
	lw $t0, -124($fp)
	addi $t1, $fp, -32
	lw $t2, -120($fp)
	add $t0, $t1, $t2
	sw $t0, -124($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -128($fp)
	move $t0, $v0
	sw $t0, -128($fp)
	lw $t0, -128($fp)
lw $t7, -124($fp)
sw $t0, 0($t7)
	lw $t0, -132($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -132($fp)
	lw $t0, -136($fp)
	addi $t1, $fp, -8
	lw $t2, -132($fp)
	add $t0, $t1, $t2
	sw $t0, -136($fp)
	lw $t0, -140($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -140($fp)
	lw $t0, -144($fp)
	addi $t1, $fp, -8
	lw $t2, -140($fp)
	add $t0, $t1, $t2
	sw $t0, -144($fp)
	lw $t0, -148($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -148($fp)
	lw $t0, -152($fp)
	addi $t1, $fp, -16
	lw $t2, -148($fp)
	add $t0, $t1, $t2
	sw $t0, -152($fp)
	lw $t0, -156($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -156($fp)
	lw $t0, -160($fp)
	addi $t1, $fp, -16
	lw $t2, -156($fp)
	add $t0, $t1, $t2
	sw $t0, -160($fp)
	lw $t0, -164($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -164($fp)
	lw $t0, -168($fp)
	addi $t1, $fp, -24
	lw $t2, -164($fp)
	add $t0, $t1, $t2
	sw $t0, -168($fp)
	lw $t0, -172($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -172($fp)
	lw $t0, -176($fp)
	addi $t1, $fp, -24
	lw $t2, -172($fp)
	add $t0, $t1, $t2
	sw $t0, -176($fp)
	lw $t0, -180($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -180($fp)
	lw $t0, -184($fp)
	addi $t1, $fp, -32
	lw $t2, -180($fp)
	add $t0, $t1, $t2
	sw $t0, -184($fp)
	lw $t0, -188($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -188($fp)
	lw $t0, -192($fp)
	addi $t1, $fp, -32
	lw $t2, -188($fp)
	add $t0, $t1, $t2
	sw $t0, -192($fp)
	addi $sp, $sp, -4
	lw $t0, -192($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -184($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -176($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -168($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -160($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -152($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -144($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -136($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	jal check
	addi $sp, $sp, 32
	lw $t0, -196($fp)
	move $t0, $v0
	sw $t0, -196($fp)
	lw $t0, -196($fp)
	lw $t1, -200($fp)
	move $t1, $t0
	sw $t1, -200($fp)
	lw $t0, -204($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -204($fp)
	lw $t0, -208($fp)
	addi $t1, $fp, -8
	lw $t2, -204($fp)
	add $t0, $t1, $t2
	sw $t0, -208($fp)
	lw $t0, -212($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -212($fp)
	lw $t0, -216($fp)
	addi $t1, $fp, -8
	lw $t2, -212($fp)
	add $t0, $t1, $t2
	sw $t0, -216($fp)
	lw $t0, -220($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -220($fp)
	lw $t0, -224($fp)
	addi $t1, $fp, -24
	lw $t2, -220($fp)
	add $t0, $t1, $t2
	sw $t0, -224($fp)
	lw $t0, -228($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -228($fp)
	lw $t0, -232($fp)
	addi $t1, $fp, -24
	lw $t2, -228($fp)
	add $t0, $t1, $t2
	sw $t0, -232($fp)
	lw $t0, -236($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -236($fp)
	lw $t0, -240($fp)
	addi $t1, $fp, -16
	lw $t2, -236($fp)
	add $t0, $t1, $t2
	sw $t0, -240($fp)
	lw $t0, -244($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -244($fp)
	lw $t0, -248($fp)
	addi $t1, $fp, -16
	lw $t2, -244($fp)
	add $t0, $t1, $t2
	sw $t0, -248($fp)
	lw $t0, -252($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -252($fp)
	lw $t0, -256($fp)
	addi $t1, $fp, -32
	lw $t2, -252($fp)
	add $t0, $t1, $t2
	sw $t0, -256($fp)
	lw $t0, -260($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -260($fp)
	lw $t0, -264($fp)
	addi $t1, $fp, -32
	lw $t2, -260($fp)
	add $t0, $t1, $t2
	sw $t0, -264($fp)
	addi $sp, $sp, -4
	lw $t0, -264($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -256($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -248($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -240($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -232($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -224($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -216($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -208($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	jal check
	addi $sp, $sp, 32
	lw $t0, -268($fp)
	move $t0, $v0
	sw $t0, -268($fp)
	lw $t0, -268($fp)
	lw $t1, -272($fp)
	move $t1, $t0
	sw $t1, -272($fp)
	lw $t0, -276($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -276($fp)
	lw $t0, -280($fp)
	addi $t1, $fp, -8
	lw $t2, -276($fp)
	add $t0, $t1, $t2
	sw $t0, -280($fp)
	lw $t0, -284($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -284($fp)
	lw $t0, -288($fp)
	addi $t1, $fp, -8
	lw $t2, -284($fp)
	add $t0, $t1, $t2
	sw $t0, -288($fp)
	lw $t0, -292($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -292($fp)
	lw $t0, -296($fp)
	addi $t1, $fp, -16
	lw $t2, -292($fp)
	add $t0, $t1, $t2
	sw $t0, -296($fp)
	lw $t0, -300($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -300($fp)
	lw $t0, -304($fp)
	addi $t1, $fp, -16
	lw $t2, -300($fp)
	add $t0, $t1, $t2
	sw $t0, -304($fp)
	lw $t0, -308($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -308($fp)
	lw $t0, -312($fp)
	addi $t1, $fp, -32
	lw $t2, -308($fp)
	add $t0, $t1, $t2
	sw $t0, -312($fp)
	lw $t0, -316($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -316($fp)
	lw $t0, -320($fp)
	addi $t1, $fp, -32
	lw $t2, -316($fp)
	add $t0, $t1, $t2
	sw $t0, -320($fp)
	lw $t0, -324($fp)
	li $t1, 0
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -324($fp)
	lw $t0, -328($fp)
	addi $t1, $fp, -24
	lw $t2, -324($fp)
	add $t0, $t1, $t2
	sw $t0, -328($fp)
	lw $t0, -332($fp)
	li $t1, 1
	li $t2, 4
	mul $t0, $t1, $t2
	sw $t0, -332($fp)
	lw $t0, -336($fp)
	addi $t1, $fp, -24
	lw $t2, -332($fp)
	add $t0, $t1, $t2
	sw $t0, -336($fp)
	addi $sp, $sp, -4
	lw $t0, -336($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -328($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -320($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -312($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -304($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -296($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -288($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	addi $sp, $sp, -4
	lw $t0, -280($fp)
	lw $t0, 0($t0)
	sw $t0, 0($sp)
	jal check
	addi $sp, $sp, 32
	lw $t0, -340($fp)
	move $t0, $v0
	sw $t0, -340($fp)
	lw $t0, -340($fp)
	lw $t1, -344($fp)
	move $t1, $t0
	sw $t1, -344($fp)
	li $t0, 0
	lw $t1, -348($fp)
	move $t1, $t0
	sw $t1, -348($fp)
	lw $t0, -200($fp)
	li $t1, 0
	bne $t0, $t1, label3
	lw $t0, -272($fp)
	li $t1, 0
	bne $t0, $t1, label3
	lw $t0, -344($fp)
	li $t1, 0
	bne $t0, $t1, label3
	j label4
label3:
	li $t0, 1
	lw $t1, -348($fp)
	move $t1, $t0
	sw $t1, -348($fp)
label4:
	lw $t0, -348($fp)
	move $a0, $t0
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	li $t0, 0
	lw $t1, -352($fp)
	move $t1, $t0
	sw $t1, -352($fp)
	lw $ra, 4($fp)
	addi $sp, $fp, 8
	li $t0, 0
	lw $fp, 0($fp)
	move $v0, $t0
	jr $ra
