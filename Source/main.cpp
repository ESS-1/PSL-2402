//----------------------------------------------------------------------------

//Project:  PSL-2401
//Compiler: IAR EWARM 6.50
//Target:   STM32F100C8T6B

//----------------------------------------------------------------------------

#include "main.h"
#include "control.h"
#if defined(PSL2402) || defined(PSL2802)
  #include "port.h"
#endif

//----------------------------- ����������: ----------------------------------

TControl *Control;
#if defined(PSL2402) || defined(PSL2802)
  TPort *Port;
#endif

//----------------------------------------------------------------------------
//------------------------- �������� ���������: ------------------------------
//----------------------------------------------------------------------------

int main(void)
{
  TSysTimer::Init();        //������������� ���������� �������
  Control = new TControl(); //�������� ������� ����������
#if defined(PSL2402) || defined(PSL2802)
  Port = new TPort();       //�������� ������� �����
#endif
  
  do                        //�������� ����
  {
    TSysTimer::Sync();      //������������� ��������� ����� � �������� ������
    Control->Execute();     //���������� �������� ����������
#if defined(PSL2402) || defined(PSL2802)
    Port->Execute();        //���������� ������ ����������
#endif
  }
  while(1);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
