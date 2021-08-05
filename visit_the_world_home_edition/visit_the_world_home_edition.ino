
https://github.com/LiquidGalaxy/liquid-galaxy
https://github.com/LiquidGalaxyLAB/liquid-galaxy
https://github.com/LiquidGalaxyLAB/Arduino-Controller
AINHOA MAESTRO GUARDIOLA - GSoC 2021
-------------------------------------------------------------
This program should only be used when connected to the PC Master Liquid Galaxy, using Serial communication via the USB cable.
This code does not allow use with WiFi network
----------------------------------------------------------
#include <LiquidGalaxyController.h>
#include <SoftwareSerial.h>
%%--Voice Commands
SoftwareSerial VR3(22,23);
byte a[27];
int rec =18;
int _pinOut = 18;
int received;
byte headVoice[4]={0xAA,0x02,0x31,0x0A};
byte groups[4] [11]={
      {0xAA,0x09,0x30,0x01,0x02,0x03,0x04,0x12,0x13,0x14,0x0A}, // First Group
      {0xAA,0x09,0x30,0x05,0x06,0x07,0x08,0x11,0x13,0x14,0x0A}, // Second Group 
      {0xAA,0x09,0x30,0x09,0x0A,0x0B,0x0C,0x11,0x12,0x14,0x0A},  
      {0xAA,0x09,0x30,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x0A}};
%%---LiquidGalaxyController parameters
const byte NumberRows = 4;
const byte NumberColuns = 4;
byte line[NumberRows] ={13,12,14,27}; 
byte column[NumberColuns]={26,25,33,32};
int Keys[NumberRows*NumberColuns] = 
  {1, 2, 3, 4,
   5, 6, 7, 8,
   9,10,11,12,
  13,14,15,16};
LG_Keypad LGKey(Keys,line,column,NumberRows,NumberColuns);
%%-------Default commands and starting places
String Commands[]={"zero","linear","zOut","zIn","right","left","up","down","rightUp","rightDown","leftUp","leftDown",
                   "CamUp","CamDown","CamRight","CamLeft","rollRight","rollLeft","tiltUp","tiltDown"};

String Coordnates[16][3]={
  {"-122.485046","37.820047","3000"},{"78.042202","27.172969","1500"},{"-43.210317","-22.951838","400"},{"-88.567935","20.683510","600"},{"12.492135","41.890079","600"},
  {"-72.545224","-13.163820","600"},{"35.441919","30.328456","600"},{"2.294473","48.857730","1000"},{"-0.124419","51.500769","500"},{"-74.044535","40.689437","500"},
  {"37.623446","55.752362","500"},{"-73.985359","40.748360","500"},{"-51.049260","0.030478","500"},{"31.132695","29.976603","500"},{"0.626502","41.617540","600"},{"116.562771","40.435456","500"}
  };

String CoordOrbit[3];
%%--------Control variables
int moviment, moviment2,moviment3 =0;
int movJoy =1;
int tourPin = 2;
int OrbitPin = 15;
int changeVoiceGroup = 5;
int TimeTour = 10;

%%-----Default commands and starting places
void setup() 
{
  Serial.begin(9600);
  VR3.begin(9600);
  pinMode(OrbitPin, INPUT);
  pinMode(tourPin, INPUT);
  pinMode(changeVoiceGroup, INPUT);
  pinMode(_pinOut,OUTPUT); 
  digitalWrite(_pinOut,LOW);
  delay(1000);
  for(int h =0; h<11;h++)VR3.write(groups[0] [h]);
  delay(100);
 for(int i = 0; i<11;i++)
 {
  a[i] = VR3.read();
  Serial.print( a[i],HEX);
  Serial.print("-");
  delay(1); 
 }
 Serial.println("");
 delay(100);
      VR3.write(0xaa);
      VR3.write(0x0D);
      VR3.write(0x0a);
}
void loop() 
{   
       %%--------------------------
    if(digitalRead(OrbitPin)){while(digitalRead(OrbitPin)){} MakeOrbit();} // Start the Orbit
    if(digitalRead(tourPin)){while(digitalRead(tourPin)){} Tour(TimeTour);} // Start and stop the tour
   %%------------------------------------
    if(digitalRead(changeVoiceGroup))
    { while(digitalRead(changeVoiceGroup) == HIGH){}  // Change the voice group after press the button
      GroupsControll(rec);
      rec ++;
      if(rec == 21)rec =17;
    }
  %%----------------------- Function to receive the voice recognition command and send the kml corresponfing
    received = GetRec();
    if (received <17 && received>0)
    {
      Serial.println(MakeKML(Coordnates[received-1][0],Coordnates[received-1][1],Coordnates[received-1][2]));
    }
 
   }

  %%----------------------------------
   int distance = Ultrasonic.UltrasonicMensure();// ULTRASONIC FUNCTION, check the distance and apply zoom In or zoom out
   if(distance < 10) 
   { 
    moviment2 =1;
    LGMove(3);
   } else {
    if(moviment2==1)
    { 
      LGMove(0);
      LGMove(0);
      moviment2=0;
    }
    if(distance < 25 && distance > 15) 
   { 
    moviment3 =1;
    LGMove(2);
   } else  if(moviment3==1)
            { 
              LGMove(0);
              LGMove(0);
              moviment3=0;
            } 
   } 
}%% End loop

