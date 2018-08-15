/*
Student:    Ryan Farrell (1488274)
Assignment: Homework 2
Class:      COSC 4371, Intro. to Web Security, Spring 2018
 */

import javax.crypto.Cipher;
import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.BadPaddingException;
import java.security.MessageDigest;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
 
public class HW2 {
  static void P1() throws Exception {
    byte[] iv = new byte[] { 0, 0, 0, 0, 
                             0, 0, 0, 0, 
                             0, 0, 0, 0, 
                             0, 0, 0, 0 };
    byte[] key = Files.readAllBytes(Paths.get("P1_key"));
    byte[] cipherText = Files.readAllBytes(Paths.get("P1_cipher.txt"));
    
    // BEGIN SOLUTION
    // Step 1: set secret key specs
    // 1.1: set the key for the cipher using the key from file "P1_key"
    final SecretKeySpec key_final = new SecretKeySpec(key, "AES");
    // 1.2: set the IV for the cipher using the above 16-byte zero array
    final IvParameterSpec iv_final = new IvParameterSpec(iv);
    // Step 2: create a Cipher object using AES with CBC and ISO10126Padding
    final Cipher cipher = Cipher.getInstance("AES/CBC/ISO10126Padding");
    // Step 3: decrypt the cipher using the final key and IV
    cipher.init(Cipher.DECRYPT_MODE, key_final, iv_final);
    // Step 4: compute the cipher and save to plainText
    byte[] plainText = cipher.doFinal(cipherText);
    System.out.println("Problem 1 solution: ");
    // END SOLUTION
    
    System.out.println(new String(plainText, StandardCharsets.UTF_8));
  }

  static void P2() throws Exception {
    byte[] cipherBMP = Files.readAllBytes(Paths.get("P2_cipher.bmp"));
    
    // BEGIN SOLUTION
    // Step 1: compute MD5 hash using solution from problem 1
    // 1.1: create messagedigest object
    MessageDigest msgDigest = MessageDigest.getInstance("MD5");
    // 1.2: set plaintext as found in problem 1
    byte[] P1_plaintext = "The quick brown fox jumps over the lazy dog.".getBytes(StandardCharsets.UTF_8);
    // 1.3: add desired plaintext to the messagedigest object
    msgDigest.update(P1_plaintext);
    // 1.4: execute the messagedigest as specified (MD5 on plaintext from problem 1) and save to new byte array
    byte[] hashBytes = msgDigest.digest();
    // the following lines were used to check the hash values computed
/*
    System.out.println("The MD5 hash of the plaintext in P1 is:");
    for (int i = 0; i < 16; i++) {
      System.out.print((byte)hashBytes[i] + " ");
    }
*/
    // Step 2: decrypt the encrypted .bmp file
    // 2.1: set the key for the cipher using the MD5 hash values computed above
    final SecretKeySpec key_final = new SecretKeySpec(hashBytes, "AES");
    // 2.2: create a Cipher object using AES with EBC and ISO10126Padding
    final Cipher cipher = Cipher.getInstance("AES/ECB/ISO10126Padding");
    // 2.3: decrypt the cipher using the final key as defined above
    cipher.init(Cipher.DECRYPT_MODE, key_final);
    // 2.4: compute the cipher and save to plainBMP
    byte[] plainBMP = cipher.doFinal(cipherBMP);
    System.out.println("\nProblem 2 solved. Please review 'P2_plain.bmp' located in working directory.");
    // END SOLUTION
    
    Files.write(Paths.get("P2_plain.bmp"), plainBMP);
  }

  static void P3() throws Exception {
    byte[] cipherBMP = Files.readAllBytes(Paths.get("P3_cipher.bmp"));
    
    // BEGIN SOLUTION
    // Step 1: create byte array large enough to fit the cipherBMP and an additional 54 byte header
    byte[] modifiedBMP = new byte[cipherBMP.length + 54];
    // Step 2: create byte array containing the header of a sample BMP with the same expected parameters as the cipherBMP
    byte[] sampleBMP = Files.readAllBytes(Paths.get("P2_plain.bmp"));
    // Step 3: copy the header bytes (first 54 bytes) from sampleBMP to modifiedBMP
    for (int i = 0; i < 54; i++) {
      modifiedBMP[i] = sampleBMP[i];
    }
    // Step 4: copy the contents of cipherBMP into the remainder of modifiedBMP
    for (int i = 54; i < modifiedBMP.length; i++) {
      modifiedBMP[i] = cipherBMP[i - 54];
    }
    System.out.println("\nProblem 3 solved. Please review 'P3_cipher_modified.bmp' located in working directory.");
    // END SOLUTION
    
    Files.write(Paths.get("P3_cipher_modified.bmp"), modifiedBMP);
  }

  static void P4() throws Exception {
    byte[] iv = new byte[] { 0, 0, 0, 0, 
                             0, 0, 0, 0, 
                             0, 0, 0, 0, 
                             0, 0, 0, 0 };
    byte[] cipherBMP = Files.readAllBytes(Paths.get("P4_cipher.bmp"));
    
    // BEGIN SOLUTION
    System.out.println("\nDetermining key value for Problem 4. Please be patient.");
    byte[] sampleBMP = Files.readAllBytes(Paths.get("P2_plain.bmp"));
    byte[] plainBMP = cipherBMP;
    byte hour = 0;
    byte minute = 0;
    byte second = 0;
    byte[] key = new byte[] {  65,   6,    39,   69,
                              115,   hour, minute, second,
                                0,   0,    0,   0,
                                0,   0,    0,   0 };
    boolean solved = false;
    for (int hr = 0; hr < 24; hr++) {
      for (int mn = 0; mn < 60; mn++) {
        for (int sc = 0; sc < 60; sc++) {
          try {
            // set key for this trial
            key[5] = (byte) hr;
            key[6] = (byte) mn;
            key[7] = (byte) sc;
            // set secret key parameters (key to try and AES method)
            SecretKeySpec key_trial = new SecretKeySpec(key, "AES");
            // set the IV for the cipher using the above 16-byte zero array
            IvParameterSpec iv_trial = new IvParameterSpec(iv);
            // create a Cipher object using AES with CBC and ISO10126Padding
            Cipher cipher = Cipher.getInstance("AES/CBC/ISO10126Padding");
            // decrypt the cipher using the trial key and IV
            cipher.init(Cipher.DECRYPT_MODE, key_trial, iv_trial);
            // compute the cipher and save to trialBMP
            byte[] trialBMP = cipher.doFinal(cipherBMP);
            // check if first 6 bytes match first 6 bytes from P2_Plain.bmp
            for (int i = 0; i < 6; i++) {
              if (trialBMP[i] == sampleBMP[i]) {
                // do nothing, keep looping locally
                if (i == 5) {
                  // we have a match if we get here
                  solved = true;
                  System.out.println("Solution for Problem 4 found using hour = " + hr + ", minute = " + mn + ", and seconds = " + sc + ".");
                  System.out.println("Please review 'P4_plain.bmp' located in working directory.");
                  plainBMP = trialBMP;
                }
              } else {
                break;
              }
            }
          } catch (BadPaddingException e) {
            continue;
          }
          if (solved) {break;}
        }
        if (solved) {break;}
      }
      if (solved) {break;}
    }

    // END SOLUTION
    
    Files.write(Paths.get("P4_plain.bmp"), plainBMP);
  }

  public static void main(String [] args) {
    try {  
      P1();
      P2();
      P3();
      P4();
    } catch (Exception e) {
      e.printStackTrace();
    } 
  }
}
