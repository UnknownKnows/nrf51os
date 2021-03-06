/***************************************************************************************
# Copyright (c) 2015,  Vipin Bakshi
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
****************************************************************************************/
#ifndef _APP_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "configuration.h"

#define APP_FREQ         10   // App requested frequency. Derived from main Main RTC Clock Freq.

#if defined(RTC_INCLUDE)
   #define APP_HZ_EVENT(counter, hz)     (counter == (RTC_TICK_FRQ / hz))
#endif   // RTC_INCLUDE

#if defined(NRF51_MPU9150)
#include "twi_master.h"
#include "nrf_delay.h"
#include "os.h"


#define delay_ms   nrf_delay_ms
#if defined(RTC_INCLUDE)
   #define get_ms     os_get_time_ms
#endif  // RTC_INCLUDE
//#define labs       abs
#define fabs(x)    (((x)>0)?(x):-(x))
#define min(a,b)   ((a<b)?a:b)
#define log_i(...)     do {} while (0)
#define log_e(...)     do {} while (0)

static inline bool i2c_write(uint8_t slave_address, uint8_t register_address, uint8_t write_size, uint8_t* write_datas)
{
   // Return opposite logic for IMU driver.
   uint8_t write_data[write_size + 1];		
   write_data[0] = register_address;			
   memcpy(&write_data[1], write_datas, write_size);		
   return (twi_master_transfer((slave_address << 1), write_data, write_size + 1, TWI_ISSUE_STOP) == false);
}

static inline bool i2c_read(uint8_t slave_address, uint8_t register_address, uint8_t read_size, uint8_t* read_datas)
{
   // Return opposite logic for IMU driver.
   if (twi_master_transfer((slave_address << 1), &register_address, 1, TWI_DONT_ISSUE_STOP))
      return (twi_master_transfer((slave_address << 1) | TWI_READ_BIT, read_datas, read_size, TWI_ISSUE_STOP) == false);

   return true;
}

#endif  // NRF51_MPU9250
#endif // _APP_H_
