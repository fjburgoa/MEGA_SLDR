#include <UTFT.h>
#include <stdio.h>


extern uint8_t SmallFont[];

#define STRSIZE  750
#define MAX_ROWS 10
#define LENX     35


char inputStringSerial1[STRSIZE] = {0}; //texto recibido de la Rasbpberry
char LCDString[LENX]             = {0};         

//gestión de los datos recibidos x raspberry
int  indx1   = 0;   
bool cmd_rcv = false;
char inChar  = 0;

//para gestionar los datos una vez recibidos y poder mostrarlos por pantalla TFT
int d   = 0; //número de letras entre cada salto
int indx= 0;
char* start;
char* end;

//estructura
struct Aircraft
{
    char Id[7];
    int  Alt;
    int  Vel;
    float lat;
    float longit;
    char row;
};

Aircraft aircft[MAX_ROWS] = { 0 };


UTFT myGLCD(ITDB32S,38,39,40,41);

int getData(char* mybuff, Aircraft* myaircraft);

void setup() 
{
  randomSeed(analogRead(0));

  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);

  myGLCD.clrScr();

  // initialize serial ports:
  Serial3.begin(115200);  //MEGA <-> PC
  Serial2.begin(9600);    //MEGA <-> GPS  
  Serial1.begin(115200);  //MEGA <-> RBPI
 
  memset(inputStringSerial1,0,STRSIZE);
  Serial3.print("RESET\r\n");
}

int j=0; //fila
char longitud[MAX_ROWS]= {0};

void loop() 
{
    //si se ha terminado de recibir un comando
    if (cmd_rcv==true)   
    { 
      myGLCD.clrScr();
      cmd_rcv = false;
      
      if (getData(inputStringSerial1,aircft))
      {
        for (int i=0; i<MAX_ROWS; i++)
        {
          memset(LCDString,0,LENX);
          indx+=d+1;
        }
      }
      else
      {
          indx = 0;
          indx1= 0;
          memset(inputStringSerial1,0,STRSIZE);     
          memset(LCDString,0,LENX);      
      }
    }
    else
    {
      indx = 0;
      indx1= 0;
      j    = 0;
      memset(inputStringSerial1,0,STRSIZE);
      memset(LCDString,0,LENX);
    }      

}
  
//recibe datos del PC PS3: MEGA <-> PC
void serialEvent3() 
{
    inChar = (char)Serial3.read();    
    Serial1.print(inChar);        //reenvía a Raspberry
}

//recibe datos del PC PS2: MEGA <-> GPS
void serialEvent2() 
{
    inChar = (char)Serial2.read();    
    Serial3.print(inChar);       //de momento que los envíe al PC
    //Serial3.print("*");    
}


//recibe datos del PC PS2: MEGA <-> Raspberry
void serialEvent1() 
{
    inChar = (char)Serial1.read();    
    Serial3.print(inChar);      //Reenvía al PC

    if (inChar == '*')          //si recibe * es que ha terminado el envío de datos
      cmd_rcv = true;

    inputStringSerial1[indx1] = inChar;
    indx1++;
    if (indx1>STRSIZE-1)
    {
      indx1=0;
      memset(inputStringSerial1,0,STRSIZE);
    } 
}


// extract data from the buffer
int getData(char* mybuff, Aircraft* myaircraft)
{
    char* ret;
    ret = strchr(mybuff, '#');

    if (ret)
    {
        memcpy(mybuff, mybuff + 2, STRSIZE-2);
    }
    else
    {
        return -1;
    }

    for (int j = 0; j < MAX_ROWS; j++)
    {
        char* char_a = strchr(mybuff, 'a');
        char* char_b = strchr(mybuff, 'b');
        char* char_c = strchr(mybuff, 'c');
        char* char_d = strchr(mybuff, 'd');
        char* char_e = strchr(mybuff, 'e');
        char* char_r = strchr(mybuff, '\n');
        char draft[10] = { 0 };
        if (char_a)
        {
            //Id
            memset(myaircraft[j].Id, 0, 7);
            memcpy(myaircraft[j].Id, char_a + 1, char_b - char_a - 1);  
            //ALT
            memcpy(draft, char_b + 1, char_c - char_b - 1);
            myaircraft[j].Alt = atoi(draft);
            memset(draft, 0, 10);
            //VEL
            memcpy(draft, char_c + 1, char_d - char_c - 1);
            myaircraft[j].Vel = atoi(draft);
            memset(draft, 0, 10);
            //LAT
            memcpy(draft, char_d + 1, char_e - char_d - 1);
            myaircraft[j].lat = atof(draft);
            memset(draft, 0, 10);
            //LONG
            memcpy(draft, char_e + 1, char_r - char_e - 1);
            myaircraft[j].longit = atof(draft);
            memset(draft, 0, 10);

            int d = char_r - char_a;
            memcpy(mybuff, mybuff + d + 1, STRSIZE - d);
        }
        else
        {
          return 1;
        }

    }

    return 2;

}










          //IMPRIME POR PANTALLA
          
          /*
          end   = inputStringSerial1+indx;
          start = strchr(inputStringSerial1+indx,'\n');
          if (start>0)   //si se ha encontrado '\n'
          {
            d = start-end;
            if (d>LENX)
              d = LENX;
            
            *start = ' ';

            memcpy(LCDString,inputStringSerial1+indx,d);
    
            if (!(strchr(LCDString,'#')>0) &&  !(strchr(LCDString,'\t')>0))
            {
              char* a = strchr(LCDString,'\r');
              if (a > 0)
                *a = 0;
              
              //aquí gestiona la información

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
              */ 

