#ifndef ENDEAVOURRAWFTDI_H
#define ENDEAVOURRAWFTDI_H

#include <memory>

#include "DeviceCom.h"
#include "EndeavourRaw.h"

class EndeavourRawFTDI : public EndeavourRaw
{
public:
  EndeavourRawFTDI();
  ~EndeavourRawFTDI();

  void setDitMin(uint32_t DIT_MIN);
  uint32_t getDitMin();

  void setDitMid(uint32_t DIT_MID);
  uint32_t getDitMid();

  void setDitMax(uint32_t DIT_MAX);  
  uint32_t getDitMax();

  void setDahMin(uint32_t DAH_MIN);
  uint32_t getDahMin();

  void setDahMid(uint32_t DAH_MID);
  uint32_t getDahMid();

  void setDahMax(uint32_t DAH_MAX);  
  uint32_t getDahMax();

  void setBitGapMin(uint32_t BITGAP_MIN);
  uint32_t getBitGapMin();

  void setBitGapMid(uint32_t BITGAP_MID);
  uint32_t getBitGapMid();

  void setBitGapMax(uint32_t BITGAP_MAX);  
  uint32_t getBitGapMax();
  
  void reset();

  void sendData(unsigned long long int data, unsigned int size);

  bool isError();
  bool isDataValid();
  void readData(unsigned long long int& data, unsigned int& size);

private:
  uint32_t m_DIT_MIN   =  6*30/40,m_DIT_MID   = 14*30/40,m_DIT_MAX   = 22*30/40;
  uint32_t m_DAH_MIN   = 29*30/40,m_DAH_MID   = 76*30/40,m_DAH_MAX   =124*30/40;
  uint32_t m_BITGAP_MIN= 11*30/40,m_BITGAP_MID= 43*30/40,m_BITGAP_MAX= 75*30/40;

  struct ftdi_context *m_ftdi;

  // Last returned data
  unsigned long long int m_readData;
  unsigned int m_readSize;

  // read all requested data
  void ftdi_read_alldata(std::vector<uint8_t>& data, uint32_t requested);
};

#endif //ENDEAVOURRAWFTDI_H
