/*
  ==============================================================================

    BookmarkAdder.h
    Created: 22 Mar 2014 1:11:30pm
    Author:  传峰

  ==============================================================================
*/

#ifndef BOOKMARKADDER_H_INCLUDED
#define BOOKMARKADDER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SelectBookmarkFolder.h"
#include "BookmarkFile.h"
#include "DeskWndObserver.h"

//==============================================================================
/*
*/
class BookmarkAdder    
    : public Component
    , public ButtonListener
{
public:
    BookmarkAdder() 
        : topSideHeigth(115)
        , bExpanded(false)
        , bookmarkListener(nullptr)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        TCHAR tName[MAX_PATH] = {0};
        DeskWndObserver::getInstance()->GetWindowTitle(tName);
        webName = String::fromUTF8(tName);
        webName = webName.substring(0, webName.indexOf(String(" - Mozilla Firefox")));

        url = DeskWndObserver::getInstance()->GetWebHref();
        if (!(url.startsWith("http://") || url.startsWith("https://")))
        {
            url = String("http://") + url;
        }

        setLookAndFeel(&v1);
        labelTitle.setFont(20.0f);
        labelTitle.setText(LoadDtdData::getInstance()->getEntityFromDtds("adder.title"), dontSendNotification);
        addAndMakeVisible(labelTitle);

        labelName.setFont(15.0f);
        labelName.setText(LoadDtdData::getInstance()->getEntityFromDtds("adder.name"), dontSendNotification);
        addAndMakeVisible(labelName);
        txtName.setText(webName);
        addAndMakeVisible(txtName);

        labelFolder.setFont(15.0f);
        labelFolder.setText(LoadDtdData::getInstance()->getEntityFromDtds("adder.folder"), dontSendNotification);//文件夹
        addAndMakeVisible(labelFolder);

        addAndMakeVisible(selectFolder);

        btnDel.setButtonText(LoadDtdData::getInstance()->getEntityFromDtds("adder.button.delete"));
        btnDel.addListener(this);
        btnDone.setButtonText(LoadDtdData::getInstance()->getEntityFromDtds("adder.button.done"));
        btnDone.addListener(this);
        btnCancle.setButtonText(LoadDtdData::getInstance()->getEntityFromDtds("adder.button.cancle"));
        btnCancle.addListener(this);
        addAndMakeVisible(btnDel);
        addAndMakeVisible(btnCancle);
        addAndMakeVisible(btnDone);
    }

    ~BookmarkAdder()
    {
        selectFolder.saveFolderTree();
    }

    void paint (Graphics& g)
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

        g.fillAll ( Colour::fromRGBA(0xf5, 0xf5, 0xf5, 0xcf) );   // clear the background

        g.setColour (Colours::grey);
        g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    }

    void resized()
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        labelTitle.setBounds(5, 5, 200, 30);
        labelName.setBounds(25, 40, leftSideWidth - 25, 25);
        txtName.setBounds(leftSideWidth + 5, 40, getWidth() - leftSideWidth - 20, 25);
        labelFolder.setBounds(10, 70, leftSideWidth - 10, 25);

        int btnY = getHeight() - 40;
        int btnX = getWidth() - 200;
        selectFolder.setBounds(leftSideWidth + 5, 70, 
            getWidth() - leftSideWidth - 20, getHeight() - topSideHeigth );//btnY - topSideHeigth
        btnDel.setBounds(btnX, btnY, 60, 25);
        btnDone.setBounds(btnX + 63, btnY, 60, 25);
        btnCancle.setBounds(btnX + 126, btnY, 60, 25);
    }

    void buttonClicked(Button* btnThatClicked)
    {
        if ( nullptr == bookmarkListener)
        {
            return;
        }
        if ( &btnDel == btnThatClicked )
        {
            var newRecord = JSON::parse("{}");
            newRecord.getDynamicObject()->setProperty("path", selectFolder.getSelectedFolderPath() + webName);
            newRecord.getDynamicObject()->setProperty("url", url);
            newRecord.getDynamicObject()->setProperty("time", Time::getCurrentTime().toString(true, false));
            var tVar; tVar.append(newRecord);
            var varNewRec = JSON::parse("{}");
            varNewRec.getDynamicObject()->setProperty("removes", tVar);
            BookmarkFileIO::getInstance()->removeRecord(varNewRec);
            bookmarkListener->onMessageTriggered(BookMarkListener::kDel);
        }
        else if ( &btnDone == btnThatClicked )
        {
            var newRecord = JSON::parse("{}");
            newRecord.getDynamicObject()->setProperty("path", selectFolder.getSelectedFolderPath() + webName);
            newRecord.getDynamicObject()->setProperty("url", url);
            newRecord.getDynamicObject()->setProperty("time", Time::getCurrentTime().toString(true, false));
            var tVar; tVar.append(newRecord);
            var varNewRec = JSON::parse("{}");
            varNewRec.getDynamicObject()->setProperty("add", tVar);
            BookmarkFileIO::getInstance()->addAnRecord(varNewRec);
            bookmarkListener->onMessageTriggered(BookMarkListener::kDone);
        }
        else if ( &btnCancle == btnThatClicked )
        {
            bookmarkListener->onMessageTriggered(BookMarkListener::kCancle);
        }
    }

    void addBookmarkListener(BookMarkListener* listener)
    {
        bookmarkListener = listener;
    }

private:
    const static int leftSideWidth = 70;
    bool  bExpanded;
    int   topSideHeigth;
    Label      labelTitle;
    Label      labelName;
    Label      labelFolder;
    TextButton btnDone;
    TextButton btnDel;
    TextButton btnCancle;
    TextEditor txtName;

    SelectBookmarkFolder selectFolder;

    BookMarkListener* bookmarkListener;

    LookAndFeel_V1 v1;

    String     webName;
    String     url;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookmarkAdder)

};


#endif  // BOOKMARKADDER_H_INCLUDED
