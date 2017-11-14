package sudoku2;

import javax.swing.JPanel;

import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

import java.awt.Graphics;
import java.awt.Font;
import java.awt.Color;

/**
 * Contains information pertaining to displaying and altering the state of the sudoku board
 *
 */
public class Game extends JPanel
{
	private static final long serialVersionUID = 1L;
	
	
	private int squareWidth;
	private int squareHeight;
	
	private int numberXOffset;
	private int numberYOffset;
	
	private Square[][] board;
	
	private boolean checkCorrect;
	private int selectX, selectY;
	
	public Game()
	{
		Square.solveSudoku();
		board = new Square[Sudoku.ROWS_AND_COLUMNS][Sudoku.ROWS_AND_COLUMNS];
		fillBoard(0,0,PuzzleInput.getPuzzle());
		selectX = 0;
		selectY = 0;
		checkCorrect = false;
		setOpaque(false);
	}
	
	public void paintComponent(Graphics g)
	{
		drawBoard(g);
	}
	
	private void fillBoard(int row, int col, int[][] board)
	{
		if(col >= Sudoku.ROWS_AND_COLUMNS)
			fillBoard(row+1,0,board);
		else if(row >= Sudoku.ROWS_AND_COLUMNS)
			return;
		
		else
		{
			if(board[row][col] != 0)
				this.board[row][col] = new Square(row,col,board[row][col],true,true);
			else
				this.board[row][col] = new Square(row,col,0,false,false);
			fillBoard(row,col+1,board);
		}
	}
	
	private void drawSelection(Graphics g)
	{
		g.setColor(Color.BLUE);
		g.fillRect(Sudoku.BORDER_WIDTH + squareWidth * selectX, Sudoku.BORDER_WIDTH + squareHeight * selectY, squareWidth, Sudoku.LINE_WIDTH);
		g.fillRect(Sudoku.BORDER_WIDTH + squareWidth * selectX, Sudoku.BORDER_WIDTH + squareHeight * selectY, Sudoku.LINE_WIDTH, squareHeight);
		g.fillRect(Sudoku.BORDER_WIDTH + squareWidth * selectX, Sudoku.BORDER_WIDTH + squareHeight - Sudoku.LINE_WIDTH  + squareHeight * selectY, squareWidth, Sudoku.LINE_WIDTH);
		g.fillRect(Sudoku.BORDER_WIDTH + squareWidth - Sudoku.LINE_WIDTH + squareWidth * selectX, Sudoku.BORDER_WIDTH + squareHeight * selectY, Sudoku.LINE_WIDTH, squareHeight);
	}
	
	public void drawBoard(Graphics g)
	{
		g.setFont(new Font(Font.SANS_SERIF,Font.BOLD,40));
		
		squareWidth = (getWidth() - 2 * Sudoku.BORDER_WIDTH) / Sudoku.ROWS_AND_COLUMNS;
		squareHeight = (getHeight() - 2 * Sudoku.BORDER_WIDTH) / Sudoku.ROWS_AND_COLUMNS;
		numberXOffset = squareWidth / 3;
		numberYOffset = squareHeight * 3 / 4;
		
		for(int row = 1; row <= Sudoku.ROWS_AND_COLUMNS; row++)
			for(int col = 1; col <= Sudoku.ROWS_AND_COLUMNS; col++)
			{
				String printValue;
				if(boardValue(row-1,col-1) == 0)
					printValue = "";
				else
					printValue = Integer.toString(boardValue(row-1,col-1));
				if(checkCorrect)
				{
					if(board[row-1][col-1].isOriginal())
						g.setColor(Color.BLACK);
					else if(board[row-1][col-1].isCorrect())
						g.setColor(Color.GREEN);
					else
						g.setColor(Color.RED);						
				}
				else
					g.setColor(Color.BLACK);
				g.drawString(printValue, Sudoku.BORDER_WIDTH + numberXOffset + squareWidth * (col - 1), Sudoku.BORDER_WIDTH + numberYOffset + squareHeight * (row - 1));
			}
		drawSelection(g);
	}
	
	public int boardValue(int row, int col)
	{
		return board[row][col].getValue();
	}
	
	public void moveUp()
	{
		if(selectY > 0)
			selectY -= 1;
		else
			selectY = 0;
	}
	
	public void moveDown()
	{
		if(selectY < Sudoku.ROWS_AND_COLUMNS)
			selectY += 1;
		else
			selectY = Sudoku.ROWS_AND_COLUMNS;
	}
	
	public void moveRight()
	{
		if(selectX < Sudoku.ROWS_AND_COLUMNS)
			selectX += 1;
		else
			selectX = Sudoku.ROWS_AND_COLUMNS;
	}
	
	public void moveLeft()
	{
		if(selectX > 0)
			selectX -= 1;
		else
			selectX = 0;
	}
	
	public void changeValue(int value)
	{
		if(value < 0 || value > Sudoku.ROWS_AND_COLUMNS)
		{
			System.err.println("Invalid value input");
			return;
		}
		//Order is reversed from intuitive x,y because x is related to columns while y is related to rows.
		//Since the board is set up as board[rows][columns], setting the value must use board[y][x]
		if(!board[selectY][selectX].isOriginal())
			board[selectY][selectX].setValue(value);
	}
	
	public void toggleCheck()
	{
		checkCorrect = !checkCorrect;
	}
}
