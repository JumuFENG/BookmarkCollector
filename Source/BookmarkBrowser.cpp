/*
  ==============================================================================

    BookmarkBrowser.cpp
    Created: 26 Mar 2014 5:03:03pm
    Author:  terry

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "BookmarkBrowser.h"

//==============================================================================
BookmarkFolder::BookmarkFolder(var itm, bool canRename) 
    : item(itm)
    , iconWidth(16)
    , triWidth(7)
    , triHeight(10)
    , bExpanded(false)
    , bHover(false)
{
    if ( BookmarkFileIO::getInstance()->isContentHasSubfolder(item["content"]))
    {
        triBtn = new ArrowButton(String("arrowL"), 0.0f, Colour::fromRGBA(0x7b, 0x68, 0xee, 0x7f));
        triBtn->addListener(this);
        addAndMakeVisible(triBtn);
    }

    img  = ImageFileFormat::loadFrom(File::getCurrentWorkingDirectory().getChildFile("Resource\\folder.png"));
    icon = new FolderIcon(&img);
    addAndMakeVisible(icon);

    label = new Label(String::empty, item["name"]);
    if (canRename)
    {
        label->setEditable(false, true);
    }
    addAndMakeVisible(label);

    childFolders = new BookmarkFolderContainer();

    addMouseListener(this, true);
}

BookmarkFolder::~BookmarkFolder()
{
    childFolders = nullptr;
}

void BookmarkFolder::paint(Graphics& g)
{
    g.fillAll(Colours::white);
    if (bHover)
    {
        g.fillAll(Colours::blue.withAlpha(0.15f));
        g.setColour(Colours::lightgrey);
        g.drawRect(getLocalBounds(), 1);
    }
}

void BookmarkFolder::resized()
{
    if (triBtn != nullptr)
    {
        Rectangle<int> triRect(1, 3 + (iconWidth - triHeight) / 2, triWidth, triHeight);
        Rectangle<int> triAnRect(1 - (triHeight - triWidth)/2, 3 + (iconWidth - triWidth) /2, triHeight, triWidth);
        triBtn->setBounds(bExpanded ? triAnRect : triRect);
        triBtn->changeDirectionTo(bExpanded ? 0.25f : 0.0f);
    }

    icon->setBounds(3 + triWidth, 1, iconWidth, iconWidth);
    label->setBounds(5 + triWidth + iconWidth, 1, getWidth() - 10 - triWidth - iconWidth, iconWidth);
}

void BookmarkFolder::showChildren()
{
    childFolders->addChildrenFolders(item["content"]);
}

void BookmarkFolder::buttonClicked(Button* btnThatClicked)
{
    if (btnThatClicked == triBtn)
    {
        bExpanded = triBtn->getArrowDirection() == 0.0f;
        showChildren();
    }
}

void BookmarkFolder::labelTextChanged(Label* labelThatHasChanged)
{
    if (labelThatHasChanged == label)
    {
        item["name"] = label->getText();
    }
}

void BookmarkFolder::mouseUp(const MouseEvent& event)
{
    if (event.mods.isPopupMenu())//.getCurrentModifiers().
    {
        PopupMenu popMenu;
        popMenu.addItem(3101, LoadDtdData::getInstance()->getEntityFromDtds("menu.newfolder"));
        if (3101 == popMenu.show())
        {
            var newFolder = JSON::parse("{}");
            newFolder.getDynamicObject()->setProperty("name", 
                LoadDtdData::getInstance()->getEntityFromDtds("menu.newfolder"));
            newFolder.getDynamicObject()->setProperty("content", String::empty);
            childFolders->addChildrenFolders(newFolder);
        }
    }
}

void BookmarkFolder::mouseEnter(const MouseEvent& event)
{
    bHover = true;
    repaint();
}

void BookmarkFolder::mouseExit(const MouseEvent& event)
{
    bHover = false;
    repaint();
}

//=======================================================================
BookmarkFolderContainer::BookmarkFolderContainer()
{

}

BookmarkFolderContainer::BookmarkFolderContainer(var folders) 
    : varfolders(folders)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

//     var ufolder;
// 
//     var uitem = JSON::parse("{}");
//     uitem.getDynamicObject()->setProperty("name", "baidu");
//     uitem.getDynamicObject()->setProperty("url", "www.baidu.com");
//     for (int i = 0; i < 5; ++i)
//     {
//         ufolder.append(uitem);
//     }
// 
//     var fitem = JSON::parse("{}");
//     fitem.getDynamicObject()->setProperty("name", "hello");
//     fitem.getDynamicObject()->setProperty("content", ufolder);
// 
//     var chItem;
//     for (int i = 0; i < 3; ++i)
//     {
//         chItem.append(fitem);
//     }
//     for (int i = 0; i < 5; ++i)
//     {
//         chItem.append(uitem);
//     }
//     var chFolder = JSON::parse("{}");
//     chFolder.getDynamicObject()->setProperty("name", "toolbar");
//     chFolder.getDynamicObject()->setProperty("content",chItem);
//     varfolders.append(chFolder);
// 
//     var chNoclass = JSON::parse("{}");
//     chNoclass.getDynamicObject()->setProperty("name", "noclassify");
//     chNoclass.getDynamicObject()->setProperty("content",chItem);
//     varfolders.append(chNoclass);

    int vsize = varfolders.size();
    for (int i = 0; i < vsize; ++i)
    {
        folderContainer.push_back(ScopedPointer<BookmarkFolder>(new BookmarkFolder(varfolders[i], false)));
    }
    for (int i = 0; i < vsize; ++i)
    {
        addAndMakeVisible(folderContainer[i]);
    }

    setSize(100, 100); // 主要目的设置高度，宽度Viewport确定
}

BookmarkFolderContainer::~BookmarkFolderContainer()
{
}

void BookmarkFolderContainer::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (Colours::white);   // clear the background
}

void BookmarkFolderContainer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	int iconWidth = jmin<int>(20, getWidth());
	int iconHeigth = jmin<int>(20, getHeight());

    for (int i = 0; i < varfolders.size(); ++i)
    {
        folderContainer[i]->setBounds(3, 3 + iconHeigth * i, getWidth() - 6, iconHeigth);
    }
}

void BookmarkFolderContainer::addChildrenFolders(var folders)
{
    int vsize = folders.size();
    for (int i = 0; i < vsize; ++i)
    {
        varfolders.append(folders[i]);
        folderContainer.push_back(ScopedPointer<BookmarkFolder>(new BookmarkFolder(folders[i])));
    }
    for (size_t i = 0; i < folderContainer.size(); ++i)
    {
        addAndMakeVisible(folderContainer[i]);
    }
}
