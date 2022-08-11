#include "temperature.h"

void temperature::setup_melopero()
{
  // initializing I2C to use default address AMG8833_I2C_ADDRESS_B and Wire (I2C-0):
  sensor.initI2C(AMG8833_I2C_ADDRESS_B,Wire);
  Wire.begin();
  
  if(Wire.available())
  {
    int statusCode = sensor.resetFlagsAndSettings();
    statusCode = sensor.setFPSMode(FPS_MODE::FPS_10);
  }
}
void temperature::calc_avge(float temp)
{
  // Rata2 suhu tertinggi dalam 5 kali read pixel
  String suhu;

  if (count < average_duration)
  {
    temp_max_scan[count] = temp;
    count++;
  }

  if (count == average_duration)
  {
    for (int x = 0; x < average_duration; x++)
      sum += temp_max_scan[x];
    temp_average = sum / average_duration;
    if(temp_average == 0) return;
   // temp_average = temp_average - 2.00;
    temp_average = temp_average + 2.7;
    rata_rata_suhu =  String(temp_average);
    sum = 0;
    count = 0;
  }
}

float max_temp[5]={0,0,0,0,0};
int cnt_max = 0;
float temperature::cal_avg_max()
{
  float sum = 0;
  for(int i =0 ; i < 5; i++)
  {
    //Serial.printf("Pixel %d value %.2f ",i, max_temp[i]);
    sum = sum + max_temp[i]; 
  } 

  sum = sum / 5;
  Serial.println();
  return sum;
}

void temperature::read_pixels()
{
  int idx = 0;
  for (int x = 0; x < 8; x++)
  {
    for (int y = 0; y < 8; y++)
    {
      pixels[idx] = sensor.pixelMatrix[y][x];

      if (idx == 0) max_value = pixels[idx];
      if (pixels[idx] > max_value) max_value = pixels[idx];
      idx++;

      if (idx == 64)
      {
        max_temp[cnt_max] = max_value;
        cnt_max = cnt_max + 1;
        if(cnt_max == 5) 
        {
          //Serial.println("OK");
          cnt_max = 0;
          float nilai = cal_avg_max() + 2.7;
          rata_rata_suhu = String(nilai,2);
          temp_finish = true;
          //Serial.println();
        }
        idx = 0;
        //calc_avge(max_value);
        
      }
    }
  }
}
void temperature::loop_melopero()
{
  if(active)
  {
    //Serial.println("Updating pixel matrix ... ");
    int statusCode = sensor.updatePixelMatrix();
    //Serial.println(statusCode);
    sensor.getErrorDescription(statusCode);
    read_pixels();
  }
}
