#include <TinyGPS.h>
#include <OneButton.h>
#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
SoftwareSerial Gsm(7, 8);
AltSoftSerial bt(10,11);
char phone_no[11];//= "7678060335";// replace with phone no. to get sms
OneButton button(2,true);              //button 1 at pin 2 for pattern
OneButton tap(3,true);                 //button 2 at pin 3 for SOS and findme feature
bool newData = "false"; 
TinyGPS gps;  //Creates a new instance of the TinyGPS object
 int x=0,y=0,l=0,k=0;                   
void setup()
{
  /*Serial.begin(9600);                           //begin GPS(at serial port)
  Gsm.begin(9600);                              //begin GSM*/

  button.attachClick(click);                            //for pattern
  button.attachDoubleClick(doubleclick);                //for pattern
  button.attachPress(press);                            //to change the pattern
  
  tap.attachClick(SOS);                               //for SOS feature
  tap.attachDoubleClick(FINDME);                      // for find the phone via device
  //tap.attachPress(nothing);

  pinMode(13,OUTPUT);                                 //buzzer
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);                               //TX for output
  pinMode(10, INPUT);                                //RX for Input
  pinMode(8, OUTPUT);                                //TX for output
  pinMode(7, INPUT);                                 //RX for Input
  digitalWrite(13, LOW);                             //initialy buzzer is off
  digitalWrite(12, LOW);                              //led for checking pattern lock in setting mode
  Serial.begin(9600);                           //begin GPS(at serial port)
  Gsm.begin(9600);                              //begin GSM

}
 
void loop()
{
  //bool newData = false;
  //unsigned long chars;
  //unsigned short sentences, failed;
  int u=0;
  int income;                           // to switch acc to requirement

  //char phone_no[11];
  byte index = 0;         
  bool ended = false;

  if(bt.available()>0)
   {
    income=bt.read();
    switch(income)
    {
     case '1':                       //for input of mobile no. for SOS feature
     u=0;
     if(u!=1)
      {
       while(Serial.available() > 0)
       {
        if(index < 10)
        {
         int v= Serial.read();
         if(v!= '\n' && v!= '\r')
         {
            phone_no[index] = Serial.read();
            index++;  
         }
         else
         {
             ended = true;
             phone_no[10] = '\0';
         }
        }
       }
       if(ended)
       {
        // Use the data in buff
        index = 0;
        ended = false;
        u=1;
       }
      }
     break;
     case '2':                //2 for incoming signal as change the pattern
     if(bt.available()>0)
    {
    char patt=bt.read();
    switch(patt)
   {
    case 'W':            //for XY
    l=0;
    k=0;
    break;
    
    case 'X':            //for YX
    l=0;
    k=1;
    break;
  
    case 'Y':            //for XX
    l=1;
    k=0;
    break;
      
    case 'Z':            //for YY
    l=1;
    k=1;
    break;
    }
   }

  case '&':
  digitalWrite(13,HIGH);      //to ring the device via phone
  }
 }
 tap.tick();                 // to check the status
 button.tick();              // to check the status
 if(x==1)
 {
  delay(100);
  button.tick();
  if(y==1)
  {
   digitalWrite(13,LOW);         //buzzer gets off
  }
 }
}

void SOS()
{
 if(bt.available()>0)               //for short range
 {
  bt.write("C");       //sos
 }
  // For one second we parse GPS data and report some key values
 for (unsigned long start = millis(); millis() - start < 1000;)       //for long range
 {
  while (Serial.available())
  {
   char c = Serial.read();
   Serial.print(c);
   if (gps.encode(c)) 
   newData = true;  
  }
 }
 if (newData)      //If newData is true
 {
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);   
  Gsm.print("AT+CMGF=1\r"); 
  delay(400);
  Gsm.print("AT+CMGS=\"");
  Gsm.print(phone_no);
  Gsm.println("\"");  
  delay(300);
  Gsm.print("http://maps.google.com/maps?q=loc:");
 
  // Gsm.print("Latitude = ");
  Gsm.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  
  //Gsm.print(" Longitude = ");
  Serial.print(",");
  Gsm.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  delay(200);
  Gsm.println((char)26); // End AT command with a ^Z, ASCII code 26
  delay(200);
  Gsm.println();
  delay(20000);
 }
}

void FINDME()
{
  bt.write("$");    //ring the phone
}  

void click()
{
  if(l==0)
  {
    x=1-k;
    y=k;
  }
  if(l==1)
  {
    x=1-k;
    y=1-k;
  }
}

void doubleclick()
{
   if(l==0)
  {
    x=k;
    y=1-k;
  }
    if(l==1)
  {
    x=k;
    y=k;
  }
}

void press()
{
  button.tick();
  if(x==1)
 {
  delay(100);
  button.tick();
  if(y==1)
   {
    digitalWrite(12,HIGH);
    bt.write("change the pattern");
   }
 }
}
