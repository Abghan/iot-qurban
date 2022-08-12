#ifndef __WEIGHT_A12E_H__
#define __WEIGHT_A12E_H__

#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include "stdio.h"

class Weight_A12E
{
public:
    void    begin();
    bool    measure(void);    
};

#endif