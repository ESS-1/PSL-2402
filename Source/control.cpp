//----------------------------------------------------------------------------

//Модуль общего управления

//----------------------------------------------------------------------------

#include "main.h"
#include "control.h"
#include "sound.h"
#include "data.h"
#include "analog.h"

//----------------------------- Переменные: ----------------------------------

TDisplay *Display;
TSound *Sound;
TEncoder *Encoder;
TData *Data;
TAnalog *Analog;

//----------------------------------------------------------------------------
//---------------------------- Класс TControl: -------------------------------
//----------------------------------------------------------------------------

//----------------------------- Конструктор: ---------------------------------


TControl::TControl(void)
{
  //начальная задержка
  //(щелчок выключателя не должен совпадать
  //по времени с начальным beep):
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
  //применение параметров:
  Data->ApplyAll();
  Sound->Beep(); //начальный beep

  if(TEeprom::Error)
  {
    //меню ошибки EEPROM:
    MnuIndex = MNU_ERROR;
    ParIndex = TEeprom::Error;
  }
  else
  {
    //основное меню:
    MnuIndex = MNU_MAIN;
    ParIndex = 0;
    KeyMsg_t KeyMsg = Keyboard->Scan();
    //проверка удержания кнопки энкодера при включении:
    if(KeyMsg == KBD_ENC)
    {
      //установка MAXV:
      MnuIndex = MNU_TOP;
      ParIndex = PAR_MAXV;
    }
  }
  //выбор нужного меню:
  Menu->SelectMenu(MnuIndex, ParIndex);
  //запуск таймера возврата из меню:
  MenuTimer->Start(Menu->SelectedMenu->Timeout);
}

//------------------------ Выполнение управления: ----------------------------

void TControl::Execute(void)
{
  Display->Execute();
  Encoder->Execute();
  Keyboard->Execute();
  Sound->Execute();
  Analog->Execute();

  //переход в другое меню, если требуется
  if(Menu->SelectedMenu->MnuIndex != MnuIndex)
  {
    MnuIndex = Menu->SelectedMenu->MnuIndex;
    ParIndex = Menu->SelectedMenu->ParIndex;
    Menu->SelectMenu(MnuIndex, ParIndex);
    MenuTimer->Start(Menu->SelectedMenu->Timeout);
  }

  //обновление индикации в меню:
  Menu->SelectedMenu->Execute();

  //считывание энкодера и клавиатуры:
  KeyMsg_t KeyMsg = Keyboard->Message;
  int8_t Step = Encoder->Message;

  //нажатие кнопки:
  if(KeyMsg != KBD_NOP)
  {
    if(!(KeyMsg & KBD_HOLD)) Sound->Beep();
    Menu->SelectedMenu->OnKeyboard(KeyMsg);
    MenuTimer->Start(Menu->SelectedMenu->Timeout);
  }

  //поворот энкодера:
  if(Step)
  {
    Menu->SelectedMenu->OnEncoder(Step);
    MenuTimer->Start(Menu->SelectedMenu->Timeout);
  }

  //переполнение таймера:
  if(MenuTimer->Over())
  {
    Menu->SelectedMenu->OnTimer();
  }

  //сервисы защиты и управления:
  ProtectionService(KeyMsg);
  OutSwitchService(KeyMsg);

  //звук, если события не обработаны:
  if((KeyMsg != KBD_NOP) && !(KeyMsg & KBD_HOLD))
    Sound->Bell();
  if(Step) Sound->Click();
  //сброс кодов клавиатуры и энкодера:
  Keyboard->Message = KBD_NOP;
  Encoder->Message = ENC_NOP;
}

//----------------------------------------------------------------------------
//------------------------- Реализация сервисов: -----------------------------
//----------------------------------------------------------------------------

//---------------------------- Сервис защиты: --------------------------------

inline void TControl::ProtectionService(KeyMsg_t &KeyMsg)
{
  //из MNU_SETUP в меню защиты перехода нет:
  if(MnuIndex != MNU_SETUP && MnuIndex != MNU_CALIB)
  {
    char ProtSt = Analog->GetProtSt();
    //проверка срабатывания OTP:
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
    //проверка срабатывания OCP:
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
    //проверка срабатывания OVP:
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
    //проверка срабатывания OPP:
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

//---------------------- Сервис управления выходом: --------------------------

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
