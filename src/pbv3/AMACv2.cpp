#include "AMACv2.h"

#include <unistd.h>

AMACv2::AMACv2(unsigned short amacid, std::unique_ptr<EndeavourRaw> raw)
  : EndeavourCom(amacid, std::move(raw)), AMACv2Reg()
{ }

AMACv2::~AMACv2(){}

void AMACv2::init()
{
  // Set AMAC ID
  EndeavourCom::setid(EndeavourCom::REFMODE::IDPads, 0x0);
  usleep(10);
}

void AMACv2::initRegisters()
{ 
  // Initialize registers with default values
  for(auto fieldKV : regMap)
    {
      if((this->*fieldKV.second).canBeWrittenField())
	(this->*fieldKV.second).writeDefaultVal();
    }

  // Write AMAC registers into the chip
  for(uint reg=0;reg<AMACv2Reg::numRegs;reg++)
    {
      EndeavourCom::write_reg(reg, getReg(reg));
      usleep(10);
    }
}

void AMACv2::syncReg(AMACv2Field AMACv2Reg::* ref)
{
  EndeavourCom::write_reg(getAddr(ref), (this->*ref).readRaw());
}

void AMACv2::wrField(AMACv2Field AMACv2Reg::* ref, uint32_t data)
{
  setField(ref, data);
  EndeavourCom::write_reg(getAddr(ref), (this->*ref).readRaw());
  usleep(1e4);
}

void AMACv2::wrField(const std::string& fieldName, uint32_t data)
{
  setField(fieldName, data);
  EndeavourCom::write_reg(getAddr(fieldName), getReg(fieldName));
  usleep(1e4);
}

uint32_t AMACv2::rdField(AMACv2Field AMACv2Reg::* ref)
{
  uint32_t ret = EndeavourCom::read_reg(getAddr(ref));  
  setReg(getAddr(ref), ret);
  return getField(ref);
}

uint32_t AMACv2::rdField(const std::string& fieldName)
{
  uint32_t ret = EndeavourCom::read_reg(getAddr(fieldName));  
  setReg(getAddr(fieldName), ret);
  return getField(fieldName);
}

void AMACv2::write_reg(unsigned int address, unsigned int data)
{
  setReg(address, data);
  EndeavourCom::write_reg(address, data);
  usleep(1e4);
}

void AMACv2::setADCslope (double ADCslope)
{ 
  m_ADCslope=ADCslope;
}

void AMACv2::setADCoffset(uint8_t ch, uint32_t counts)
{
  if(ch>=16) return;
  m_ADCoffset[ch]=counts;
}

double AMACv2::calibrateCounts(uint8_t ch, uint32_t counts)
{
  if(ch>=16) return 0.;

  return m_ADCslope*(counts-m_ADCoffset[ch]);
}

double AMACv2::getADC(uint8_t ch)
{
  if(ch>=16) return 0.;
  uint32_t counts=0;
  counts=rdField("Ch"+std::to_string(ch)+"Value");

  return calibrateCounts(ch, counts);
}

double AMACv2::getVDDREG()
{
  wrField(&AMACv2Reg::Ch3Mux, 0);
  uint32_t counts=rdField(&AMACv2Reg::Ch3Value);
  return calibrateCounts(3, counts)*3/2;
}

double AMACv2::getAM()
{
  wrField(&AMACv2Reg::Ch4Mux, 0);
  uint32_t counts=rdField(&AMACv2Reg::Ch4Value);
  return calibrateCounts(4, counts);
}

double AMACv2::getAM900()
{
  wrField(&AMACv2Reg::Ch3Mux, 2);
  uint32_t counts=rdField(&AMACv2Reg::Ch3Value);
  return calibrateCounts(3, counts);
}

double AMACv2::getNTCx()
{
  uint32_t counts=rdField(&AMACv2Reg::Ch7Value);
  return calibrateCounts(7, counts);
}

double AMACv2::getNTCy()
{
  uint32_t counts=rdField(&AMACv2Reg::Ch8Value);
  return calibrateCounts(8, counts);
}

double AMACv2::getNTCpb()
{
  uint32_t counts=rdField(&AMACv2Reg::Ch9Value);
  return calibrateCounts(9, counts);
}

double AMACv2::getCur10V()
{
  wrField(&AMACv2Reg::Ch12Mux , 0); // Read the out difference
  uint32_t counts=rdField(&AMACv2Reg::Ch12Value);
  return calibrateCounts(12, counts)/10.4;
}

double AMACv2::getCur1V()
{
  wrField(&AMACv2Reg::Ch13Mux , 0); // Read the out difference
  uint32_t counts=rdField(&AMACv2Reg::Ch13Value);
  return calibrateCounts(13, counts)/30.;
}