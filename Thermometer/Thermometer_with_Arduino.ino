# include <SevenSeg.h> 

SevenSeg disp (2,3,4,5,6,7,8); 

const int numOfDigits=3; 
int digitPins[numOfDigits]={10,11,12}; 

int pushPin=A1;
int push=0;
int buttonState=0;

int tempC;
int tempF;
int tempK;
int tempPin = A2;

int ledB = A3;
int ledG = A4;
int ledR = A5;


void setup(){
  
disp.setCommonCathode();
disp.setDigitPins ( numOfDigits , digitPins );

pinMode(pushPin, INPUT);
pinMode(ledB, OUTPUT);
pinMode(ledG, OUTPUT);
pinMode(ledR, OUTPUT);

}

void loop(){
  
  tempC = analogRead(tempPin);           
  tempC = (5.0 * tempC * 100)/1023;  
  tempF = (tempC * 9/5) + 32;
  tempK = 273 + tempC;          

buttonState=digitalRead(pushPin);

if (buttonState==HIGH)
{
  push++;
  delay(100);
}

switch(push)
{
  case 1:
  disp.write(tempF);
  digitalWrite(ledG, HIGH);
  digitalWrite(ledB, LOW);
  digitalWrite(ledR, LOW);
  push=1;
  break;

  case 2:
  disp.write(tempK);
  digitalWrite(ledR, HIGH);
  digitalWrite(ledG, LOW);
  digitalWrite(ledB, LOW);
  push=2;
  break;

  default:
  disp.write(tempC);
  digitalWrite(ledB, HIGH);
  digitalWrite(ledR, LOW);
  digitalWrite(ledG, LOW);
  push = 0;
  break;
}
}
