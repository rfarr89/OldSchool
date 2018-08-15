// COSC 4371, Spring 2018
// Ryan Farrell (1488274)

public class HW1_P1 {
  private static final String cipherText = "GWN GANBDJAN HWNDG ZD EJAZNK WNAN:\n" +
    "DBZI QARL DFNINGRO ZDIBOK GR GWN NBDG-DRJGWNBDG.\n" +
    "BQGNA GPR KBVD, VRJ PZII QZOK B KNDNAGNK ZDIBOK.\n" +
    "HIZLE GR GWN WZTWNDG URZOG RQ GWN ZDIBOK BOK IRRF QRA B IBATN HIZQQ GR GWN ORAGW.\n" +
    "PBIF B WJOKANK VBAKD GRPBAKD GWN HIZQQ BOK DGRU BG GWN GBII GANN.\n" +
    "PBIF GNO QNNG GR GWN NBDG, BOK VRJ PZII QZOK B ARHF PZGW BO S UBZOGNK RO ZG.\n" +
    "GWN GANBDJAN ZD EJAZNK RON QRRG ENIRP GWN ARHF.\n";
  
  static final int alphaIndex = (int) 'A';
  static final int alphaLength = (int) 'Z' + 1 - (int) 'A';
  
  // needs to be filled out automatically based on the ciphertext
  static int[] frequency = new int[alphaLength];
  
  private static int decrypt(int index) {
    // ... decrypt ...
	  // k1 = 3
	  int k1_inv = 9;  
	  int k2 = 1;
	  // add 26 to index to avoid error when (index - k2) <= 0
	  index = (((index+26) - k2)*(k1_inv))%26;
	  return index;
  }
  
  private static void cryptanalysis() {
	  // function to count and print the frequency of each character in the ciphertext
	  for (char cipherChar : cipherText.toCharArray()) {
	      if (Character.isLetter(cipherChar)) { // only letters are encrypted, punctuation marks and whitespace are not
	        // following line converts letters to numbers between 0 and 25
	        int cipher = (int) cipherChar - alphaIndex;
	        // following line increments the value at frequency[cipher] to count frequency of letters used
	        frequency[cipher] = frequency[cipher] + 1;
	      }
	  }  
	  char currChar = 'A';
  	  for (int i = 0; i < 26; i++) {
  		  System.out.println(currChar++ + " " + frequency[i]);
  	  }
  }
  
  public static void main(String [] args) {
	  // run cryptanalysis() to determine frequency of each character in the ciphertext
	  cryptanalysis();
	  
	  for (char cipherChar : cipherText.toCharArray())
		  if (Character.isLetter(cipherChar)) { // only letters are encrypted, punctuation marks and whitespace are not
			  // following line converts letters to numbers between 0 and 25
			  int cipher = (int) cipherChar - alphaIndex;
			  int plain = decrypt(cipher);
			  // following line coverts numbers between 0 and 25 to letters
			  char plainChar = (char) (plain + alphaIndex);
			  System.out.print(plainChar);
		  }
		  else
			  System.out.print(cipherChar);
  }  
}
