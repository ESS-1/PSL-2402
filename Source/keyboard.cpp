//----------------------------------------------------------------------------

//������ ��������� ����������

//----------------------- ������������ �������: ------------------------------

//������ "OUT ON/OFF" � ������ �������� ���������� � ������ Pin_KeyO �
//Pin_KeyE ��������������. ��� ������� ������ �� ����� ����������
//������ �������.

//----------------------------------------------------------------------------

#include "main.h"
#include "keyboard.h"

//--------------------------- ���������: -------------------------------------

#define DEBOUNCE_TM    50 //�������� ���������� ��������, ms
#define HOLD_DELAY   1000 //�������� ��������� ������, ms

//----------------------------------------------------------------------------
//--------------------------- ����� TKeyboard: -------------------------------
//----------------------------------------------------------------------------

//----------------------------- �����������: ---------------------------------

TKeyboard::TKeyboard(void)
{
  Pin_KeyO.Init(IN_PULL, PULL_UP);
  Pin_KeyE.Init(IN_PULL, PULL_UP);
  Message = KBD_NOP;
  Prev_Key = KBD_NOP;
  DebounceTimer = new TSoftTimer();
  DebounceTimer->Oneshot = 1;
  HoldTimer = new TSoftTimer();
  HoldTimer->Oneshot = 1;
}

//----------------------- ������������ ����������: ---------------------------

KeyMsg_t TKeyboard::Scan(void)
{
  char s = KBD_NOP;
  if(!Pin_KeyO) s |= KBD_OUT;  //������ OUT ON/OFF
  if(!Pin_KeyE) s |= KBD_ENC;  //������ ��������
  return((KeyMsg_t)s);
}

//------------------------- ��������� ����������: ----------------------------

void TKeyboard::Execute(void)
{
  if(TSysTimer::Tick) //������ ��������� ��� (���� �� �����)
  {
    KeyMsg_t Key = Scan();               //������������ ����������
    if(Key != Prev_Key)                  //��� ����������
    {
      DebounceTimer->Start(DEBOUNCE_TM); //������ ������� ���������� ��������
      HoldTimer->Start(HOLD_DELAY);      //������ ������� ��������� ������
      Prev_Key = Key;                    //���������� ������ ����
    }
    else if(Key != KBD_NOP)              //��� ��������� � �� ����� KBD_NOP
    {
      if(DebounceTimer->Over())          //���� �������� ���,
        Message = Key;                   //��������� ��������� �������
      if(HoldTimer->Over())              //���� �������� �����,
        Message = (KeyMsg_t)(Key + KBD_HOLD); //��������� ��������� ���������
    }
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
