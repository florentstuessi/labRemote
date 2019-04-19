#ifndef AGILENTPS_H
#define AGILENTPS_H

// ####################
// Agilent/Keysight Power supply
// Author: Timon Heim
// Date: Feb 2017
// Notes: Assuming Prologix GPIB to USB
// ###################

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "SerialCom.h"
#include "GenericPs.h"

class AgilentPs : public GenericPs {
    public:
        AgilentPs(std::string dev, unsigned addr);
        ~AgilentPs();

        void init();
        void setCh(unsigned ch);
        void setRange(unsigned range);
        void setVoltage(double volt);
        std::string getVoltage();
        void setCurrent(double cur);
        std::string getCurrent();
        void turnOn();
        void turnOff();

    private:
        SerialCom *m_com;
        unsigned m_addr;

        void send(std::string cmd);
        std::string receive(std::string cmd);
        
        std::chrono::milliseconds m_wait{200};
};

#endif
