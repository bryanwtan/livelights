#include <RunningMedian.h>

const byte bands = 5;
const byte analogPin = 0, strobePin = 12, resetPin = 13;
const byte r = 3, g = 5, b = 6, y = 9, w = 10;

int bandPass[bands]; //hold a2d values
const byte led[bands] = {3, 5, 6, 9, 10};
const byte vMin = 150, sampleSize = 50;
int v1[bands], v2[bands], v3[bands];
RunningMedian vSamples[5] = RunningMedian(sampleSize);

void setup(){
    delay(1000);
    analogReference(DEFAULT); pinMode(analogPin, INPUT);
    pinMode(resetPin, OUTPUT); pinMode(strobePin, OUTPUT);
    for(int i = 0; i < bands; i++){
        pinMode(led[i], OUTPUT);
    }
    digitalWrite(resetPin, LOW); digitalWrite(strobePin, HIGH);
    for(int i = 0; i < 5; i++){
        v1[i] = vMin;
        v2[i] = 2*v1[i];
        v3[i] = 3*v1[i];
    }
}

void loop(){
    digitalWrite(resetPin, HIGH); digitalWrite(resetPin, LOW);
    delayMicroseconds(75);
  
    for(int i = 0; i < 5; i++){
        digitalWrite(strobePin, LOW);
        delayMicroseconds(40); // allow output settle, officially 36               
        bandPass[i] = analogRead(analogPin);
        digitalWrite(strobePin, HIGH);
        delayMicroseconds(40);
        
        vSamples[i].add(bandPass[i]);
        if (vSamples[i].getMedian() > vMin){ 
            v1[i] = 1.2*vSamples[i].getMedian();
            v2[i] = 1.5*v1[i];
            v3[i] = 2*v1[i];
        }
    }
    
    delay(40);  
    
    for(int i = 0; i < bands; i++){
        if(bandPass[i] <= v1[i]){ digitalWrite(led[i], LOW); }
        else if(bandPass[i] <= v2[i]){ analogWrite(led[i], 1); }
        else if(bandPass[i] <= v3[i]){ analogWrite(led[i], 50); }
        else{ digitalWrite(led[i], HIGH); }
    } 
}
