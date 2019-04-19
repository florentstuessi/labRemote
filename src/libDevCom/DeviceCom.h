#ifndef DEVICECOM_H
#define DEVICECOM_H

#include <vector>
#include <cstdint>

class DeviceCom
{
public:
  DeviceCom();

  //
  // Write commands
  virtual void write_reg32(uint32_t address, uint32_t data) =0;
  virtual void write_reg16(uint32_t address, uint16_t data) =0;
  virtual void write_reg8 (uint32_t address, uint8_t  data) =0;

  virtual void write_reg32(uint32_t data) =0;
  virtual void write_reg16(uint16_t data) =0;
  virtual void write_reg8 (uint8_t  data) =0;

  virtual void write_block(uint32_t address, const std::vector<uint8_t>& data) =0;
  virtual void write_block(const std::vector<uint8_t>& data) =0;


  //
  // Read commands
  virtual uint32_t read_reg32(uint32_t address) =0;
  virtual uint16_t read_reg16(uint32_t address) =0;
  virtual uint8_t  read_reg8 (uint32_t address) =0;

  virtual uint32_t read_reg32() =0;
  virtual uint16_t read_reg16() =0;
  virtual uint8_t  read_reg8 () =0;

  virtual void read_block(uint32_t address, std::vector<uint8_t>& data) =0;
  virtual void read_block(std::vector<uint8_t>& data) =0;
};

#endif // DEVICECOM_H