%%-----------------This function is responsable for building the simple kml
String MakeKML(String longitude, String latitude, String range)
{
  CoordOrbit[0] = longitude;  
  CoordOrbit[1] = latitude; 
  CoordOrbit[2]= range;
  String kml ="flytoview=<LookAt><longitude>";
  kml += longitude;
  kml += "</longitude><latitude>";
  kml +=latitude ;
  kml += "</latitude><range>";
  kml += range;
  kml += "</range></LookAt>";
  return kml;
}
%%---------------Function to perform an orbit
void MakeOrbit()
{
  bool Step = true;
  digitalWrite(_pinOut,HIGH);
  String kmlOrbit = "";
   for(int g =0; g<361; g ++)
    {
      kmlOrbit = "";
      kmlOrbit = "flytoview=<LookAt><longitude>";
      kmlOrbit += CoordOrbit[0];
      kmlOrbit += "</longitude><latitude>";
      kmlOrbit += CoordOrbit[1];
      kmlOrbit += "</latitude><heading>";
      kmlOrbit += String(g);
      kmlOrbit += "</heading><range>";
      kmlOrbit += CoordOrbit[2];
      kmlOrbit += "</range><tilt>40</tilt></LookAt>";
      Serial.println(kmlOrbit);
      if(Step){Step = false; delay(8000);}
      if(digitalRead(OrbitPin)){while(digitalRead(OrbitPin)){} break;}
   }
    digitalWrite(_pinOut,LOW);
}

%%--------------Tour is responsable for send the kml's with the selected time by the user, creating the tour for 16 places
void Tour(int Time)
{
  digitalWrite(_pinOut,HIGH);
  while(digitalRead(tourPin)){}
  int Tb,Ta;
  for(int tour = 0 ;tour<16;tour++)
  {
    Tb = Ta = millis();
    Serial.println(MakeKML(Coordnates[tour][0],Coordnates[tour][1],Coordnates[tour][2]));
  
    while((Tb - Ta) <(Time*1000))
    { 
      if(digitalRead(tourPin)) tour =16;
      Tb = millis();
    }
  }
  digitalWrite(_pinOut,LOW);
}
%%---------GetRec receive the comands sent by the voice module and return the a[5], position corresponding to voice index
int GetRec()
{
    if(VR3.available())
   {
      for(int i = 0; i<27;i++)
     {
      a[i] = VR3.read();
      delay(1); 
     }
   return a[5];
  }
  return 0;
}
%%----roupsControll controls the voice groups and reports the status with a led
void GroupsControll(int rec)
{
  int h,t,p;
  delay(500);
  switch (rec)
  {
    case 17:// First Group
    digitalWrite(_pinOut,!digitalRead(_pinOut)); delay(500);
    digitalWrite(_pinOut,!digitalRead(_pinOut));
    for(t =0; t<4;t++)VR3.write(headVoice[t]);
    delay(500);
    for(h =0; h<11;h++)VR3.write(groups[0] [h]);
    break;
    case 18:// First Group
    for(p =0;p<4;p++){digitalWrite(_pinOut,!digitalRead(_pinOut)); delay(500);}
    for(t =0; t<4;t++)VR3.write(headVoice[t]);
    delay(500);
    for(h =0; h<11;h++)VR3.write(groups[1] [h]);
    break;
    case 19:// First Group
    for(p =0;p<6;p++){digitalWrite(_pinOut,!digitalRead(_pinOut)); delay(500);}
    for(t =0; t<4;t++)VR3.write(headVoice[t]);
    delay(500);
    for(h =0; h<11;h++)VR3.write(groups[2] [h]);
    break;
    case 20:// First Group
    for(p =0;p<8;p++){digitalWrite(_pinOut,!digitalRead(_pinOut)); delay(500);}
    for(t =0; t<4;t++)VR3.write(headVoice[t]);
    delay(500);
    for(h =0; h<11;h++)VR3.write(groups[3] [h]);
    break;
    default:
    break;
  } 
}
