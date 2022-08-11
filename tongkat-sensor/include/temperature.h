#ifndef temperature_H
#define temperature_H
#include <Arduino.h>

#include "Melopero_AMG8833.h"

class temperature
{
private:
    Melopero_AMG8833 sensor;
    bool stop_stream;
    float pixels[64];
    float max_value;
    int count = 0;
    const int average_duration = 5;
    float temp_average, sum = 0;
    float temp_max_scan[5];

    void calc_avge(float temp);
    void read_pixels();
    float cal_avg_max();

public:
    void setup_melopero();
    void loop_melopero();
    String rata_rata_suhu;
    bool active = false;
    bool temp_finish = false; 

};
#endif