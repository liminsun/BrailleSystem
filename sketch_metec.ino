// include the library code:
#include <MsTimer2.h>

const int CLOCK = 2;
const int STROBE = 3;
const int DATA   = 4;

const int TRIGGER1 = 8;
const int TRIGGER2 = 9;
const int TRIGGER3 = 10;
const int TRIGGER4 = 11;
const int TRIGGER5 = 12;


// for communication with labview
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// Stimuli Setup
int StimType = 1; // 0 -- transient ; 1 -- steady state ; 2 -- vibrotactile
int Pinup = 10;
int Jitter =100;
int VibroCycles = 20;
int ISI = 1200;
int Trials = 200;
int CountTick = 0;
int tmpISI =1200;
int nTri = 0;

boolean Active [5] ={true,true,true,true,true};
int Freqs [5] = { 20, 50, 70, 80 ,110 };
int interval [5] = {500,200,143,125,91};
int tickCount [5] = {0,0,0,0,0};
int PinupTick[5] = {10,10,10,10,10};
boolean StartCountPinUpTick[5] = {false,false,false,false,false};
int CycleCount [5] = {0,0,0,0,0};
boolean stopping[5] = {false,false,false,false,false};
int VibroTactile_Trials[5] = {200,200,200,200,200};      
int CountTick_V[5] = {0,0,0,0,0};
int tmpISI_V[5] = {1200,1200,1200,1200,1200}; 
 
// braille cell control
boolean Work = false;
byte p16 [5];
byte p16stat [5] = {255,255,255,255,255};

boolean ReadySend = false;

void setup() {
  pinMode(TRIGGER1,OUTPUT);
  pinMode(TRIGGER2,OUTPUT);
  pinMode(TRIGGER3,OUTPUT);
  pinMode(TRIGGER4,OUTPUT);
  pinMode(TRIGGER5,OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
      delay(100); // wait for serial port to connect. Needed for native USB port only
  }
  inputString.reserve(100);
  
  pinMode(CLOCK, OUTPUT);
  pinMode(STROBE,OUTPUT);
  pinMode(DATA,  OUTPUT);
  
  //establishContact();  // send a byte to establish contact until receiver responds
   MsTimer2::set(1, flash); 
   MsTimer2::start();

//   Work = true;
}

void Send()
{
  for (int i=sizeof(p16)-1;i>=0;i--)
  {
    if(bitRead(p16[i],6) ){digitalWrite(DATA,1);} else {digitalWrite(DATA,0);}  digitalWrite(CLOCK,1);digitalWrite(CLOCK,0);
    if(bitRead(p16[i],2) ){digitalWrite(DATA,1);} else {digitalWrite(DATA,0);}  digitalWrite(CLOCK,1);digitalWrite(CLOCK,0);
    if(bitRead(p16[i],1) ){digitalWrite(DATA,1);} else {digitalWrite(DATA,0);}  digitalWrite(CLOCK,1);digitalWrite(CLOCK,0);
    if(bitRead(p16[i],0) ){digitalWrite(DATA,1);} else {digitalWrite(DATA,0);}  digitalWrite(CLOCK,1);digitalWrite(CLOCK,0);
    if(bitRead(p16[i],7) ){digitalWrite(DATA,1);} else {digitalWrite(DATA,0);}  digitalWrite(CLOCK,1);digitalWrite(CLOCK,0);
    if(bitRead(p16[i],5) ){digitalWrite(DATA,1);} else {digitalWrite(DATA,0);}  digitalWrite(CLOCK,1);digitalWrite(CLOCK,0);
    if(bitRead(p16[i],4) ){digitalWrite(DATA,1);} else {digitalWrite(DATA,0);}  digitalWrite(CLOCK,1);digitalWrite(CLOCK,0);
    if(bitRead(p16[i],3) ){digitalWrite(DATA,1);} else {digitalWrite(DATA,0);}  digitalWrite(CLOCK,1);digitalWrite(CLOCK,0);    
  }
  digitalWrite(STROBE,1);
  digitalWrite(STROBE,0);
}

