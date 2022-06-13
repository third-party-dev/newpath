/*
https://stackoverflow.com/questions/1403772/how-can-i-check-if-a-java-programs-input-output-streams-are-connected-to-a-term

Java has no isatty(). Can use System.console() in most cases.
*/

package dev.thirdparty.way;

class Cli
{
  public static void main(String args[])  //static method  
  {  
    if (System.console() != null) {
      System.out.println("Not in a tty.");
    }
    else {
      System.out.println("In a tty.");
    }
  }  
}  