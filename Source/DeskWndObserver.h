#pragma once
#include "MSAAObj.h"

class DeskWndObserver;

DWORD WINAPI DeskWinProc( LPVOID lpParam );


class DeskWndObserver{
public:
    DeskWndObserver()
        : wndClsName(TEXT("MozillaWindowClass"))
        , accfavirateObjData(TEXT("地址"), TEXT("按下按钮"), TEXT("MozillaWindowClass"), TEXT("收藏为书签"))
        , accaddressObjData(TEXT("搜索或输入网址 <Ctrl+L>"), TEXT("可编辑文本"), TEXT("MozillaWindowClass"))
    {
        OleInitialize(NULL);
    }

    ~DeskWndObserver()
    {
        OleUninitialize();
    }

    juce_DeclareSingleton(DeskWndObserver, true);

    void FindTargetWnd()
    {
        std::vector<HWND> wnds;

        WindowAccessHelper::FindMainWnds(wnds, wndClsName);

        if (wnds.empty())
        {
            wndHandle = NULL;
            return;
        }
        //	m_hMainWnd = *(wnds.begin());
        wndHandle = WindowAccessHelper::GetHwndToModify(wnds);
    }

    RECT getFavirateRect()
    {
        return accfavirateObjData.m_rect;
    }
  
    void FindAccObj()
    {
        if (!WindowAccessHelper::FindAccObj(wndHandle, accfavirateObjData))
        {
            accfavirateObjData.m_Description = TEXT("编辑此书签");
            WindowAccessHelper::FindAccObj(wndHandle, accfavirateObjData);
        }
        WindowAccessHelper::FindAccObj(wndHandle, accaddressObjData);
    }

    void recaptureFavariteLoc()
    {
        RECT r = {0};
        accfavirateObjData.m_rect = r;
        accfavirateObjData.m_Description = TEXT("收藏为书签");
        if (!WindowAccessHelper::FindAccObj(wndHandle, accfavirateObjData))
        {
            accfavirateObjData.m_Description = TEXT("编辑此书签");
            WindowAccessHelper::FindAccObj(wndHandle, accfavirateObjData);
        }
    }

    HWND GetBrowserHwnd(){return wndHandle;}

    UINT GetWindowTitle( LPTSTR lpszTitle )
    {
        TCHAR tmsz[MAX_PATH] = {0};
        GetWindowText(wndHandle, tmsz, MAX_PATH);
        return Convert(tmsz, lpszTitle, CP_ACP, CP_UTF8);
    }

    std::string GetWebHref()
    {
        RECT r = {0};
        accaddressObjData.m_rect = r;
        memset(accaddressObjData.m_Value, 0, MAX_PATH);
        WindowAccessHelper::FindAccObj(wndHandle, accaddressObjData);
        return accaddressObjData.m_Value;
    }

    int Convert(const char* strIn, char* strOut, int sourceCodepage, int targetCodepage)
    {
        int unicodeLen=MultiByteToWideChar(sourceCodepage,0,strIn,-1,NULL,0);
        wchar_t* pUnicode;
        pUnicode=new wchar_t[unicodeLen+1];
        memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));
        MultiByteToWideChar(sourceCodepage,0,strIn,-1,(LPWSTR)pUnicode,unicodeLen);
        BYTE * pTargetData = NULL;
        int targetLen=WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,0,NULL,NULL);
        pTargetData=new BYTE[targetLen+1];
        memset(pTargetData,0,targetLen+1);
        int outLen = WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,targetLen,NULL,NULL);
        lstrcpy(strOut,(char*)pTargetData);
        delete pUnicode;
        delete pTargetData;
        return outLen;
    }

private:
    WinAccObj accfavirateObjData;
    WinAccObj accaddressObjData;
    LPCTSTR   wndClsName;
    HWND     wndHandle;
};

DWORD WINAPI DeskWinProc( LPVOID lpParam )
{
    DeskWndObserver* pObserver = (DeskWndObserver*)lpParam;
    pObserver->FindTargetWnd();
    return 0;
}
