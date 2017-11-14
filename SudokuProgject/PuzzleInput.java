package sudoku2;

import java.util.Random;
import java.util.Scanner;
import java.io.FileInputStream;
import java.io.FileNotFoundException;

import javax.swing.JFrame;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

/**Reads a source file containing a sudoku puzzle
 * The input file is expected to be a series of numbers from 0 to 9 separated by whitespace
 * All spaces on the sudoku board must be represented by a number. Any blank spaces should receive a value of 0.
 * 
 * The class will randomly read from the available sources.
 * The constant UNIQUEPUZZLES should be adjusted to match the number of available puzzles in the directory.
 * The names of puzzles must follow the format SampleSudoku#.txt where # is the number puzzle starting with 1.
 * If 3 puzzles are available to be read in the directory, UNIQUEPUZZLES should have a value of 3 and the names should be as follows:
 * 	SampleSudoku1.txt
 * 	SampleSudoku2.txt
 * 	SampleSudoku3.txt
 * 
 * #### This is not true in the current version. If a file is not able to be read, a window informing that the file could not be found will appear with a large exit button.
 * Clicking the button will exit the program. Closing the window will exit as well.
 */
public class PuzzleInput {
	private static int[][] puzzle;
	private static int[][] puzzleSolution;
	public static final int UNIQUEPUZZLES = 1;
	
/*	/**
	 * Used solely for the error window prompting the user to exit
	 *
	 */
/*	private class exitListener implements ActionListener
	{
		
		//Exits the program
		 
		public void actionPerformed(ActionEvent e)
		{
			System.exit(0);
		}
	}*/
	
	/**
	 * Reads the puzzle from a random source file available in the directory.
	 * File names should follow the format given above.
	 * 
	 * @return the sudoku puzzle read from the source file represented by integers
	 */
	public static int[][] getPuzzle()
	{
		puzzle = new int[Sudoku.ROWS_AND_COLUMNS][Sudoku.ROWS_AND_COLUMNS];
		Random r = new Random();
		String fileName = "SampleSudoku" + (r.nextInt(UNIQUEPUZZLES) + 1) + ".txt";
		Scanner input = null;
		
		try
		{
			input = new Scanner(new FileInputStream(fileName));
		}
		catch (FileNotFoundException e)
		{
			System.err.println(e);
/*			JFrame error = new JFrame("Error: File " + fileName + " not found");
			JButton exit = new JButton("Exit");
			exit.addActionListener(new exitListener());
			error.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			error.add(exit);
			error.setVisible(true);
			error.setSize(800, 200);*/
		}
		
		for(int row = 0; row < puzzle.length; row++)
			for(int col = 0; col < puzzle[row].length; col++)
				puzzle[row][col] = input.nextInt();
		input.close();
		
		return puzzle;
	}
	
	/**Implementation of this should be changed to actually solve the puzzle rather than just reading a file of the solved sudoku
	 * 
	 */
	public static int[][] getSolution()
	{
		Scanner input = null;
		puzzleSolution = new int[Sudoku.ROWS_AND_COLUMNS][Sudoku.ROWS_AND_COLUMNS];
		try
		{
			input = new Scanner(new FileInputStream("SampleSudoku1Answer.txt"));
		}
		catch (FileNotFoundException e)
		{
			System.err.println(e);
/*			JFrame error = new JFrame("Error: File SampleSudoku1Answer.txt not found");
			JButton exit = new JButton("Exit");
			exit.addActionListener(new exitListener());
			error.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			error.add(exit);
			error.setVisible(true);
			error.setSize(800, 200);*/
		}
		for(int row = 0; row < puzzleSolution.length; row++)
		{
			for(int col = 0; col < puzzleSolution[row].length; col++)
			{
				puzzleSolution[row][col] = input.nextInt();
			}

		}
		input.close();
		
		return puzzleSolution;
	}
}