#include <UTFT.h>
#include <stdio.h>
//#include <string.h>

extern uint8_t SmallFont[];

#define STRSIZE 1500

char inputStringSerial1[STRSIZE] = {0};
char LCDString[30]= {0};

char indx1   = 0;
bool cmd_rcv = false;
char inChar  = 0;

int d=0;
int indx= 0;
char* start;
char* end;

UTFT myGLCD(ITDB32S,38,39,40,41);

void setup() 
{
  randomSeed(analogRead(0));

  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);

  myGLCD.clrScr();

  // initialize both serial ports:
  Serial.begin(115200);
  Serial1.begin(115200);
 
  memset(inputStringSerial1,0,STRSIZE);
  //memcpy(inputStringSerial1,"#\n461F9E S\n407D44 S    5417\n343650 S\n3C49EA S    4447 CFG9CA\n345543 S    6033 ANE26EA\n451D86 S    4417 TAP1020\n*\n",sizeof("#\n461F9E S\n407D44 S    5417\n343650 S\n3C49EA S    4447 CFG9CA\n345543 S    6033 ANE26EA\n451D86 S    4417 TAP1020\n*\n"));
 

  Serial.print("RESET\r\n");
}

int j=0;
void loop() 
{
    if (cmd_rcv==true)
    { 
      cmd_rcv = false;
      
      for (int i=0; i<14; i++)
      {
        end   = inputStringSerial1+indx;
        start = strchr(inputStringSerial1+indx,'\n');
        if (start>0)
        {
          d = start-end;
          if (d>30)
             d = 30;
          
          *start = ' ';

          memcpy(LCDString,inputStringSerial1+indx,d);
  
          if (! (strchr(LCDString,'#')>0))
          {
            char* a = strchr(LCDString,'\r');
            if (a > 0)
              *a = 0;
            
            myGLCD.print(LCDString,0,j*10);j++;
          }
          memset(LCDString,' ',30);
          indx+=d+1;
        }
        
        else
        {
          
          indx = 0;
          indx1= 0;
          memset(inputStringSerial1,0,STRSIZE);     
          memset(LCDString,' ',30);      

        }
         

      }

      indx = 0;
      indx1=0;
      memset(inputStringSerial1,0,STRSIZE);
      memset(LCDString,' ',30);
      j =0 ;


    }

}
 


 
//puerto con el PC 
void serialEvent() 
{
    inChar = (char)Serial.read();    
    Serial1.print(inChar);    
}


//puerto con el RBPI 
void serialEvent1() 
{
    inChar = (char)Serial1.read();    
    Serial.print(inChar);
    

    if (inChar == '*')
    {
      cmd_rcv = true;
    }
    inputStringSerial1[indx1] = inChar;
    indx1++;
    if (indx1>STRSIZE-1)
    {
      indx1=0;
      memset(inputStringSerial1,0,STRSIZE);
    } 

}
