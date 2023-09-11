//
// Created by Alex on 30/10/2018.
//

#include "Arduino.h"
#include "MAX5250.h"
#include "SPI.h"

const byte MAX5250::CHA = 0;
const byte MAX5250::CHB = 1;
const byte MAX5250::CHC = 2;
const byte MAX5250::CHD = 3;

const byte MAX5250::LOAD_NC = 1;
const byte MAX5250::LOAD_UD = 3;

const byte MAX5250::UP_LOW = 0;
const byte MAX5250::UP_HIGH = 1;

const byte MAX5250::_ADDR = 14;
const byte MAX5250::_CTRL = 12;
const byte MAX5250::_DATA = 2;



MAX5250::MAX5250(byte CS, byte CL, byte PDL, float V_ref, float gain)
{
    _CS_PIN = CS;
    _CL_PIN = CL;
    _PDL_PIN = PDL;
    _VREF = V_ref;
    _GAIN = gain;
}

void MAX5250::begin()
{
    SPI.begin();
    pinMode(_CS_PIN, OUTPUT);
    digitalWrite(_CS_PIN, HIGH);

    pinMode(_CL_PIN, OUTPUT);
    digitalWrite(_CL_PIN, HIGH);

    pinMode(_PDL_PIN, OUTPUT);
    digitalWrite(_PDL_PIN, HIGH);
}

void MAX5250::set_UPO(byte state)
{
    //Control word
    uint16_t ctrl = (uint16_t) 0 | (state << MAX5250::_ADDR) | (2 << MAX5250::_CTRL);

    //Send control word
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    digitalWrite(_CS_PIN, LOW);
    SPI.transfer16(ctrl);
    digitalWrite(_CS_PIN, HIGH);
    SPI.endTransaction();
}

void MAX5250::write_DAC(byte channel, float voltage, byte update)
{
    //DAC code
    uint16_t code = calc_codec(voltage);

    //Control word
    uint16_t ctrl = 0 | (channel << MAX5250::_ADDR) | (update << MAX5250::_CTRL) | (code << MAX5250::_DATA) ;

    //Send control word
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    digitalWrite(_CS_PIN, LOW);
    SPI.transfer16(ctrl);
    digitalWrite(_CS_PIN, HIGH);
    SPI.endTransaction();
}

void MAX5250::power_down()
{
    //Control word
    uint16_t ctrl = (uint16_t) 0u | (3u << MAX5250::_ADDR) | (0u<< MAX5250::_CTRL);

    //Send control word
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    digitalWrite(_CS_PIN, LOW);
    SPI.transfer16(ctrl);
    digitalWrite(_CS_PIN, HIGH);
    SPI.endTransaction();
}

void MAX5250::update_all()
{
    //Control word
    uint16_t ctrl = (uint16_t) 0u | (1u << MAX5250::_ADDR) | (0u<< MAX5250::_CTRL);

    //Send control word
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    digitalWrite(_CS_PIN, LOW);
    SPI.transfer16(ctrl);
    digitalWrite(_CS_PIN, HIGH);
    SPI.endTransaction();
}

void MAX5250::write_all(float voltage)
{
    //DAC codec
    uint16_t codec = calc_codec(voltage);

    //Control word
    uint16_t ctrl = (uint16_t) 0 | (2u << MAX5250::_ADDR) | (0u<< MAX5250::_CTRL) | (codec << MAX5250::_DATA);

    //Send control word
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    digitalWrite(_CS_PIN, LOW);
    SPI.transfer16(ctrl);
    digitalWrite(_CS_PIN, HIGH);
    SPI.endTransaction();
}

void MAX5250::clear_all()
{
    digitalWrite(_CL_PIN, LOW);
    digitalWrite(_CL_PIN, HIGH);
}

uint16_t MAX5250::calc_codec(float voltage)
{
    return (uint16_t) (voltage / _VREF / _GAIN * 1023);
}

void MAX5250::setVref(float vref)
{
    _VREF = vref;
}

void MAX5250::setGain(float gain)
{
    _GAIN = gain;
}