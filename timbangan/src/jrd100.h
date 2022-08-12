#ifndef __JRD100_H__
#define __JRD100_H__

#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include "stdio.h"

class JRD100
{
public:
    void beginJRD100();
    bool measure_jrd100(void);
    bool send_cmd(void);
    char datarx1[250];
    unsigned char BIsResultCompleted1 = 0;
    bool trig_dataID = false;
    // SoftwareSerial myPort;
    // String              getWeight(void);

private:
    // SoftwareSerial*     WeightSerial;
    // String              weight;
};

#endif