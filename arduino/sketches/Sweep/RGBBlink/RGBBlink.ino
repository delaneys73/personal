/*
  RGBBlink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */


int RED = 9;
int BLUE = 10;
int GREEN = 11;

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(RED, OUTPUT);     
  pinMode(BLUE, OUTPUT);     
  pinMode(GREEN, OUTPUT);     
  Serial.begin(19200);
  Serial.println("Startup");
}

void loop() {
  Serial.println("Start");
  digitalWrite(RED, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(RED, LOW);    // set the LED off
  delay(1000);              // wait for a second
    digitalWrite(BLUE, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(BLUE, LOW);    // set the LED off
  delay(1000);              // wait for a second
  digitalWrite(GREEN, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(GREEN, LOW);    // set the LED off
  delay(1000);              // wait for a second

}
