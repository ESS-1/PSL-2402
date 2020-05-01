//----------------------------------------------------------------------------

//������ ������ ����������

//----------------------------------------------------------------------------

#include "main.h"
#include "control.h"
#include "sound.h"
#include "data.h"
#include "analog.h"

//----------------------------- ����������: ----------------------------------

TDisplay *Display;
TSound *Sound;
TEncoder *Encoder;
TData *Data;
TAnalog *Analog;

//----------------------------------------------------------------------------
//---------------------------- ����� TControl: -------------------------------
//----------------------------------------------------------------------------

//----------------------------- �����������: ---------------------------------


TControl::TControl(void)
{
  //��������� ��������
  //(������ ����������� �� ������ ���������
  //�� ������� � ��������� beep):
  TSysTimer::Delay_ms(200);
  TEeprom::Init();
  Display = new TDisplay();
  Sound = new TSound();
  Encoder = new TEncoder();
  Keyboard = new TKeyboard();
  Analog = new TAnalog();
  Data = new TData();
  Menu = new TMenuItems(MENUS);
  MenuTimer = new TSoftTimer();
  MenuTimer->Oneshot = 1;
  //���������� ����������:
  Data->ApplyAll();
  Sound->Beep(); //��������� beep

  if(TEeprom::Error)
  {
    //���� ������ EEPROM:
    MnuIndex = MNU_ERROR;
    ParIndex = TEeprom::Error;
  }
  else
  {
    //�������� ����:
    MnuIndex = MNU_MAIN;
    ParIndex = 0;
    KeyMsg_t KeyMsg = Keyboard->Scan();
    //�������� ��������� ������ �������� ��� ���������:
    if(KeyMsg == KBD_ENC)
    {
      //��������� MAXV:
      MnuIndex = MNU_TOP;
      ParIndex = PAR_MAXV;
    }
  }
  //����� ������� ����:
  Menu->SelectMenu(MnuIndex, ParIndex);
  //������ ������� �������� �� ����:
  MenuTimer->Start(Menu->SelectedMenu->Timeout);
}

//------------------------ ���������� ����������: ----------------------------

void TControl::Execute(void)
{
  Display->Execute();
  Encoder->Execute();
  Keyboard->Execute();
  Sound->Execute();
  Analog->Execute();

  //������� � ������ ����, ���� ���������
  if(Menu->SelectedMenu->MnuIndex != MnuIndex)
  {
    MnuIndex = Menu->SelectedMenu->MnuIndex;
    ParIndex = Menu->SelectedMenu->ParIndex;
    Menu->SelectMenu(MnuIndex, ParIndex);
    MenuTimer->Start(Menu->SelectedMenu->Timeout);
  }

  //���������� ��������� � ����:
  Menu->SelectedMenu->Execute();

  //���������� �������� � ����������:
  KeyMsg_t KeyMsg = Keyboard->Message;
  int8_t Step = Encoder->Message;

  //������� ������:
  if(KeyMsg != KBD_NOP)
  {
    if(!(KeyMsg & KBD_HOLD)) Sound->Beep();
    Menu->SelectedMenu->OnKeyboard(KeyMsg);
    MenuTimer->Start(Menu->SelectedMenu->Timeout);
  }

  //������� ��������:
  if(Step)
  {
    Menu->SelectedMenu->OnEncoder(Step);
    MenuTimer->Start(Menu->SelectedMenu->Timeout);
  }

  //������������ �������:
  if(MenuTimer->Over())
  {
    Menu->SelectedMenu->OnTimer();
  }

  //������� ������ � ����������:
  ProtectionService(KeyMsg);
  OutSwitchService(KeyMsg);

  //����, ���� ������� �� ����������:
  if((KeyMsg != KBD_NOP) && !(KeyMsg & KBD_HOLD))
    Sound->Bell();
  if(Step) Sound->Click();
  //����� ����� ���������� � ��������:
  Keyboard->Message = KBD_NOP;
  Encoder->Message = ENC_NOP;
}

//----------------------------------------------------------------------------
//------------------------- ���������� ��������: -----------------------------
//----------------------------------------------------------------------------

//---------------------------- ������ ������: --------------------------------

inline void TControl::ProtectionService(KeyMsg_t &KeyMsg)
{
  //�� MNU_SETUP � ���� ������ �������� ���:
  if(MnuIndex != MNU_SETUP && MnuIndex != MNU_CALIB)
  {
    char ProtSt = Analog->GetProtSt();
    //�������� ������������ OTP:
    if(ProtSt & PR_OTP)
    {
      if(MnuIndex != MNU_PROT)
      {
        MnuIndex = MNU_PROT;
        ParIndex = PAR_OTP;
        Menu->SelectMenu(MnuIndex, ParIndex);
        MenuTimer->Start(0);
      }
      if(KeyMsg == KBD_OUT)
        KeyMsg = KBD_ERROR;
    }
    //�������� ������������ OCP:
    else if(ProtSt & PR_OCP)
    {
      if(MnuIndex != MNU_PROT)
      {
        MnuIndex = MNU_PROT;
        ParIndex = PAR_OCP;
        Menu->SelectMenu(MnuIndex, ParIndex);
        MenuTimer->Start(0);
      }
    }
    //�������� ������������ OVP:
    else if(ProtSt & PR_OVP)
    {
      if(MnuIndex != MNU_PROT)
      {
        MnuIndex = MNU_PROT;
        ParIndex = PAR_OVP;
        Menu->SelectMenu(MnuIndex, ParIndex);
        MenuTimer->Start(0);
      }
    }
    //�������� ������������ OPP:
    else if(ProtSt & PR_OPP)
    {
      if(MnuIndex != MNU_PROT)
      {
        MnuIndex = MNU_PROT;
        ParIndex = PAR_OPP;
        Menu->SelectMenu(MnuIndex, ParIndex);
        MenuTimer->Start(0);
      }
    }
  }
}

//---------------------- ������ ���������� �������: --------------------------

inline void TControl::OutSwitchService(KeyMsg_t &KeyMsg)
{
  if(KeyMsg == KBD_OUT)
  {
    Analog->OutControl(!Analog->OutState());
    if(Data->SetupData->Items[PAR_OUT]->Value == ON)
      Data->SaveV();
    KeyMsg = KBD_NOP;
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
