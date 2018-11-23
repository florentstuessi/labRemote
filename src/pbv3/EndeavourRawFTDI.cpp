#include "EndeavourRawFTDI.h"

#include "EndeavourComException.h"

#include "MCP4801.h"
#include "MAX11619.h"

#include "ftdihelp.h"

#include <libftdi1/ftdi.h>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <stdint.h>

#define CHUNK_SIZE 65535
#define LATENCY_MS 4
#define USB_TIMEOUT 120000


EndeavourRawFTDI::EndeavourRawFTDI()
{
  int ret;
  if((m_ftdi=ftdi_new())==0)
    throw EndeavourComException("Unable to open FTDI");

  if((ret=ftdi_usb_open(m_ftdi,0x0403,0x6014))<0)
    throw EndeavourComException("Unable to find FTDI device: " + std::string(ftdi_get_error_string(m_ftdi)));

  //
  // Configure device for MPSSE
  
  // Reset USB device
  ftdi_usb_reset(m_ftdi);
  
  // Set USB request transfer sizes to 64K
  ftdi_write_data_set_chunksize(m_ftdi, CHUNK_SIZE);
  ftdi_read_data_set_chunksize (m_ftdi, CHUNK_SIZE);

  // Disable event and error characters
  ftdi_set_event_char(m_ftdi, 0, false);
  ftdi_set_error_char(m_ftdi, 0, false);

  // Sets the read and write timeouts in milliseconds
  m_ftdi->usb_read_timeout  = USB_TIMEOUT;
  m_ftdi->usb_write_timeout = USB_TIMEOUT;

  // Set the latency timer to 1mS (default is 16mS)
  ftdi_set_latency_timer(m_ftdi, LATENCY_MS);

  // Turn on flow control to synchronize IN requests
  ftdi_setflowctrl(m_ftdi, SIO_RTS_CTS_HS);

  // Reset controller
  ftdi_set_bitmode(m_ftdi, 0x0, BITMODE_RESET);

  // Enable MPSSE mode
  ftdi_set_bitmode(m_ftdi, 0x0, BITMODE_MPSSE);

  // Purge buffers
  ftdi_usb_purge_buffers(m_ftdi);

  // Set default state of pins
  std::vector<uint8_t> data={0x80, 0x00, 0xFB};
  ftdi_write_data(m_ftdi, &data[0], data.size());

  // Disable 5x clock divisor
  data={0x8A};
  ftdi_write_data(m_ftdi, &data[0], data.size());

  // Set clock to 30MHz (max)
  data={0x86, 0x00, 0x00};
  ftdi_write_data(m_ftdi, &data[0], data.size());

  // initialize high bytes values
  data={0x82, 0x0C, 0xDF};
  ftdi_write_data(m_ftdi, &data[0], data.size());

  // SPI devices
  m_spiADC=std::make_shared<PBv3CommPatchSPICom>(m_ftdi, 2);
  m_spiDAC=std::make_shared<PBv3CommPatchSPICom>(m_ftdi, 3);

  m_dac=std::make_shared<MCP4801 >(m_spiDAC);
  m_adc=std::make_shared<MAX11619>(2.5, m_spiADC);
  // setOF(true);
  
  // std::vector<uint8_t> spidata={0x3F,0xF0};
  // m_spiDAC->write_block(spidata);

  // //
  // // Run something related to SPI ADC
  // //
  // m_spiADC->write_block({0x18, 0xF9});
    
  // data.resize(2*4);  
  // m_spiADC->read_block(data);
  // for(uint8_t i=0; i<4;i++)
  //   {
  //     uint16_t reading=(data[2*i+0]<<8)|(data[2*i+1]<<0);
  //     std::cout << "CH" << i << ": " << ((reading>>2)&0x3FF) << std::endl;
  //   }
}

EndeavourRawFTDI::~EndeavourRawFTDI()
{
  ftdi_usb_close(m_ftdi);
  ftdi_free(m_ftdi);
}

void EndeavourRawFTDI::setDitMin(uint32_t DIT_MIN)
{ m_DIT_MIN=DIT_MIN; }

uint32_t EndeavourRawFTDI::getDitMin()
{ return m_DIT_MIN; }

void EndeavourRawFTDI::setDitMid(uint32_t DIT_MID)
{ m_DIT_MID=DIT_MID; }

uint32_t EndeavourRawFTDI::getDitMid()
{ return m_DIT_MID; }

void EndeavourRawFTDI::setDitMax(uint32_t DIT_MAX)
{ m_DIT_MAX=DIT_MAX; }

uint32_t EndeavourRawFTDI::getDitMax()
{ return m_DIT_MAX; }

void EndeavourRawFTDI::setDahMin(uint32_t DAH_MIN)
{ m_DAH_MIN=DAH_MIN; }

uint32_t EndeavourRawFTDI::getDahMin()
{ return m_DAH_MIN; }

void EndeavourRawFTDI::setDahMid(uint32_t DAH_MID)
{ m_DAH_MID=DAH_MID; }

uint32_t EndeavourRawFTDI::getDahMid()
{ return m_DAH_MID; }

void EndeavourRawFTDI::setDahMax(uint32_t DAH_MAX)
{ m_DAH_MAX=DAH_MAX; }

