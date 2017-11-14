# Class:	COSC 3330 Computer Architecture, Fall 2016
# Assignment: 	Homework 5
# Program:	Exercise A.6
# Author:	Ryan Farrell (PSID: 1488274)
# Description:	Read numbers input by the user and print a running sum.

.data
	welcome:	.asciiz "Welcome to running sum adder! I can keep a running sum for you.\n"	# string used for welcome message
	prompt:		.asciiz	"Please input a number to add and press enter: "	# string used to prompt user for input
	sumStr:		.asciiz	"The running sum is: "					# string used to inform user of running sum total
	again:		.asciiz "Input 1 to keep adding or 0 to exit: "			# string used to prompt user for loop
	line:		.asciiz	"\n"							# string used for line breaks
	goodbye:	.asciiz "Good Bye!\n"						# string used for exit message

.text

main:
	# display welcome message
	li	$v0, 4		# syscall command for print string
	la	$a0, welcome	# load string welcome into $a0 for printing to console
	syscall			# print welcome to console

addPrompt:
	# line break
	li	$v0, 4		# syscall command for print string	
	la	$a0, line	# load string line into $a0 for printing to console
	syscall			# print line to console

	# display input prompt to console
	li	$v0, 4		# syscall command for print string	
	la	$a0, prompt	# load string prompt into $a0 for printing to console
	syscall			# print prompt to console
	
	# read in double value from console
	li	$v0, 7		# syscall command for read double
	syscall			# read double value from user input
	
	# update running sum in register $f12
	add.d	$f12, $f12, $f0	# add $f0 (input double) to $f12 (running sum, double)
	
	# line break
	li	$v0, 4		# syscall command for print string	
	la	$a0, line	# load string line into $a0 for printing to console
	syscall			# print line to console

	# display running sum to console
	li	$v0, 4		# syscall command for print string	
	la	$a0, sumStr	# load string sumStr into $a0 for printing to console
	syscall			# print sumStr to console
	li	$v0, 3		# syscall command for print double (stored in $f12)
	syscall			# print running sum to console

	# double line break
	li	$v0, 4		# syscall command for print string	
	la	$a0, line	# load string line into $a0 for printing to console
	syscall			# print line to console
	syscall			# print line to console (again)

	# prompt for loop
againloop:
	li	$v0, 4		# syscall command for print string
	la	$a0, again	# load string again into $a0 for printing to console
	syscall			# print again to console
	li	$v0, 5		# syscall command for read integer
	syscall			# read integer from console
	move 	$t0, $v0	# move user input from $v0 to $t0
	
	# line break
	li	$v0, 4		# syscall command for print string	
	la	$a0, line	# load string line into $a0 for printing to console
	syscall			# print line to console
	
	# branch conditions
	beq	$t0, 1, addPrompt	# branch to back to addPrompt if user input 1 to keep adding
	beq	$t0, 0, endProg		# branch to endProg if user input 0 to exit
	b againloop			# branch to againloop if user did not input a proper input	

	# code to exit program
endProg:
	li	$v0, 4		# syscall command for print string
	la	$a0, goodbye	# load string goodbye into $a0 for printing to console
	syscall			# print goodbye to console
	li	$v0, 10		# syscall command to exit program
	syscall			# exit program