/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(11, OUTPUT);     
    pinMode(10, OUTPUT);  
      pinMode(9, OUTPUT);  
}

void loop() {
  digitalWrite(11, HIGH);   // set the LED on
  delay(5);              // wait for a second
  digitalWrite(11,LOW);    // set the LED off // wait for a second
  digitalWrite(10, HIGH);   // set the LED on
  delay(15);              // wait for a second
  digitalWrite(10,LOW);    // set the LED off // wait for a second
  digitalWrite(9, HIGH);   // set the LED on
  delay(5);              // wait for a second
  digitalWrite(9,LOW);    // set the LED off // wait for a second
  
}
