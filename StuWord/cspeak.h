#ifndef CSPEAK_H
#define CSPEAK_H
#undef UNICODE
#include <sapi.h>
#include <sphelper.h>
#include <comdef.h>
#define UNICODE

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <QString>
#include <QAxObject>
class CSpeak
{
public:
    CSpeak();
    static void Speak(QString);
    static void InitSpeak();
    static void ClearSpeak();
private:
    static QAxObject *speak;
};

#endif // CSPEAK_H
