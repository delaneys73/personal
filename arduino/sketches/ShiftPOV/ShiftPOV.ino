/*
¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡
persistence of vision typography with arduino
michael zoellner - march 2006
http://i.document.m05.de

connect anodes (+) of 5 leds to digital ports of the arduino board
and put 20-50 ohm resistors from the cathode (-) to ground.

the letters are lookup tables consisting arrays width the dot status in y rows.
¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡
*/


// defining the alphabet
int _[] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
int A[] = {0,1,1,1,1, 1,0,1,0,0, 0,1,1,1,1};
int B[] = {1,1,1,1,1, 1,0,1,0,1, 0,1,0,1,0};
int C[] = {0,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1};
int D[] = {1,1,1,1,1, 1,0,0,0,1, 0,1,1,1,0};
int E[] = {1,1,1,1,1, 1,0,1,0,1, 1,0,1,0,1};
int F[] = {1,1,1,1,1, 1,0,1,0,0, 1,0,1,0,0};
int G[] = {0,1,1,1,0, 1,0,1,0,1, 0,0,1,1,0};
int H[] = {1,1,1,1,1, 0,0,1,0,0, 1,1,1,1,1};
int I[] = {0,0,0,0,1, 1,0,1,1,1, 0,0,0,0,1};
int J[] = {1,0,0,0,0, 1,0,0,0,1, 1,1,1,1,1};
int K[] = {1,1,1,1,1, 0,0,1,0,0, 0,1,0,1,1};
int L[] = {1,1,1,1,1, 0,0,0,0,1, 0,0,0,0,1};
int M[] = {1,1,1,1,1, 0,1,1,0,0, 0,1,1,1,1};
int N[] = {1,1,1,1,1, 1,0,0,0,0, 0,1,1,1,1};
int O[] = {0,1,1,1,0, 1,0,0,0,1, 0,1,1,1,0};
int P[] = {1,1,1,1,1, 1,0,1,0,0, 0,1,0,0,0};
int Q[] = {0,1,1,1,1, 1,0,0,1,1, 0,1,1,1,1};
int R[] = {1,1,1,1,1, 1,0,1,0,0, 0,1,0,1,1};
int S[] = {0,1,0,0,1, 1,0,1,0,1, 1,0,0,1,0};
int T[] = {1,0,0,0,0, 1,1,1,1,1, 1,0,0,0,0};
int U[] = {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1};
int V[] = {1,1,1,1,0, 0,0,0,0,1, 1,1,1,1,0};
int W[] = {1,1,1,1,0, 0,0,1,1,0, 1,1,1,1,0};
int X[] = {1,1,0,1,1, 0,0,1,0,0, 1,1,0,1,1};
int Y[] = {1,1,0,0,0, 0,0,1,0,0, 1,1,1,1,1};
int Z[] = {1,0,0,1,1, 1,0,1,0,1, 1,1,0,0,1};

int letterSpace;
int dotTime;

//Pin connected to ST_CP of 74HC595
int latchPin = 12;
//Pin connected to SH_CP of 74HC595
int clockPin = 13;
////Pin connected to DS of 74HC595
int dataPin = 11;

//holder for infromation you're going to pass to shifting function
byte data = 0; 



void setup() {
  
  //set pins to output because they are addressed in the main loop
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(10,HIGH);
  digitalWrite(9,LOW);
  pinMode(latchPin, OUTPUT);
  
  // defining the space between the letters (ms)
  letterSpace = 6;
  // defining the time dots appear (ms)
  dotTime = 3;
  
}

void shiftColumn(int letter[],int offset,int delayTime)
{
  data = 0;
  // printing the first y row of the letter
  digitalWrite(latchPin, LOW);
  for (int y=0; y<5; y++)
  {
    bitWrite(data,y, letter[y]+offset);
  }
  shiftOut(dataPin,clockPin,LSBFIRST,data);
  digitalWrite(latchPin, HIGH);
  delay(delayTime);
}

void printLetter(int letter[])
{
  shiftColumn(letter,0,dotTime);
  shiftColumn(letter,5,dotTime);
  shiftColumn(letter,10,dotTime);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin,clockPin,LSBFIRST,0);
  digitalWrite(latchPin, HIGH);
  delay(letterSpace);
}

void loop()
{
  // printing some letters
  printLetter(N);
  printLetter(E);
  printLetter(R);
  printLetter(D);
  printLetter(S);
  printLetter(_);
}
