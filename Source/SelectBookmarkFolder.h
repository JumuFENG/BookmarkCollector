/*
  ==============================================================================

    SelectBookmarkFolder.h
    Created: 22 Mar 2014 4:45:14pm
    Author:  传峰

  ==============================================================================
*/

#ifndef SELECTBOOKMARKFOLDER_H_INCLUDED
#define SELECTBOOKMARKFOLDER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "BookmarkFile.h"
#include "BookMarkListener.h"
#include "BookmarkBrowser.h"

//==============================================================================
/*
*/
class SelectBookmarkFolder 
    : public Component
    , ComboBoxListener
    , ButtonListener
{
public:
    SelectBookmarkFolder() 
        : bGrownUp ( false)//const String& name
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        fillupFolderCombox();
        folderCombox.addListener(this);
        addAndMakeVisible(folderCombox);

        arrowBtn = new ArrowButton(String(L"arrowup"), 0.0f, Colour::fromRGBA(0x7b, 0x68, 0xee, 0x7f));//ff7b68ee
        addAndMakeVisible(arrowBtn);//Colours::mediumslateblue
        arrowBtn->addListener(this);

        viewPort = new Viewport();
        bookmarkFolderView = new BookmarkFolderContainer(BookmarkFileIO::getInstance()->getBookmarkLists());
        viewPort->setViewedComponent(bookmarkFolderView);
        addAndMakeVisible(viewPort);
     }

    ~SelectBookmarkFolder()
    {
    }

    void paint (Graphics& g)
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

        g.fillAll (Colour::fromRGBA(0xff, 0xff, 0xff, 0x7f));   // clear the background

    }

    void resized()
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        folderCombox.setBounds(1, 1, getWidth() - 40, 24);

        arrowBtn->setBounds(getWidth() - 30, 8, 25, 15);
        arrowBtn->changeDirectionTo( bGrownUp ? 0.75f : 0.25f );

        if (bGrownUp)
        {
            viewPort->setBoundsInset(BorderSize<int>(30, 1, 1, 1));
        }
        else
        {
            viewPort->setBounds(0, 0, 0, 0);
        }
        bookmarkFolderView->setSize(viewPort->getWidth() - (viewPort->isVerticalScrollBarShown() ? 20 : 2), bookmarkFolderView->getHeight());
    }

    void comboBoxChanged(ComboBox* comboBoxThatHasChanged)
    {
        if ( &folderCombox == comboBoxThatHasChanged)
        {
            if ( 3 == comboBoxThatHasChanged->getSelectedId() )
            {
                bGrownUp = true;
                reloadChildElments();
            }
        }
    }

    void buttonClicked(Button* btnThatClicked)
    {
        if (arrowBtn.get() == btnThatClicked)
        {
            bGrownUp = arrowBtn->getArrowDirection() == 0.25f;
            reloadChildElments();
            return;
        }

    }

    void fillupFolderCombox()
    {
        folderCombox.clear(dontSendNotification);
        folderCombox.addItem(LoadDtdData::getInstance()->getEntityFromDtds("bookmark.toolbar"), 1);
        folderCombox.addItem(LoadDtdData::getInstance()->getEntityFromDtds("bookmark.notclassify"), 2);
        if ( !bGrownUp )
        {
            folderCombox.addSeparator();
            folderCombox.addItem(LoadDtdData::getInstance()->getEntityFromDtds("combox.choose"), 3);
            folderCombox.addSeparator();
        }
        folderCombox.addItem(String(L"Folder 1"), 4);
        folderCombox.setEditableText(true);
        folderCombox.setSelectedId(1);
    }

    void reloadChildElments()
    {
        fillupFolderCombox();

        int ww = bGrownUp ? 460 : 278;
        int hh = bGrownUp ? 300 : 150;
        int iright = getParentComponent()->getRight();
        int iTop = getParentComponent()->getY();
        getParentComponent()->setBounds(iright - ww, iTop, ww, hh);
    }

    void testAddChild()
    {
//         var nf = JSON::parse("{}");
//         nf.getDynamicObject()->setProperty("name", "new one");
//         nf.getDynamicObject()->setProperty("content", String::empty);
//         var p; 
//         p.append(nf);
//         var newFolder = JSON::parse("{}");
//         newFolder.getDynamicObject()->setProperty("name", 
//             LoadDtdData::getInstance()->getEntityFromDtds("menu.newfolder"));
//         newFolder.getDynamicObject()->setProperty("content", String::empty);
//         var childFdr;
//         childFdr.append(newFolder);
//         bookmarkFolderView->addSubFolders(childFdr);
    }

private:
    bool     bGrownUp;
    ComboBox folderCombox;
    ScopedPointer<ArrowButton> arrowBtn;
    ScopedPointer<Viewport>    viewPort;
    ScopedPointer<BookmarkFolderContainer>  bookmarkFolderView;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SelectBookmarkFolder)
};


#endif  // SELECTBOOKMARKFOLDER_H_INCLUDED