void flash() { 
  if (Work) {    
    if (StimType == 0){ //transient   
      if (CountTick == 0){     
        for (int k=0;k<5;k++){
          if (Active[k]){
            p16[k] = p16stat[k]; 
          }       
        }
        digitalWrite(TRIGGER5, HIGH);
        Send();
        digitalWrite(TRIGGER5, LOW);
        
              
        CountTick = 1;
        tmpISI = ISI + random(-Jitter,Jitter);
        nTri ++;
      }
      else {
        if(CountTick == Pinup ) {
          for (int k=0;k<5;k++){
            if (Active[k]){
               p16[k] = 0;    
            } 
          }
          Send();           
        }     
        CountTick ++;     
      }
      if(CountTick >= tmpISI){
          CountTick = 0;
      }
    }
    
    if (StimType == 1) {
     // steady state
        for(int i=0; i<5;i++){
          if (Active[i]){
            tickCount[i] ++;
            if (tickCount[i] == interval[i]){
              tickCount[i] = 0;
              CycleCount[i] ++;      
              p16[i] = p16stat[i];
              StartCountPinUpTick[i] = true;
              if (i==0){
                digitalWrite(TRIGGER1, HIGH);
              }
              if (i==1){
                digitalWrite(TRIGGER2, HIGH);
              }
              if (i==2){
                digitalWrite(TRIGGER3, HIGH);                
              }
              if (i==3){
                digitalWrite(TRIGGER4, HIGH);
             }
              if (i==4){
                digitalWrite(TRIGGER5, HIGH);
             }              
            }
            if (StartCountPinUpTick[i]){
              PinupTick[i] --;
              if(PinupTick[i] <= 0){
                p16[i] = 0;
                PinupTick[i] = Pinup;
                StartCountPinUpTick[i] = false;                          
                if (i==0){
                  digitalWrite(TRIGGER1, LOW);
                }
                if (i==1){
                  digitalWrite(TRIGGER2, LOW);
                }
                if (i==2){
                  digitalWrite(TRIGGER3, LOW);                
                }
                if (i==3){
                  digitalWrite(TRIGGER4, LOW);
               }
                if (i==4){
                  digitalWrite(TRIGGER5, LOW);
               }              
              }
            }
          }                 
        }
        Send();  
    } 
    if (StimType == 2) {
     // vibrotactile
       for(int i=0; i<5;i++){
          if (Active[i]){//for each cell
             if (CountTick_V[i] == 0){  //start point of one vibratactile
                CountTick_V[i] ++;
                tmpISI_V[i] = ISI + random(-Jitter,Jitter);
                CycleCount[i] ++;
                tickCount[i] = 0;
              }
              else{//steady state in one vibratactile
                if (CountTick_V[i] < VibroCycles*interval[i] )
                {
                  tickCount[i] ++;
                  if (tickCount[i] == interval[i]){
                    tickCount[i] = 0;
                    p16[i] = p16stat[i];
                    StartCountPinUpTick[i] = true;
              
                    if (i==0){digitalWrite(TRIGGER1, HIGH); }
                    if (i==1){digitalWrite(TRIGGER2, HIGH); }
                    if (i==2){digitalWrite(TRIGGER3, HIGH); }
                    if (i==3){digitalWrite(TRIGGER4, HIGH); }
                    if (i==4){digitalWrite(TRIGGER5, HIGH); }              
                  }
                  if (StartCountPinUpTick[i]){
                    PinupTick[i] --;
                    if(PinupTick[i] <= 0){
                      p16[i] = 0;
                      PinupTick[i] = Pinup;
                      StartCountPinUpTick[i] = false;       
                    if (i==0){digitalWrite(TRIGGER1, LOW);}
                    if (i==1){digitalWrite(TRIGGER2, LOW);}
                    if (i==2){digitalWrite(TRIGGER3, LOW);}
                    if (i==3){digitalWrite(TRIGGER4, LOW);}
                    if (i==4){digitalWrite(TRIGGER5, LOW);}              
                                         
                    }
                  } 
                }
                CountTick_V[i] ++; 
                
              }
              
              if(CountTick_V[i] >= tmpISI_V[i]+VibroCycles*interval[i]){ // stop one vibratactile
                CountTick_V[i] = 0;
              }
            
          }//active
       }// for loop: all cells
       Send();
    }// if stim type
  }//if work
} //end of function    

void ParseSetupPara(){
  if (inputString[2]=='0') StimType = 0;
  else if (inputString[2]=='1')StimType = 1;
  else if (inputString[2]=='2')StimType = 2;
  
  if (inputString[3] == 'P') {
    Pinup = inputString.substring(4,7).toInt();        
  }
  if (inputString[7] == 'J') {
    Jitter = inputString.substring(8,12).toInt();  
  }
  if (inputString[12] == 'V') {
    VibroCycles = inputString.substring(13,17).toInt();
  }
  
}
      
