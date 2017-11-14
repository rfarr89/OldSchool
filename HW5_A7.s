# Assignment: 	Homework 5
# Program:	Exercise A.7
# Author:	Ryan Farrell (PSID: 1488274)
# Description:	Read 3 integers and print out the sum of the largest two.

.data
	welcome:	.asciiz "Welcome! Input 3 integers and I will return the sum of the largest 2.\n"	# string used for welcome message
	prompt:		.asciiz	"Please input an integer and press enter: "					# string used to prompt user for input
	resPrompt:	.asciiz "The sum of the 2 largest input integers is: "					# string used to display results
	line:		.asciiz	"\n"										# string used for line breaks
	goodbye:	.asciiz "Good Bye!\n"									# string used for exit message

.text

main:
	# welcome message
	li	$v0, 4		# syscall for print string
	la	$a0, welcome	# load string welcome into $a0 for printing
	syscall			# print welcome to the console

	# prompt user for input 1
	li	$v0, 4		# syscall for print string
	la	$a0, prompt	# load string prompt into $a0 for printing
	syscall			# print prompt to the console
	li	$v0, 5		# syscall for read integer
	syscall			# read integer
	move	$t0, $v0	# move input integer from $v0 to $t0
	li	$v0, 4		# syscall for print string
	la	$a0, line	# load string line into $a0 for printing
	syscall			# print line to the console

	# prompt user for input 2
	li	$v0, 4		# syscall for print string
	la	$a0, prompt	# load string prompt into $a0 for printing
	syscall			# print prompt to the console
	li	$v0, 5		# syscall for read integer
	syscall			# read integer
	move	$t1, $v0	# move input integer from $v0 to $t1
	li	$v0, 4		# syscall for print string
	la	$a0, line	# load string line into $a0 for printing
	syscall			# print line to the console

	# prompt user for input 3
	li	$v0, 4		# syscall for print string
	la	$a0, prompt	# load string prompt into $a0 for printing
	syscall			# print prompt to the console
	li	$v0, 5		# syscall for read integer
	syscall			# read integer
	move	$t2, $v0	# move input integer from $v0 to $t2
	li	$v0, 4		# syscall for print string
	la	$a0, line	# load string line into $a0 for printing
	syscall			# print line to the console

	# find the largest 2 (or smallest 1)
	ble	$t0, $t1, t0small	# branch to t0small if $t0 <= $t1
	ble	$t1, $t2, t1smallest	# branch to t1smallest if $t1 <= $t2 (and < $t0)
	b	t2smallest		# branch to t2smallest ($t2 < $t1 < $t0)

t0small:	# $t0 is smaller than $t1, test if smaller than $t2
	ble	$t0, $t2, t0smallest	# branch to t0smallest if $t0 <= $t2 (and <= $t1)
	b 	t2smallest		# branch to t2smallest if $t2 < $t0 (and <= $t1)

t0smallest:	# $t0 is known to be <= $t1 and $t2
	add	$t3, $t1, $t2	# add $t1 and $t2 if $t0 is smallest
	b	results		# branch to results

t1smallest:	# $t1 is known to be <= $t0 and $t2
	add	$t3, $t0, $t2	# add $t0 and $t2 if $t1 is smallest
	b	results		# branch to results

t2smallest:	# $t2 is known to be <= $t0 and $t1
	add	$t3, $t0, $t1	# add $t0 and $t1 if $t2 is smallest

results:	# display the sum of the largest 2 integers and exit
	li	$v0, 4		# syscall for print string
	la	$a0, resPrompt	# load string resPrompt into $a0 for printing
	syscall			# print resPrompt to console
	li	$v0, 1		# syscall for print integer
	move	$a0, $t3	# move sum of 2 largest integers from $t3 to $a0 for printing
	syscall			# print result to console
	li	$v0, 4		# syscall for print string
	la	$a0, line	# load string line into $a0 for printing
	syscall			# print line to console
	li	$v0, 4		# syscall for print string
	la	$a0, goodbye	# load string goodbye into $a0 for printing
	syscall			# print goodbye to console
	li	$v0, 10		# syscall to exit program
	syscall			# exit program

