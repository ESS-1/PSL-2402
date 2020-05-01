//----------------------------------------------------------------------------

//������ ��������� LED-�������, ������������ ����

//----------------------------------------------------------------------------

#ifndef DISPLAY_H
#define DISPLAY_H

//----------------------------------------------------------------------------

#include "systimer.h"
#include "sreg.h"

//----------------------------- ���������: -----------------------------------

#define POINT      0x80 //���� ����������� �����
#define AUTO_RANGE 0x40 //���� ��������������� ������ ������
#define AUTO_SCALE 0x80 //���� �������������������

enum PosName_t //����� ������� �������
{
  POS_1,
  POS_2,
  POS_3,
  DIGS
};

enum RowName_t //����� ����� �������
{
  ROW_V,
  ROW_I,
  ROWS
};

enum Blink_t //����� ������� �������
{
  BLINK_NO   = 0x00,
  BLINK_V    = 0x01,
  BLINK_I    = 0x02,
  BLINK_VI   = 0x03,
  BLINK_NC   = 0x04
};

//----------------------------------------------------------------------------
//---------------------------- ����� TDisplay: -------------------------------
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
  bool LedCV;              //���������� ����������� CV
  bool LedCC;              //���������� ����������� CC
  void Blink(Blink_t blink); //���������/���������� �������
  void Clear(void);        //������� �������
  void Off(void);          //���������� ������� (������ �����)
  void On(void);           //��������� ������� (������ �����)
  void Disable(void);      //���������� ���� ���������
  void Enable(void);       //��������� ���� ���������
  void SetPos(char row, char pos); //��������� �������
  void PutChar(char ch);   //����� �������
  void PutString(char *s); //����� ������ �� RAM
  void PutString(const char *s); //����� ������ �� ROM
  void PutIntF(int32_t v, char n, char d); //��������������� ����� �����
};

//----------------------------------------------------------------------------

extern TDisplay *Display;

//----------------------------------------------------------------------------

#endif
