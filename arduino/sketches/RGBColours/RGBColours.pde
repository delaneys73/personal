float RGB1[3];
float RGB2[3];
float INC[3];

int red, green, blue;

int RedPin = 9;
int GreenPin = 11;
int BluePin = 10;


void setup()
{
    Serial.begin(9600);
  randomSeed(analogRead(0));
  
for (int x=0; x<3; x++) {
  RGB1[x] = random(256);
  RGB2[x] = random(256); }

}

void loop()
{
  randomSeed(analogRead(0));
  
  for (int x=0; x<3; x++) {
    INC[x] = (RGB1[x] - RGB2[x]) / 256; }
  
  for (int x=0; x<256; x++) {
    
    red = int(RGB1[0]);
    green = int(RGB1[1]);
    blue = int(RGB1[2]);

    analogWrite (RedPin, red);  
    analogWrite (GreenPin, green);  
    analogWrite (BluePin, blue);    
    delay(75);  

    for (int x=0; x<3; x++) {
	   RGB1[x] -= INC[x];}
 
  }
  
  for (int x=0; x<3; x++) {
  RGB2[x] = random(956)-700;
  RGB2[x] = constrain(RGB2[x], 0, 255);
  
  delay(500);
 }
}
 
