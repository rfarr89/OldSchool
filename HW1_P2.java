// COSC 4371, Spring 2018
// Ryan Farrell (1488274)

public class HW1_P2 {
  private static byte[] cipherText = new byte[] { -119, 119, 48, -18, 29, 23, -85, 81, 22, -85, 70, 74, -66, 90, 20, -15, 66, 5, -67, 65, 19, -95, 64, 0, -13, 83, 5, -68, 86, 18, -81, 64, 15, -18, 122, 48, -102, 98, 75, -1, 28, 85, -60 };
  
  private static void MultiTimePadTry(int key_size) {
	  // key_size needs to be greater than 0 and less than 6
	  if (key_size > 0 && key_size < 6) {
		  byte[] key = new byte[key_size];
		  key[0] = (byte)(cipherText[0] ^ (byte)'G');
		  if (key_size > 1) { key[1] = (byte)(cipherText[1] ^ (byte)'E'); }
		  if (key_size > 2) { key[2] = (byte)(cipherText[2] ^ (byte)'T'); }
		  if (key_size > 3) { key[3] = (byte)(cipherText[3] ^ (byte)' '); }
		  if (key_size > 4) { key[4] = (byte)(cipherText[4] ^ (byte)'/'); }
		  System.out.print("Key Size = " + key_size + " : ");
		  for (int i = 0; i < key.length; i++) {
		    	System.out.print(key[i] + " ");
		  }
		  System.out.println(" ");
	  }
	  else if (key_size < 0){
		  // key size must be greater than 0
		  System.out.println("The key size must be 1 or greater.");
	  }
	  else if (key_size > 5) {
		  // We are only certain that the text begins with "GET /". Beyond that it could be anything.
		  System.out.println("We only know the first 5 characters. Key size must be 5 or less.");
	  }
	  
  }
  
  public static void main(String [] args) {
	// Try multiple key sizes to look for a trend (repeat of key values)  
	for (int i = 0; i < 6; i++) {
		MultiTimePadTry(i);
	}
	// First 5 potential key values: -50, 50, 100, -50, 50
	// Since this is a multi-time pad we know the key repeats. 
	// Key values appear to be -50, 50 and 100, repeated. Try that:
	  
    // key may be of different length and - obviously - contain different values
    byte[] key = { 0, 0, 0};
        
    // key[i] = cipher[i] XOR plain[i]
    key[0] = (byte)(cipherText[0] ^ (byte)'G');
    key[1] = (byte)(cipherText[1] ^ (byte)'E');
    key[2] = (byte)(cipherText[2] ^ (byte)'T');
    
    for (int i = 0; i < cipherText.length; i++) 
      System.out.print((char) (cipherText[i] ^ key[i % key.length]));
  }  
}