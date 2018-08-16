#ifndef RIGOLDP832_H
#define RIGOLDP832_H

// ####################
// Rigol DP832 Source Meter
// Author: Timon Heim
// Date: April 2018
// Notes: USB
// ###################

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "SerialCom.h"

class RigolDP832 {
    public:
        RigolDP832(std::string dev);
        ~RigolDP832();

        void init();
        void turnOn(unsigned channel);
        void turnOff(unsigned channel);
	//void setCh(unsigned ch);
        void setVoltageCurrent(unsigned channel, float volt, float cur);
        float getVoltage(unsigned channel);
        float getCurrent(unsigned channel);
        std::string getVoltageStr(unsigned channel);
        std::string getCurrentStr(unsigned channel);

    private:
        SerialCom *m_com;

        void send(std::string cmd);
        std::string receive(std::string cmd);

        std::chrono::milliseconds m_wait{200};
};

#endif