uint32_t EndeavourRawFTDI::getDahMax()
{ return m_DAH_MAX; }

void EndeavourRawFTDI::setBitGapMin(uint32_t BITGAP_MIN)
{ m_BITGAP_MIN=BITGAP_MIN; }

uint32_t EndeavourRawFTDI::getBitGapMin()
{ return m_BITGAP_MIN; }

void EndeavourRawFTDI::setBitGapMid(uint32_t BITGAP_MID)
{ m_BITGAP_MID=BITGAP_MID; }

uint32_t EndeavourRawFTDI::getBitGapMid()
{ return m_BITGAP_MID; }

void EndeavourRawFTDI::setBitGapMax(uint32_t BITGAP_MAX)
{ m_BITGAP_MAX=BITGAP_MAX; }

uint32_t EndeavourRawFTDI::getBitGapMax()
{ return m_BITGAP_MAX; }

void EndeavourRawFTDI::reset()
{  }

bool EndeavourRawFTDI::isError()
{ return false; }

bool EndeavourRawFTDI::isDataValid()
{ return m_readSize>0; }

void EndeavourRawFTDI::sendData(unsigned long long int data, unsigned int size)
{
  std::vector<uint8_t> ftdidata;
  
  uint8_t bytecounter=0, byte=0;
  for(uint32_t bit=size;bit>0;bit--)
    {
      bool value=(data>>(bit-1))&1;

      // Add 1's for data
      for(uint32_t i=0;i<((value)?m_DAH_MID:m_DIT_MID);i++)
      {
	byte<<=1;
	byte|=1;
	bytecounter++;
	if(bytecounter==8)
	  {
	    ftdidata.push_back(byte);
	    byte=0;
	    bytecounter=0;
	  }
      }

      // Add 0's for interbit gap
      for(uint32_t i=0;i<m_BITGAP_MID;i++)
	{
	  byte<<=1;
	  bytecounter++;
	  if(bytecounter==8)
	    {
	      ftdidata.push_back(byte);
	      byte=0;
	      bytecounter=0;
	    }
	}
    }

  // Pad rest of the bits
  ftdidata.resize(1024-3,0);

  // Prepare send package
  uint16_t Length=ftdidata.size()-1;
  ftdidata.insert(ftdidata.begin(),0x31);
  ftdidata.insert(ftdidata.begin()+1,(Length>>0)&0xFF);
  ftdidata.insert(ftdidata.begin()+2,(Length>>8)&0xFF);
  //std::cout << "Command header: " << std::hex << (uint32_t)ftdidata[0] << " " << (uint32_t)ftdidata[1] << " " << (uint32_t)ftdidata[2] << std::dec << std::endl;

  //
  // Start dealing with the FTDI

  // Make sure all buffers are clean
  ftdi_usb_purge_buffers(m_ftdi);

  // Send the data
  ftdi_write_data(m_ftdi, &ftdidata[0], ftdidata.size());
  // std::cout << "writing" << std::hex << std::endl;
  // for(const auto& x : ftdidata)
  // std::cout << (uint32_t)x << std::endl;
  // std::cout << std::dec;

  // Read back any response
  ftdi_read_alldata(m_ftdi, ftdidata, 1021);
  // std::cout << "reading" << std::hex << std::endl;
  // for(const auto& x : ftdidata)
  //   std::cout << (uint32_t)x << std::endl;
  // std::cout << std::dec;

  m_readData=0;
  m_readSize=0;
  uint32_t countbit=0;
  bool lastBit=0;
  //std::cout << "reading" << std::hex << std::endl;
  for(const auto& x : ftdidata)
    {
      //std::cout << (uint32_t)x << std::endl;
      for(uint8_t i=8;i>0;i--)
	{
	  bool bit=(x>>(i-1))&0x1;
	  if(bit!=lastBit) // something new is happening
	    {
	      if(lastBit==1) // Finished an endeavour bit
		{
		  if(m_DIT_MIN<countbit && countbit<m_DIT_MAX)
		    {
		      m_readData<<=1;
		      m_readData|=0;
		      m_readSize++;
		    }
		  else if(m_DAH_MIN<countbit && countbit<m_DAH_MAX)
		    {
		      m_readData<<=1;
		      m_readData|=1;
		      m_readSize++;
		    }
		}
	      countbit=0;
	    }
	  countbit++;
	  lastBit=bit;
	}
    }
  //std::cout << std::dec << std::endl;
}

void EndeavourRawFTDI::readData(unsigned long long int& data, unsigned int& size)
{
  data=m_readData;
  size=m_readSize;
}

void EndeavourRawFTDI::setOF(bool value)
{
  std::vector<uint8_t> data;

  data={0x83};
  ftdi_write_data(m_ftdi, &data[0], data.size());
  ftdi_read_alldata(m_ftdi, data, 1);
  uint8_t hbyte=data[0];
  
  hbyte=(value)?(hbyte|(1<<0)):(hbyte&~(1<<0));

  data={0x82, hbyte, 0xDF};  
  ftdi_write_data(m_ftdi, &data[0], data.size());
}

std::shared_ptr<DACDevice> EndeavourRawFTDI::getDAC() const
{ return m_dac; }

std::shared_ptr<ADCDevice> EndeavourRawFTDI::getADC() const
{ return m_adc; }

