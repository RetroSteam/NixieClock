//driver for NCM107+NCT318+NCT818 (registers HV5122)
//ONLY FOR 1K revision!!!!
//driver version 1.0
//1 on register's output will turn on a digit 

#include "doIndication818.h"

#define UpperDotsMask 0x80000000
#define LowerDotsMask 0x40000000
#define TubeON 0xFFFF
#define TubeOFF 0x3C00

void doIndication()
{
  
  static unsigned long lastTimeInterval1Started;
  if ((micros()-lastTimeInterval1Started)<2000) return;
  lastTimeInterval1Started=micros();
    
  unsigned long Var32=0;
  
  long digits=stringToDisplay.toInt();
  
  /**********************************************************
   * Подготавливаем данные по 32бита 3 раза
   * Формат данных [H1][H2}[M1][M2][S1][Y1][Y2]
   *********************************************************/
  //-------- REG 2 ----------------------------------------------- 
  Var32|=(unsigned long)(SymbolArray[digits%10]&doEditBlink(7)&blankDigit(0)&moveMask())<<10; // Y2
  digits=digits/10;

  Var32 |= (unsigned long)SymbolArray[digits%10]&doEditBlink(6)&blankDigit(1)&moveMask();//y1
  digits=digits/10;

  if (LD) Var32&=~LowerDotsMask;
    else  Var32|=LowerDotsMask;
  
  if (UD) Var32&=~UpperDotsMask; 
    else  Var32|=UpperDotsMask; 
    

  digitalWrite(LEpin, LOW);    

  SPI.transfer(Var32>>24);
  SPI.transfer(Var32>>16);
  SPI.transfer(Var32>>8);
  SPI.transfer(Var32);
 //-------------------------------------------------------------------------

 //-------- REG 1 ----------------------------------------------- 
  Var32=0;
 
  Var32|=(unsigned long)(SymbolArray[digits%10]&doEditBlink(5)&blankDigit(2)&moveMask())<<20; // s2
  digits=digits/10;

  Var32|=(unsigned long)(SymbolArray[digits%10]&doEditBlink(4)&blankDigit(3)&moveMask())<<10; //s1
  digits=digits/10;

  Var32|=(unsigned long) (SymbolArray[digits%10]&doEditBlink(3)&blankDigit(4)&moveMask()); //m2
  digits=digits/10;

  if (LD) Var32&=~LowerDotsMask;
    else  Var32|=LowerDotsMask;
  
  if (UD) Var32&=~UpperDotsMask; 
    else  Var32|=UpperDotsMask; 

  SPI.transfer(Var32>>24);
  SPI.transfer(Var32>>16);
  SPI.transfer(Var32>>8);
  SPI.transfer(Var32);
 //-------------------------------------------------------------------------

 //-------- REG 0 ----------------------------------------------- 
  Var32=0;
  
  Var32|=(unsigned long)(SymbolArray[digits%10]&doEditBlink(2)&blankDigit(5)&moveMask())<<20; // m1
  digits=digits/10;

  Var32|= (unsigned long)(SymbolArray[digits%10]&doEditBlink(1)&blankDigit(6)&moveMask())<<10; //h2
  digits=digits/10;

  Var32|= (unsigned long)SymbolArray[digits%10]&doEditBlink(0)&blankDigit(7)&moveMask(); //h1
  digits=digits/10;

  if (LD) Var32&=~LowerDotsMask;  
    else  Var32|=LowerDotsMask;
  
  if (UD) Var32&=~UpperDotsMask; 
    else  Var32|=UpperDotsMask; 
     
  SPI.transfer(Var32>>24);
  SPI.transfer(Var32>>16);
  SPI.transfer(Var32>>8);
  SPI.transfer(Var32);

  digitalWrite(LEpin, HIGH);    
//-------------------------------------------------------------------------
}

word doEditBlink(int pos)
{
  if (!BlinkUp) return 0xFFFF;
  if (!BlinkDown) return 0xFFFF;
  int lowBit=blinkMask>>pos;
  lowBit=lowBit&B00000001;
  
  static unsigned long lastTimeEditBlink=millis();
  static bool blinkState=false;
  word mask=0xFFFF;
  static int tmp=0;//blinkMask;
  if ((millis()-lastTimeEditBlink)>300) 
  {
    lastTimeEditBlink=millis();
    blinkState=!blinkState;
    if (blinkState) tmp= 0;
      else tmp=blinkMask;
  }
  if (((dotPattern&~tmp)>>6)&1==1) LD=true;//digitalWrite(pinLowerDots, HIGH);
      else LD=false; //digitalWrite(pinLowerDots, LOW);
  if (((dotPattern&~tmp)>>7)&1==1) UD=true; //digitalWrite(pinUpperDots, HIGH);
      else UD=false; //digitalWrite(pinUpperDots, LOW);
      
  if ((blinkState==true) && (lowBit==1)) mask=0x3C00;//mask=B11111111;
  return mask;
}

word blankDigit(int pos)
{
  int lowBit=blankMask>>pos;
  lowBit=lowBit&B00000001;
  word mask=0xFFFF;
  if (lowBit==1) mask=0x3C00;
  return mask;
}

word moveMask()
{
  #define tubesQuantity 3
  static int callCounter=0;
  static int tubeCounter=0;
  word onoffTubeMask;
  if (callCounter == tubeCounter) onoffTubeMask=TubeON;
    else onoffTubeMask=TubeOFF;
  callCounter++;
  if (callCounter == tubesQuantity)
  {
    callCounter=0;
    tubeCounter++;
    if (tubeCounter == tubesQuantity) tubeCounter = 0;
  }
  return onoffTubeMask;
}
