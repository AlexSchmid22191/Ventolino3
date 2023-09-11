//
// Created by Alex on 30/10/2018.
//

//Class for interfacing an Arduino with the MAX525 4 channel 12-bit analog to digital converter.

#ifndef MAX5250_H
#define MAX5250_H

#include "Arduino.h"

class MAX5250
{
public:
    static const byte CHA;
    static const byte CHB;
    static const byte CHC;
    static const byte CHD;

    static const byte LOAD_NC;
    static const byte LOAD_UD;

    static const byte UP_LOW;
    static const byte UP_HIGH;

    MAX5250(byte CS, byte CL, byte PDL, float V_ref, float gain);

    void begin();
    void write_DAC(byte channel, float voltage, byte update);
    void set_UPO(byte state);
    void power_down();
    void update_all();
    void write_all(float voltage);
    void clear_all();

private:
    float _VREF;
    float _GAIN;

    //Chip select, clear and power down pins
    byte _CS_PIN;
    byte _CL_PIN;
    byte _PDL_PIN;

    //Bit masks for control byte
    static const byte _ADDR;
    static const byte _CTRL;
    static const byte _DATA;

    uint16_t calc_codec(float voltage);
public:
    void setVref(float vref);
    void setGain(float gain);
};

#endif //MAX525_H
