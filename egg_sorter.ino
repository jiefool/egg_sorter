#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27,20,4);
Servo smallServo;
Servo mediumServo;
Servo largeServo;

int currentWeightSmall = 0;  
int currentWeightMedium = 0;
int currentWeightLarge = 0;

int countSmall = 0;  
int countMedium = 0; 
int countLarge = 0; 

long sample=0;
float val=0;
long count=0;

long smallSample=0;
float smallVal=0;
long smallCount=0;

long mediumSample=0;
float mediumVal=0;
long mediumCount=0;

long largeSample=0;
float largeVal=0;
long largeCount=0;

int smallSCK = A4;
int smallDT = A5;

int mediumSCK = A2;
int mediumDT = A3;

int largeSCK = A0;
int largeDT = A1;

int extraLargeSCK = A6;
int extraLargeDT = A7;

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.clear();         
  lcd.backlight(); 

  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);

  smallServo.attach(10);
  mediumServo.attach(9);
  largeServo.attach(8);
  
  smallServo.write(180);
  mediumServo.write(180);
  largeServo.write(180);

  pinMode(smallSCK, OUTPUT);
  pinMode(mediumSCK, OUTPUT);
  pinMode(largeSCK, OUTPUT);
  
  lcd.print("    Weight ");
  lcd.setCursor(0,1);
  lcd.print(" Measurement ");
  delay(1000);
  lcd.clear();


  flickServo(smallServo);
  flickServo(mediumServo);
  flickServo(largeServo);
  
  calibrate(smallSCK, smallDT, "small");
  resetCounters();
  calibrate(mediumSCK, mediumDT, "medium");
  resetCounters();
  calibrate(largeSCK, largeDT, "large");
  resetCounters();



  constantScreen();
}

void loop() {  

  count= readCount(smallSCK, smallDT);
  currentWeightSmall=(((count-smallSample)/smallVal)-2*((count-smallSample)/smallVal));

  count= readCount(mediumSCK, mediumDT);
  currentWeightMedium=(((count-mediumSample)/mediumVal)-2*((count-mediumSample)/mediumVal));

  count= readCount(largeSCK, largeDT);
  currentWeightLarge=(((count-largeSample)/largeVal)-2*((count-largeSample)/largeVal));
  
  printToLCD(3,2, (String)currentWeightSmall + "g  ");
  printToLCD(9,2, (String)currentWeightMedium + "g  ");
  printToLCD(15,2, (String)currentWeightLarge + "g  ");

  printToLCD(3,3, (String)countSmall);
  printToLCD(9,3, (String)countMedium);
  printToLCD(15,3, (String)countLarge);

  if(currentWeightSmall >= 48 && currentWeightSmall <= 54){
    flickServo(smallServo);  
    countSmall++;
  }

  if(currentWeightMedium >= 55 && currentWeightMedium <= 59){
    flickServo(mediumServo);  
    countMedium++;
  }

  if(currentWeightLarge >= 60 && currentWeightLarge <= 71){
    flickServo(largeServo);  
    countLarge++; 
  }

}


void constantScreen(){
  printToLCD(0,0, "     Egg Sorter    ");
  printToLCD(0,1, "   SM    MD    LG  ");
  printToLCD(0,2, "W:");
  printToLCD(0,3, "C:");
}

void printToLCD(int x, int y, String text){
  lcd.setCursor(x,y);
  lcd.print(text);
}

void flickServo(Servo servo){
   servo.write(90);
   delay(200);
   servo.write(180);
   delay(200);
}

unsigned long readCount(int SCK, int DT)
{
  unsigned long Count;
  unsigned char i;
  pinMode(DT, OUTPUT);
  digitalWrite(DT,HIGH);
  digitalWrite(SCK,LOW);
  Count=0;
  pinMode(DT, INPUT);
  while(digitalRead(DT));
  for (i=0;i<24;i++)
  {
    digitalWrite(SCK,HIGH);
    Count=Count<<1;
    digitalWrite(SCK,LOW);
    if(digitalRead(DT)) 
    Count++;
  }
  digitalWrite(SCK,HIGH);
  Count=Count^0x800000;
  digitalWrite(SCK,LOW);
  return(Count);
}

void calibrate(int SCK, int DT, String scale)
{
  lcd.clear();
  lcd.print("Calibrating..." + scale);
  lcd.setCursor(0,1);
  lcd.print("Please Wait...");
  for(int i=0;i<100;i++)
  {
    count=readCount(SCK, DT);
    sample+=count;
    Serial.println(count);
  }
  sample/=100;
  Serial.print("Avg:");
  Serial.println(sample);
  lcd.clear();
  lcd.print("Put 100g & wait");
  count=0;
  while(count<1000)
  {
    count=readCount(SCK, DT);
    count=sample-count;
    Serial.println(count);
  }
  lcd.clear();
  lcd.print("Please Wait....");
  delay(2000);
  for(int i=0;i<100;i++)
  {
    count=readCount(SCK, DT);
    val+=sample-count;
    Serial.println(sample-count);
  }
  val=val/100.0;
  val=val/100.0;        // put here your calibrating weight
  lcd.clear();

  if(scale == "small"){
    smallCount = count;
    smallVal = val;
    smallSample = sample;
  }
  
  if(scale == "medium"){
    mediumCount = count;
    mediumVal = val;
    mediumSample = sample;
  }

  if(scale == "large"){
    largeCount = count;
    largeVal = val;
    largeSample = sample;
  }
}

void resetCounters(){
    val=0;
    sample=0;
    count=0;
}
