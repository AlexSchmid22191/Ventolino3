//
// Created by Alex on 31/07/2023.
//
#include <Arduino.h>
#include <Display/display.h>
#include "SPI.h"
#include "MAX5250.h"

//Pin definitions
const byte DAC_CS = 10;
const byte DAC_CL = 8;
const byte DAC_PDL = 9;

const byte ADC_Pins[] = {A0, A2, A1, A3};
const byte Relais_Pins[] = {5, 3, 4, 2};

//Hardware defined paramteres
const float V_REF = 4.096;
const float GAIN = 1.25;
const float FULLRANGE = 5.0;

//MFC Input and Output values
float MFC_set[4] = {0.0, 0.0, 0.0, 0.0};
float MFC_is[4] = {0.0, 0.0, 0.0, 0.0,};

MAX5250 MAX_DAC(DAC_CS,DAC_CL,DAC_PDL, V_REF, GAIN);

float calc_set_voltage(float set_percentage);
float calc_is_percentage(int codec);
byte channel_idx(byte channel);

void read_ADC();
void set_DAC();
void listen_to_serial();

void setup()
{
    for(auto &pin : ADC_Pins)
    {
        pinMode(pin, INPUT);
    }

    for(auto pin : Relais_Pins)
    {
        pinMode(pin, INPUT);
    }

    MAX_DAC.begin();

    init_dispaly();
    Serial.begin(9600);
    Serial.setTimeout(500);
    SPIClass::begin();
}

void loop()
{
    read_ADC();
    listen_to_serial();
    set_DAC();
    update_display(MFC_set, MFC_is);
}

void set_DAC()
{
    for(uint8_t channel = 0; channel <= 3; channel++)
    {
        float voltage = calc_set_voltage(MFC_set[channel_idx(channel)]);
        MAX_DAC.write_DAC(channel, voltage, MAX5250::LOAD_UD);
    }
}

byte channel_idx(byte channel)
{
    switch(channel)
    {
        case 0:
            return 1;
        case 1:
            return 3;
        case 2:
            return 0;
        case 3:
            return 2;
    }
    return -1;
}

float calc_set_voltage(float set_percentage)
{
    return FULLRANGE / 100 * set_percentage;
}

void read_ADC()
{
    for(uint8_t channel = 0; channel <= 3; channel++)
    {
        int codec = analogRead(ADC_Pins[channel]);
        MFC_is[channel] = calc_is_percentage(codec);
    }
}

float calc_is_percentage(int codec)
{
    return (float)codec*100 / 1023;
}

void listen_to_serial()
{
    char commandBuffer[16];

    //Check if a serial communication is requested
    while((bool)Serial.available())
    {
        //Read bytes until the start character (0x02) is encountered
        int x = Serial.read();
        if(x == 0x02)
        {
            //Clear buffer
            memset(commandBuffer, 0, 16);
            //Read into buffer
            Serial.readBytesUntil(0x0D, commandBuffer, 14);

            //Replace commas with points
            char *ptr = &commandBuffer[0];
            while(ptr != nullptr)
            {
                ptr = strchr(ptr, ',');
                if(ptr != nullptr)
                {
                    *ptr = '.';
                }
            }

            //Parse for MFC channel
            char* ctrl_seq = nullptr;
            auto channel = (uint8_t)strtol(commandBuffer, &ctrl_seq, 0) - 1;

            //Check if channel is between 1 and 4
            if((0 <= channel) && (channel <= 3))
            {
                //Set MFC Flow Rate
                if(strncmp(ctrl_seq, "SFD", 3) == 0)
                {
                    float value = strtod(&commandBuffer[5], nullptr);

                    if((0.0 <= value) && (value <= 100.0))
                    {
                        MFC_set[channel] = value;
                    }
                    Serial.println("rec");
                }
                //Read set value
                else if(strncmp(ctrl_seq, "RFD", 3) == 0) {
                    Serial.print(MFC_set[channel]);
                    Serial.print("\n");
                }

                //Measure flow data
                else if(strncmp(ctrl_seq, "RFX", 3) == 0)
                {
                    Serial.print(MFC_is[channel]);
                    Serial.print("\n");
                }
            }
        }
    }
}