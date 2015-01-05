#include "cspeak.h"

QAxObject *CSpeak::speak;

CSpeak::CSpeak()
{
}

void CSpeak::Speak(QString word)
{
    //speak->dynamicCall("SetRate(int)",tr("%1").arg(voiceRate_));     //根据选项设置语速
    //speak->dynamicCall("SetVolume(int)",tr("%1").arg(voiceVol_));   //根据选项设置音量
    speak->dynamicCall("Speak(QString)",word);
}

void CSpeak::InitSpeak()
{
    speak=new QAxObject;
    ::CoInitialize(NULL);//初始化语音环境
    speak->setControl("{96749377-3391-11D2-9EE3-00C04F797396}"); //启动 Microsoft Speech 5.1
}

void CSpeak::ClearSpeak()
{
    delete speak;
    CoUninitialize();
}
