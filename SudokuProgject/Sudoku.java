package sudoku2;

import javax.swing.JPanel;
import javax.swing.JFrame;

import java.awt.Graphics;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.Color;
import java.awt.BorderLayout;

/**Class which organizes the start and run sequence as well as contains constants which define the type of board and appearance of the Sudoku board
 * 
 *
 */
public class Sudoku extends JPanel 
{	
	private static final long serialVersionUID = 1L;
	
	public static final int ROWS_AND_COLUMNS = 9;
	public static final int ROWS_OR_COLUMNS_IN_A_BOX = 3;
	public static final int LINE_WIDTH = 5;
	public static final int BORDER_WIDTH = 100;
	
	private Game gameBoard;
	private KeyboardInput keyboard;
	
	/**Creates an instance of the Sudoku game by initializing and setting size, default close operation and visibility of a JFrame 
	 */
	public Sudoku()
	{
		JFrame window = new JFrame("Sudoku Game");
		gameBoard = new Game();
		keyboard = new KeyboardInput();
		window.setSize(800,800);
		window.setVisible(true);
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		window.setLayout(new BorderLayout());
		window.add(this, BorderLayout.CENTER);
		window.addKeyListener(keyboard);
		
		setLayout(new BorderLayout());
		
		add(gameBoard,BorderLayout.CENTER);
	}
	
	/**Creates the grid of the Sudoku game
	 * 
	 */
	public void paintComponent(Graphics g)
	{
		g.setColor(Color.WHITE);
		g.fillRect(0, 0, getWidth(), getHeight());
		drawGrid(g);
		gameBoard.repaint();
	}
	
	/**Recursive function to draw all horizontal lines separating each square
	 * 
	 * @param g Graphics used to draw
	 * @param line The line number to start on
	 */
	private void drawHorizontal(Graphics g, int line)
	{
		if(line >= ROWS_AND_COLUMNS)
			return;
		else
		{
			if(line % ROWS_OR_COLUMNS_IN_A_BOX == 0)
				g.setColor(Color.BLACK);
			else
				g.setColor(Color.LIGHT_GRAY);
			g.fillRect(BORDER_WIDTH, BORDER_WIDTH + (getHeight() - 2 * BORDER_WIDTH) * line / ROWS_AND_COLUMNS, getWidth() - 2 * BORDER_WIDTH, LINE_WIDTH);
			drawHorizontal(g,line+1);
		}
	}
	
	/**Recursive function to draw all vertical lines separating each square
	 * 
	 * @param g Graphics used to draw
	 * @param line The line number to start on
	 */
	private void drawVertical(Graphics g, int line)
	{
		if(line >= ROWS_AND_COLUMNS)
			return;
		else
		{
			if(line % ROWS_OR_COLUMNS_IN_A_BOX == 0)
				g.setColor(Color.BLACK);
			else
				g.setColor(Color.LIGHT_GRAY);
			g.fillRect(BORDER_WIDTH + (getWidth() - 2 * BORDER_WIDTH) * line / ROWS_AND_COLUMNS, BORDER_WIDTH, LINE_WIDTH, getHeight() - 2 * BORDER_WIDTH);
			drawVertical(g,line+1);
		}
	}
	
	/**Draws the outside border of the Sudoku board
	 * 
	 * @param g Graphics used to draw
	 */
	private void drawBorder(Graphics g)
	{
		g.setColor(Color.BLACK);
		g.fillRect(BORDER_WIDTH - LINE_WIDTH, BORDER_WIDTH - LINE_WIDTH, getWidth() - 2 * BORDER_WIDTH + 2 * LINE_WIDTH, LINE_WIDTH);
		g.fillRect(BORDER_WIDTH - LINE_WIDTH, BORDER_WIDTH - LINE_WIDTH, LINE_WIDTH, getHeight() - 2 * BORDER_WIDTH + 2 * LINE_WIDTH);
		g.fillRect(BORDER_WIDTH - LINE_WIDTH, getHeight() - BORDER_WIDTH, getWidth() - 2 * BORDER_WIDTH + 2 * LINE_WIDTH, LINE_WIDTH);
		g.fillRect(getWidth() - BORDER_WIDTH, BORDER_WIDTH - LINE_WIDTH, LINE_WIDTH, getHeight() - 2 * BORDER_WIDTH + 2 * LINE_WIDTH);

	}
	
	/**Single function which creates the border and all horizontal and vertical lines
	 * 
	 * @param g Graphics used to draw
	 */
	private void drawGrid(Graphics g)
	{
		drawHorizontal(g, 1);
		drawVertical(g, 1);
		drawBorder(g);
	}
	
	/**Class used to accept keyboard input
	 * 
	 * Valid inputs are:
	 * Arrow keys: Move the selected coordinates throughout the Sudoku board.
	 * Numbers 0-9: Alter the value of the selected square if the value can change. 0 will clear any value already input.
	 * Backspace: Functions the same as if 0 is input. Clears any numbers in the currently selected square if the value can change.
	 * c key: Toggles between whether the program will highlight correct input numbers in green and incorrect input numbers in red 
	 * 	or if all numbers will be black.
	 */
	private class KeyboardInput extends KeyAdapter
	{
		/**Determines when a key is pressed and what key is pressed
		 * 
		 * @param e Event fired when a key is pressed
		 */
		public void keyPressed(KeyEvent e)
		{
			int code = e.getKeyCode();
			if(code == KeyEvent.VK_UP)
			{
				gameBoard.moveUp();
			}
			else if(code == KeyEvent.VK_DOWN)
				gameBoard.moveDown();
			else if(code == KeyEvent.VK_LEFT)
				gameBoard.moveLeft();
			else if(code == KeyEvent.VK_RIGHT)
				gameBoard.moveRight();
			else if(code == KeyEvent.VK_1)
				gameBoard.changeValue(1);
			else if(code == KeyEvent.VK_2)
				gameBoard.changeValue(2);
			else if(code == KeyEvent.VK_3)
				gameBoard.changeValue(3);
			else if(code == KeyEvent.VK_4)
				gameBoard.changeValue(4);
			else if(code == KeyEvent.VK_5)
				gameBoard.changeValue(5);
			else if(code == KeyEvent.VK_6)
				gameBoard.changeValue(6);
			else if(code == KeyEvent.VK_7)
				gameBoard.changeValue(7);
			else if(code == KeyEvent.VK_8)
				gameBoard.changeValue(8);
			else if(code == KeyEvent.VK_9)
				gameBoard.changeValue(9);
			else if(code == KeyEvent.VK_0)
				gameBoard.changeValue(0);
			else if(code == KeyEvent.VK_BACK_SPACE)
				gameBoard.changeValue(0);
			else if(code == KeyEvent.VK_C)
				gameBoard.toggleCheck();
			e.consume();
		}
	}
}
