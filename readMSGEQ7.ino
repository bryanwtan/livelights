#include <RunningMedian.h>

// # of distinct lights, MSGEQ7 supports 7 frequency bands
const byte bands = 5;

// jumper cable locations
const byte analogPin = 0,
    strobePin = 12,
    resetPin = 13;

const byte red = 3,
    green = 5,
    blue = 6,
    yellow = 9,
    white = 10;

// hold analog to digital values
int bandPass[bands];

// transistor pins
const byte led[bands] = {
    red,
    green,
    blue,
    yellow,
    white
};

// minimum voltage for flash (0 - 5 volts get mapped to 0 - 1023)
const byte vMin = 150,
    sampleSize = 50;
// volume cutoffs for brighter flashes
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
    
    // flash brighter at higher voltages
    for(int i = 0; i < 5; i++){
        v1[i] = vMin;
        v2[i] = 2*v1[i];
        v3[i] = 3*v1[i];
    }
}

void loop(){
    // start from the lowest frequency band
    digitalWrite(resetPin, HIGH); digitalWrite(resetPin, LOW);
    delayMicroseconds(75);
  
    for(int i = 0; i < 5; i++){
        // strobe switches between preset frequency bands
        digitalWrite(strobePin, LOW);
        
        // allow output settle, officially 36
        delayMicroseconds(40);            
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
        // voltages below the first cutoff turn the light off
        if(bandPass[i] <= v1[i]){ digitalWrite(led[i], LOW); }
        else if(bandPass[i] <= v2[i]){ analogWrite(led[i], 1); }
        else if(bandPass[i] <= v3[i]){ analogWrite(led[i], 50); }
        else{ digitalWrite(led[i], HIGH); }
    } 
}
