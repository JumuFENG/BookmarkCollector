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
    , sizeListener(nullptr)
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
    addChildComponent(childFolders);
    childFolders->addSizeChangeListener(this);
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
        validRect = Rectangle<int>(0, 0, getWidth(), iconWidth + 2 );
        Point<int> mPos = getMouseXYRelative();
        if (validRect.contains(mPos))
        {
            g.setColour(Colours::blue.withAlpha(0.15f));
            g.fillRect(validRect);
            g.setColour(Colours::lightgrey);
            g.drawRect(validRect, 1);
        }
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
    if (getHeight() <= 0)
    {
        setSize(getWidth(), iconWidth);
    }
}

void BookmarkFolder::showChildren()
{
    childFolders->setVisible(true);
    childFolders->setTopLeftPosition(1 + triWidth, iconWidth + 3);
    childFolders->setSize(getWidth() - triWidth, 100);
    childFolders->addChildrenFolders(item["content"]);
}

void BookmarkFolder::hideChildren()
{
    childFolders->setBounds(0, 0, 0, 0);
    childFolders->setVisible(false);
}

void BookmarkFolder::onNewSize()
{
    setNewSize();
}

void BookmarkFolder::addSizeChangeListener(SizeChangeListener* lsn)
{
    sizeListener = lsn;
}

void BookmarkFolder::setNewSize()
{
    setSize(getWidth(), getAcctuallyHeight());
    if (sizeListener != nullptr)
    {
        sizeListener->onNewSize();
    }
}

void BookmarkFolder::buttonClicked(Button* btnThatClicked)
{
    if (btnThatClicked == triBtn)
    {
        bExpanded = triBtn->getArrowDirection() == 0.0f;
    }
    bExpanded ? showChildren() : hideChildren();
    setNewSize();
}

void BookmarkFolder::labelTextChanged(Label* labelThatHasChanged)
{
    if (labelThatHasChanged == label)
    {
        item["name"] = label->getText();
    }
}

void BookmarkFolder::addSubFolders(var sfdr)
{
    childFolders->addChildrenFolders(sfdr);
    if (triBtn == nullptr)
    {
        triBtn = new ArrowButton(String("arrowL"), 0.0f, Colour::fromRGBA(0x7b, 0x68, 0xee, 0x7f));
        triBtn->addListener(this);
        addAndMakeVisible(triBtn);
        resized();
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
            var childFdr;
            childFdr.append(newFolder);
            addSubFolders(childFdr);
        }
    }
}

void BookmarkFolder::mouseDoubleClick(const MouseEvent& event)
{
//     if (triBtn != nullptr && validRect.contains(event.getPosition()))
//     {
//         buttonClicked(triBtn);
//     }
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

bool BookmarkFolder::isExpanded()
{
    return bExpanded;
}

int BookmarkFolder::getAcctuallyHeight()
{
    return bExpanded ? iconWidth + childFolders->getHeight() : iconWidth + 3;
}

//=======================================================================
BookmarkFolderContainer::BookmarkFolderContainer()
    : sizeListener(nullptr)
{
    setSize(100,100);
}

BookmarkFolderContainer::BookmarkFolderContainer(var folders) 
    : varfolders(folders)
    , sizeListener(nullptr)
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
        folderContainer[i]->addSizeChangeListener(this);
    }

    setSize(100,100);// 主要目的设置高度，宽度Viewport确定
    setNewSize(); 
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

    int oldY = 3;
    for (int i = 0; i < varfolders.size(); ++i)
    {
        folderContainer[i]->setBounds(3, oldY, getWidth() - 6, iconHeigth);
        oldY += folderContainer[i]->getAcctuallyHeight();
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
        folderContainer[i]->addSizeChangeListener(this);
    }
    setNewSize();
}

void BookmarkFolderContainer::setNewSize()
{
    int h = 4;
    for (size_t fsize = 0;  fsize < folderContainer.size(); ++fsize)
    {
        int nH = folderContainer[fsize]->getAcctuallyHeight();
        h += nH > 0 ? nH : 16;
    }
    setSize(getWidth(), h);
    if (sizeListener != nullptr)
    {
        sizeListener->onNewSize();
    }
}

void BookmarkFolderContainer::addSizeChangeListener(SizeChangeListener* lsn)
{
    sizeListener = lsn;
}

void BookmarkFolderContainer::onNewSize()
{
    setNewSize();
}
