/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TransparentWnd.h"
#include "DeskWndObserver.h"
#include <string>

juce_ImplementSingleton(DeskWndObserver)

//DWORD WINAPI WndSizeObserverProc( LPVOID lpParam );

//==============================================================================
class NewProjectApplication  
    : public JUCEApplication
    , public ActionBroadcaster
{
public:
    //==============================================================================
    NewProjectApplication()
        : browserHwnd(NULL)
        , hObserver(NULL)
    {
        NewProjectApplication::bStopObserve = false;
    }

    const String getApplicationName()       { return ProjectInfo::projectName; }
    const String getApplicationVersion()    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()       { return true; }

    //==============================================================================
    void initialise (const String& commandLine)
    {
        LoadDtdData::getInstance()->parseDtdFile("strings.dtd");
        BookmarkFileIO::getInstance()->init("bookmarks.json");

        DeskWndObserver::getInstance()->FindTargetWnd();
        DeskWndObserver::getInstance()->FindAccObj();
        browserHwnd = DeskWndObserver::getInstance()->GetBrowserHwnd();
        Logger::writeToLog(String::toHexString((int)browserHwnd));
        RECT rct = DeskWndObserver::getInstance()->getFavirateRect();
        juce::Rectangle<int> wndPos(int(rct.left * 0.8f), int(rct.top * 0.8f), 
            int(rct.right * 0.8f), int(rct.bottom * 0.8f));
#ifdef _DEBUG
        if (wndPos.isEmpty())
        {
            wndPos = juce::Rectangle<int>(700,0,15,18);
        }
#endif
        wnd = new TransparentWnd(wndPos);
        wnd->addToDesktop(ComponentPeer::windowIsSemiTransparent);
        wnd->setBounds(wndPos);
        wnd->setAlwaysOnTop(true);
        wnd->setVisible(true);
        
        GetWindowRect(browserHwnd, &browserRect);
        hObserver = CreateThread(NULL, 0, NewProjectApplication::WndSizeObserverProc, this, 0, NULL);
        addActionListener(wnd);
        // Add your application's initialisation code here..
    }

    void onBrowserMoved()
    {
        DeskWndObserver::getInstance()->recaptureFavariteLoc();
        RECT rct = DeskWndObserver::getInstance()->getFavirateRect();
        juce::Rectangle<int> wndPos(int(rct.left * 0.8f), int(rct.top * 0.8f), 
            int(rct.right * 0.8f), int(rct.bottom * 0.8f));
        wnd->changeLocation(wndPos);
        sendActionMessage("browser window resized/moved");
    }

    void setTopmost(bool bTop)
    {
        wnd->setAlwaysOnTop(bTop);
        if (bTop) wnd->setVisible(true);
    }

    void hideWindow()
    {
        wnd->setVisible(false);
    }

    void shutdown()
    {
        NewProjectApplication::bStopObserve = true;
        if ( NULL != hObserver)
        {
            WaitForSingleObject(hObserver, INFINITE);
            hObserver = NULL;
        }
        BookmarkFileIO::getInstance()->saveToFile();
        BookmarkFileIO::deleteInstance();
        LoadDtdData::deleteInstance();
        DeskWndObserver::deleteInstance();
        wnd = nullptr;
        // Add your application's shutdown code here..
    }

    //==============================================================================
    void systemRequestedQuit()
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const String& commandLine)
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }
private:
    static DWORD WINAPI WndSizeObserverProc(LPVOID pMain)
    {
        Logger::writeToLog("mesage hooked");
        NewProjectApplication* projMain = (NewProjectApplication*) pMain;
        while (!projMain->bStopObserve)
        {
            Thread::sleep(30);
            if ( GetForegroundWindow() != projMain->browserHwnd )
            {
                projMain->setTopmost(false);
                continue;
            }
            projMain->setTopmost(true);

            RECT rect;
            GetWindowRect(projMain->browserHwnd, &rect);
            
            if (IsIconic(projMain->browserHwnd))
            {
                projMain->hideWindow();
            }
            if (!WindowAccessHelper::IsSameRect(rect, projMain->browserRect))
            {
                projMain->onBrowserMoved();
                projMain->browserRect = rect;
            }
        }
        return 0;
    }
    
private:
    ScopedPointer<TransparentWnd> wnd;
    HWND              browserHwnd;
    HANDLE            hObserver;
    bool              bStopObserve;
    RECT              browserRect;
};


//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (NewProjectApplication)