void procCommand(){
  if (stringComplete) {
    Serial.println(inputString+"\n");
    char inChar = inputString[0];
    if(inChar == '1') { 
      Work = true; 
      Serial.println("START");
      for (int i=0; i<5;i++){
        interval[i] = 10000/Freqs[i];
        tickCount[i] = 0;
        CycleCount[i] = 0;
        PinupTick[i] = Pinup;
        StartCountPinUpTick[i] = false;
      }
    }
    else if(inChar == '2') {
      Work = false; 
      Serial.println("STOP");
    }
    else if(inChar == 'S') {
      //parse the stimuli setup parameters
      ParseSetupPara();   
      Work = false; 
    }
    else if (inChar == 'M'){
      if (inputString[1]=='0') {StimType = 0;}
      else if (inputString[1]=='1') {StimType = 1;}
      else if (inputString[1]=='2') {StimType = 2;}
    }
    else if (inChar == 'P'){
      Pinup  = inputString.substring(1,4).toInt();
    }
    else if (inChar == 'J'){
      Jitter = inputString.substring(1,5).toInt();
    }
    else if (inChar == 'V'){
      VibroCycles = inputString.substring(1,5).toInt();
    }
    else if (inChar == 'I'){
      ISI = inputString.substring(1,5).toInt();
    }
    else if (inChar == 'N'){
      Trials = inputString.substring(1,5).toInt();
       for (int i=0;i<5; i++){
           VibroTactile_Trials[i] = Trials;
       }
    }
    else if (inChar == 'F'){
      if (inputString[1] == '1'){
          int indx = 0;
          p16stat[indx] = 0;
          if(inputString[2] == '1') p16stat[indx] += 1;
          if(inputString[3] == '1') p16stat[indx] += 8; 
          if(inputString[4] == '1') p16stat[indx] += 2; 
          if(inputString[5] == '1') p16stat[indx] += 16; 
          if(inputString[6] == '1') p16stat[indx] += 4; 
          if(inputString[7] == '1') p16stat[indx] += 32; 
          if(inputString[8] == '1') p16stat[indx] += 64; 
          if(inputString[9] == '1') p16stat[indx] += 128;
      }   
      if (inputString[1] == '2'){
          int indx = 1;
          p16stat[indx] = 0;
          if(inputString[2] == '1') p16stat[indx] += 1;
          if(inputString[3] == '1') p16stat[indx] += 8; 
          if(inputString[4] == '1') p16stat[indx] += 2; 
          if(inputString[5] == '1') p16stat[indx] += 16; 
          if(inputString[6] == '1') p16stat[indx] += 4; 
          if(inputString[7] == '1') p16stat[indx] += 32; 
          if(inputString[8] == '1') p16stat[indx] += 64; 
          if(inputString[9] == '1') p16stat[indx] += 128;
      }     
      if (inputString[1] == '3'){
          int indx = 2;
          p16stat[indx] = 0;
          if(inputString[2] == '1') p16stat[indx] += 1;
          if(inputString[3] == '1') p16stat[indx] += 8; 
          if(inputString[4] == '1') p16stat[indx] += 2; 
          if(inputString[5] == '1') p16stat[indx] += 16; 
          if(inputString[6] == '1') p16stat[indx] += 4; 
          if(inputString[7] == '1') p16stat[indx] += 32; 
          if(inputString[8] == '1') p16stat[indx] += 64; 
          if(inputString[9] == '1') p16stat[indx] += 128;
      }     
      if (inputString[1] == '4'){
          int indx = 3;
          p16stat[indx] = 0;
          if(inputString[2] == '1') p16stat[indx] += 1;
          if(inputString[3] == '1') p16stat[indx] += 8; 
          if(inputString[4] == '1') p16stat[indx] += 2; 
          if(inputString[5] == '1') p16stat[indx] += 16; 
          if(inputString[6] == '1') p16stat[indx] += 4; 
          if(inputString[7] == '1') p16stat[indx] += 32; 
          if(inputString[8] == '1') p16stat[indx] += 64; 
          if(inputString[9] == '1') p16stat[indx] += 128;
      }     
      if (inputString[1] == '5'){
          int indx = 4;
          p16stat[indx] = 0;
          if(inputString[2] == '1') p16stat[indx] += 1;
          if(inputString[3] == '1') p16stat[indx] += 8; 
          if(inputString[4] == '1') p16stat[indx] += 2; 
          if(inputString[5] == '1') p16stat[indx] += 16; 
          if(inputString[6] == '1') p16stat[indx] += 4; 
          if(inputString[7] == '1') p16stat[indx] += 32; 
          if(inputString[8] == '1') p16stat[indx] += 64; 
          if(inputString[9] == '1') p16stat[indx] += 128;
      }     
              
    }
    else if (inChar == 'C'){
      if (inputString[1] == '1'){
         Freqs[0] = inputString.substring(2,5).toInt(); 
         interval[0] = 10000/Freqs[0];
         tickCount[0] = 0;
         PinupTick[0] = Pinup;
         StartCountPinUpTick[0] = false;
      }
      if (inputString[1] == '2'){
         Freqs[1] = inputString.substring(2,5).toInt(); 
         interval[1] = 10000/Freqs[1];
         tickCount[1] = 0;
         PinupTick[1] = Pinup;
         StartCountPinUpTick[1] = false;
      }
      if (inputString[1] == '3'){
         Freqs[2] = inputString.substring(2,5).toInt(); 
         interval[2] = 10000/Freqs[2];
         tickCount[2] = 0;
         PinupTick[2] = Pinup;
         StartCountPinUpTick[2] = false;
      }
      if (inputString[1] == '4'){
         Freqs[3] = inputString.substring(2,5).toInt(); 
         interval[3] = 10000/Freqs[3];
         tickCount[3] = 0;
         PinupTick[3] = Pinup;
         StartCountPinUpTick[3] = false;
      }
      if (inputString[1] == '5'){
         Freqs[4] = inputString.substring(2,5).toInt(); 
         interval[4] = 10000/Freqs[4];
         tickCount[4] = 0;
         PinupTick[4] = Pinup;
         StartCountPinUpTick[4] = false;
      }     
      Serial.println(Freqs[0]); 
    }
    else if (inChar == 'A'){
      //setup active
      if (inputString[1] == '1'){
         if (inputString[2] == '1') {Active[0] = true;  }
         else {Active[0] = false; }
      }
      else if (inputString[1] == '2'){
         if (inputString[2] == '1') {Active[1] = true; }
         else {Active[1] = false; }
      }
      else if (inputString[1] == '3'){
         if (inputString[2] == '1') {Active[2] = true; }
         else {Active[2] = false;}
      }
      else if (inputString[1] == '4'){
         if (inputString[2] == '1') {Active[3] = true; }
         else { Active[3] = false; }
      }
      else if (inputString[1] == '5'){
         if (inputString[2] == '1') {Active[4] = true; }
         else { Active[4] = false; }
      }
    }
    else if (inChar == 'Q') {
      char rt [100];
      int Acti[5];
      for (int i=0;i<5; i++){
        if (Active[i]) Acti[i]= 1;
        else Acti[i] = 0;
      }
      sprintf(rt, "Conf%1d%2d%3d%2d%4d%3d,%03d%03d%03d%03d%03d,%1d%1d%1d%1d%1d\n",
                  StimType,Pinup,Jitter,VibroCycles,ISI,Trials,
                  Freqs[0],Freqs[1],Freqs[2],Freqs[3],Freqs[4],
                  Acti[0],Acti[1],Acti[2],Acti[3],Acti[4]);      
      Serial.println(rt); 
    };
     
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

}




void readCommand()
{
   while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void parseCellCfg(String scell) {
  if (scell[0] == 'C'){
    int indx = (int) scell[1] - 1;
    if (indx >= 0){      
      Freqs[indx] = scell.substring(2,5).toInt(); 
      p16stat[indx] = 0;
      if(scell[5] == '1') p16stat[indx] += 1;
      if(scell[6] == '1') p16stat[indx] += 8; 
      if(scell[7] == '1') p16stat[indx] += 2; 
      if(scell[8] == '1') p16stat[indx] += 16; 
      if(scell[9] == '1') p16stat[indx] += 4; 
      if(scell[10] == '1') p16stat[indx] += 32; 
      if(scell[11] == '1') p16stat[indx] += 64; 
      if(scell[12] == '1') p16stat[indx] += 128; 
    }
  }
}
void loop() {
  // print the string when a newline arrives:
  readCommand(); // read command from serial port
  procCommand();
  
  // process
  if (Work){
    if (Active[0]||Active[1]||Active[2]||Active[3]||Active[4]){
      for (int i=0; i<5; i++) {
        if (CycleCount[i] > Trials || Active[i]==false) {
          stopping[i] = true;
        }
        else{
          stopping[i] = false;        
        }
      }
      
      if ((stopping[0] && stopping[1] && stopping[2] && stopping[3] && stopping[4])|| (nTri > Trials)){
        Work = false;
        Serial.println("Stop\n");
      }    
    }
  }
}

