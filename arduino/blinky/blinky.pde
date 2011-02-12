#include <string.h>

/* Controls timing out when no data has been received */
#define TIMEOUT 4000
unsigned long lastCmdMS;
int timeoutEnabled = 1;

/* Maps Led indices [0,5] to their actual pin numbers */
const char ledPins[] = {3, 5, 6, 9, 10, 11};

/* Current output state of LEDs, assuming we haven't timed out */
#define LEDINIT 255
char ledStates[] = {LEDINIT, LEDINIT, LEDINIT, LEDINIT, LEDINIT, LEDINIT};
int  ledStateChanged = 0;

/* Should we echo chars back? */
int echoEnabled = 0;

/* Output a particular intensity on all LEDs */
void setLeds(int intensity);
/* Set all LEDs from an array of intensities, one intensity value per LED */
void setLeds(char* values);
/* Change the currently stored intensity for a given LED */
void setLedState(int led, int intensity);
/* LED driver.  
 * -Outputs LED states from the ledStates array when commands have been received recently
 * -Blinks LED 0 if no commands have been received. */
void updateLeds();
/* Handle a null terminated line of input from the console */
void handleLine(char* line);

void setup()
{
  Serial.begin(115200);
  
  for(int i=2; i<=13; ++i) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  
  lastCmdMS = millis();
  setLeds(255);
}

void loop()
{ 
    updateLeds(); 
  
    static char buf[32];
    static int  bufsz = 0;
  
    int ch = Serial.read();
    if(ch == -1) return;
    if(echoEnabled) Serial.write(ch);
    
    if( !bufsz ) {
       switch( ch ) {
       case '?':
           Serial.write("rgblinky\n");
           
       case '\r':
       case '\n':
           return;
       default:
           break;
       }
    }
   
    buf[bufsz++] = ch;
    buf[bufsz] = '\0';

    if(bufsz==32) bufsz = 0;
    
    if( ch == '\n' || ch == '\r' ) {
       handleLine(buf);
       bufsz = 0;
    }
    
}

void setLeds(char* values)
{
   for(int i=0; i<6; ++i) {
      analogWrite(ledPins[i], values[i]);
   } 
}

void setLeds(int intensity)
{
    for(int i=0; i<6; ++i) {
       analogWrite(ledPins[i], intensity); 
    }
}

void setLedState(int led, int intensity) 
{
   ledStates[led] = intensity;
   ledStateChanged = 1;
}

void updateLeds()
{
    static int timedOut = 1;
    unsigned long sinceLast = millis() - lastCmdMS;
    
    
    if( timedOut && (sinceLast < TIMEOUT) ) {
       timedOut = 0;
       setLeds(ledStates); 
    }
    
    if( !timedOut && (sinceLast >= TIMEOUT) && timeoutEnabled ) {
       setLeds(0);
       timedOut = 1; 
    }
    
    if( ledStateChanged && !timedOut ) {
       setLeds(ledStates);
       ledStateChanged = 0;
    }
    
    if( timedOut ) {
       if( (sinceLast/1000) & 1 ) {
          analogWrite(ledPins[0], 0); 
       } else {
          analogWrite(ledPins[0], 255); 
       }
    }
    
}

void handleLine(char* line) 
{
    lastCmdMS = millis();
    
    if(! strncmp("led", line, 3) ) {
       int led = line[3] - '0';
       if( (led<0) || (led>=6) ) return;
       
       int intensity = atoi(line+5);
    
       setLedState(led, intensity);
    } else if(! strncmp(line, "echo on", 7) ) {
       echoEnabled = 1;
    } else if(! strncmp(line, "echo off", 8) ) {
       echoEnabled = 0; 
    } else if(! strncmp(line, "red on", 6) ) {
       digitalWrite(13, HIGH); 
    } else if(! strncmp(line, "red off", 7) ) {
       digitalWrite(13, LOW); 
    } else if(! strncmp(line, "yellow on", 9) ) {
       digitalWrite(12, HIGH); 
    } else if(! strncmp(line, "yellow off", 10) ) {
       digitalWrite(12, LOW); 
    } else if(! strncmp(line, "timeout on", 10) ) {
       timeoutEnabled = 1; 
    } else if(! strncmp(line, "timeout off", 11) ) {
       timeoutEnabled = 0; 
    }
    
}
