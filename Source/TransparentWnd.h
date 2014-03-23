/*
  ==============================================================================

    TransparentWnd.h
    Created: 22 Mar 2014 10:31:34am
    Author:  传峰

  ==============================================================================
*/

#ifndef TRANSPARENTWND_H_INCLUDED
#define TRANSPARENTWND_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "BookmarkAdder.h"

//==============================================================================
/*
*/

class TransparentWnd    
    : public Component
    , public BookMarkListener
{
public:
    TransparentWnd()
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        setSize(300, 50);
        bShowBorder = false;
    }

    ~TransparentWnd()
    {
    }

    void paint (Graphics& g)
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */
        
        g.fillAll ( Colour::fromRGBA(0xff, 0xff, 0xff, 0x01) );   // clear the background
        g.fillAll ( Colours::whitesmoke );
        if (bShowBorder)
        {
            g.setColour (Colours::grey);
            g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
        }

        g.setColour (Colours::lightblue);
        g.setFont (14.0f);
    }

    void resized()
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        setBounds(1206, 36, 23, 16);//(28,20)(20,25)
//        setBounds(0, 0, 500, 500);
    }

    void mouseEnter(const MouseEvent&)
    {
        bShowBorder = true;
    }

    void mouseExit(const MouseEvent&)
    {
        bShowBorder = false;
    }

    void mouseUp(const MouseEvent& event) override
    {
        if (event.mods.isPopupMenu())
        {
            PopupMenu popup;
            popup.addItem( 3001, String( L"Exit" ) );
//            popup.addItem( 3001, String( L"退出" ) );
            int iRet = popup.show();
            switch (iRet)
            {
            case 3001:
                // 退出事件
                JUCEApplication::getInstance()->systemRequestedQuit();
                break;
            default:
                break;
            }
            return;
        }
        bookmarkAdder = nullptr;
        bookmarkAdder = std::shared_ptr<BookmarkAdder>(new BookmarkAdder()) ;
        bookmarkAdder->addBookmarkListener(this);
        bookmarkAdder->addToDesktop(ComponentPeer::windowIsTemporary);
        bookmarkAdder->setSize(278, 150);
        bookmarkAdder->setTopRightPosition(getX() + getWidth(), getY() + getHeight());
        bookmarkAdder->setVisible(true);
    }

    void mouseMove(const MouseEvent&)
    {
        
    }

    void onMessageTriggered(BookmarkMessageType tp, const String& cmdMsg) override
    {
        switch (tp)
        {
        case BookMarkListener::kDone:
        case BookMarkListener::kDel:
        case BookMarkListener::kCancle:
            bookmarkAdder.reset();
            break;
        }
    }

private:
    std::shared_ptr<BookmarkAdder> bookmarkAdder;
    bool  bShowBorder;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransparentWnd)
};


#endif  // TRANSPARENTWND_H_INCLUDED
