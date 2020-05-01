//----------------------------------------------------------------------------

//Модуль поддержки LED-дисплея, заголовочный файл

//----------------------------------------------------------------------------

#ifndef DISPLAY_H
#define DISPLAY_H

//----------------------------------------------------------------------------

#include "systimer.h"
#include "sreg.h"

//----------------------------- Константы: -----------------------------------

#define POINT      0x80 //флаг децимальной точки
#define AUTO_RANGE 0x40 //флаг автоматического выбора единиц
#define AUTO_SCALE 0x80 //флаг автомасштабирования

enum PosName_t //имена позиций дисплея
{
  POS_1,
  POS_2,
  POS_3,
  DIGS
};

enum RowName_t //имена строк дисплея
{
  ROW_V,
  ROW_I,
  ROWS
};

enum Blink_t //имена режимов мигания
{
  BLINK_NO   = 0x00,
  BLINK_V    = 0x01,
  BLINK_I    = 0x02,
  BLINK_VI   = 0x03,
  BLINK_NC   = 0x04
};

//----------------------------------------------------------------------------
//---------------------------- Класс TDisplay: -------------------------------
//----------------------------------------------------------------------------

class TDisplay
{
private:
  TGpio<PORTA, PIN12> Pin_SC1; 
  TGpio<PORTA, PIN11> Pin_SC2; 
  TGpio<PORTB, PIN1>  Pin_SC3; 
  char SegDataV[DIGS];
  char SegDataI[DIGS];
  char Row;
  char Pos;
  char Conv(char d);
  void SetScan(char phase);
  Blink_t BlinkEn;
  bool BlinkOn;
  bool DispOn;
  TSreg Sreg;
  TSoftTimer *BlinkTimer;
public:
  TDisplay(void);
  void Execute(void); 
  bool LedCV;              //управление светодиодом CV
  bool LedCC;              //управление светодиодом CC
  void Blink(Blink_t blink); //включение/выключение мигания
  void Clear(void);        //очистка дисплея
  void Off(void);          //отключение дисплея (только цифры)
  void On(void);           //включение дисплея (только цифры)
  void Disable(void);      //отключение всей индикации
  void Enable(void);       //включение всей индикации
  void SetPos(char row, char pos); //установка позиции
  void PutChar(char ch);   //вывод символа
  void PutString(char *s); //вывод строки из RAM
  void PutString(const char *s); //вывод строки из ROM
  void PutIntF(int32_t v, char n, char d); //форматированный вывод числа
};

//----------------------------------------------------------------------------

extern TDisplay *Display;

//----------------------------------------------------------------------------

#endif
