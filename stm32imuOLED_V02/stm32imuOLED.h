#ifndef stm32imuOLED_H_
#define stm32imuOLED_H_

#include <SPI.h>
#include <Adafruit_SSD1306_STM32.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

void degerOlusturma(void);
void cizgiOlusturma(struct cizgi2B *ret,struct cizgi3B vec);
void goruntuOlusturma(void);
void ilkCizim();

#endif