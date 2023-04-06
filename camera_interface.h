#ifndef _ESPCAMERA_H_
#define _ESPCAMERA_H_

#include <Arduino.h>

namespace ESP_CAMERA
{
  const byte header[] = { 'V', 'F', 'M' };
  const byte footer[] = { 'V', 'E', 'D' };
  
  void init_camera();
  int get_frame(byte *frame);
}

#endif
