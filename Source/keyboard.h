//----------------------------------------------------------------------------

//������ ��������� ����������: header file

//----------------------------------------------------------------------------

#ifndef KEYBOARD_H
#define KEYBOARD_H

//----------------------------------------------------------------------------

#include "systimer.h"

//----------------------------- ���������: -----------------------------------

enum KeyMsg_t //���� ��������� ����������
{
  KBD_NOP    = 0x00, //��� �������
  KBD_OUT    = 0x01, //������ OUT ON/OFF
  KBD_ENC    = 0x02, //������ ��������
  KBD_HOLD   = 0x80, //���� ��������� ������
  KBD_ENCH   = KBD_ENC  + KBD_HOLD, //��������� ������ ��������
  KBD_ERROR  = 0x7F  //��� ������ ��� ������� ��������
};

//----------------------------------------------------------------------------
//--------------------------- ����� TKeyboard: -------------------------------
//----------------------------------------------------------------------------

class TKeyboard
{
private:
  TGpio<PORTB, PIN11> Pin_KeyO; 
  TGpio<PORTB, PIN12> Pin_KeyE; 
  TSoftTimer *DebounceTimer;
  TSoftTimer *HoldTimer;
  KeyMsg_t Prev_Key;
public:
  TKeyboard(void);
  void Execute(void);
  KeyMsg_t Scan(void);
  KeyMsg_t Message;
};

//----------------------------------------------------------------------------

#endif
