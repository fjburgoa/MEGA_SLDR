#include <UTFT.h>
#include <stdio.h>

extern uint8_t SmallFont[];

#define STRSIZE   750
#define  LENX      35

char inputStringSerial1[STRSIZE] = {0};
char LCDString[LENX]= {0};

int  indx1   = 0;
bool cmd_rcv = false;
char inChar  = 0;

int d   = 0; //número de letras entre cada salto
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
  Serial.print("RESET\r\n");
}

int j=0; //fila

char longitud[10]= {0};

void loop() 
{
    //si se ha terminado de recibir un comando
    if (cmd_rcv==true)   
    { 
      myGLCD.clrScr();
      cmd_rcv = false;
      
      for (int i=0; i<25; i++)
      {
        end   = inputStringSerial1+indx;
        start = strchr(inputStringSerial1+indx,'\n');
        if (start>0)   //si se ha encontrado '\n'
        {
          d = start-end;
          if (d>LENX)
             d = LENX;
          
          *start = ' ';

          memcpy(LCDString,inputStringSerial1+indx,d);
  
          if (!(strchr(LCDString,'#')>0) &&  !(strchr(LCDString,'\t')>0) /* &&  !(strchr(LCDString,'*')>0)  &&  !(strchr(LCDString,'[')>0) */)
          {
            char* a = strchr(LCDString,'\r');
            if (a > 0)
              *a = 0;
            
            /*aquí gestiona la información*/         

            char* aa = strchr(LCDString,'.');
            if (*(aa-1) == ' ')
            {
               memcpy(longitud,aa-1,6);
            }
            else if (*(aa-2) == ' ')
            {
              memcpy(longitud,aa-2,7);   
            }
            else if (*(aa-3) == ' ')
            {
              memcpy(longitud,aa-3,9);    
            }
            
            myGLCD.print(LCDString,0,j*10); j++;
            
            if (aa > 0)
            {
              //myGLCD.printNumF(atof(longitud),4,0,j*10); j++;
              memset(longitud,0,10);    
            }
            
          }
          memset(LCDString,0,LENX);
          indx+=d+1;
        }
        else
        {
          indx = 0;
          indx1= 0;
          memset(inputStringSerial1,0,STRSIZE);     
          memset(LCDString,0,LENX);      
        }
      }

      indx = 0;
      indx1= 0;
      j    = 0;
      memset(inputStringSerial1,0,STRSIZE);
      memset(LCDString,0,LENX);
      
      
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
