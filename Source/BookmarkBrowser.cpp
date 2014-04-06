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
    , bSelected(false)
    , sizeListener(nullptr)
    , selectedFolderListener(nullptr)
{
    if ( item["content"].toString().isNotEmpty() )
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
        label->addListener(this);
    }
    addAndMakeVisible(label);

    childFolders = new BookmarkFolderContainer();
    childFolders->addFolderChangeListener(this);
    addChildComponent(childFolders);
    if (!item["content"].isVoid())
    {
        childFolders->addChildrenFolders(item["content"]);
    }
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
    if (bHover || bSelected)
    {
        validRect = Rectangle<int>(0, 0, getWidth(), iconWidth + 2 );
        Point<int> mPos = getMouseXYRelative();
        if (validRect.contains(mPos) || bSelected)
        {
            g.setColour(Colours::blue.withAlpha(bSelected ? 0.35f : 0.15f));
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
    if (bExpanded)
    {
        childFolders->setVisible(true);
        childFolders->setTopLeftPosition(1 + triWidth, iconWidth + 3);
        childFolders->setSize(getWidth() - triWidth, childFolders->getHeight()); //设置合适的宽度
        childFolders->setNewSize();// 设置合适的高度
        if (childFolders->isContainerEmpty())
        {
            childFolders->addChildrenFolders(item["content"]);
        }
    }
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

void BookmarkFolder::onSelectedFolderChanged(std::vector<String> selectedFolder)
{
    selectedFolder.insert(selectedFolder.begin(), item["name"]);
    if (selectedFolder.empty() || item["name"].toString().isEmpty())
    {
        return;
    }
    ((BookmarkFolderContainer*)getParentComponent())->onSelectedFolderChanged(selectedFolder);
    bSelected = false;
}

void BookmarkFolder::addFolderChangeListener(BookMarkFolerListener* fLsner)
{
    selectedFolderListener = fLsner;
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

void BookmarkFolder::unSelected()
{
    bSelected = false;
    childFolders->unSelectedChildren();
}

void BookmarkFolder::labelTextChanged(Label* labelThatHasChanged)
{
    if (labelThatHasChanged == label)
    {
        item.getDynamicObject()->setProperty("name", label->getText());
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
    if (!validRect.contains(event.getEventRelativeTo(this).getMouseDownPosition()) )
    {
        return;
    }

    if (event.mods.isPopupMenu())//.getCurrentModifiers().
    {
        if (0 == item["name"].toString().compare(LoadDtdData::getInstance()->getEntityFromDtds("bookmark.notclassify")))
        {
            return;
        }
        Logger::writeToLog("to be shown Popup Menu");
        PopupMenu popMenu;
        popMenu.dismissAllActiveMenus();
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
    else
    {
        if (item["name"].toString().isEmpty())
        {
            return;
        }
        bSelected = true;
        childFolders->unSelectedChildren();
        std::vector<String> selected;
        selected.push_back(item["name"]);
        if (selectedFolderListener != nullptr)
        {
            selectedFolderListener->onSelectedFolderChanged(selected);
        }
        else
        {
            ((BookmarkFolderContainer*)getParentComponent())->onSelectedFolderChanged(selected);
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

var BookmarkFolder::getBookmarkFolderTree()
{
    var varChild = item;
    varChild.getDynamicObject()->setProperty("content", childFolders->getChildrenFolderTree());
    return varChild;
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
    , selectedFolderListener(nullptr)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

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

    int oldY = 3;
    for (int i = 0; i < varfolders.size(); ++i)
    {
        int acctHeight = folderContainer[i]->getAcctuallyHeight();
        folderContainer[i]->setBounds(3, oldY, getWidth() - 6, acctHeight);
        oldY += acctHeight;
    }
}

void BookmarkFolderContainer::addChildrenFolders(var folders)
{
    int vsize = folders.size();
    if (vsize <= 0)
    {
        return;
    }
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
    ((BookmarkFolder*)getParentComponent())->showChildren();
    setNewSize();
}

bool BookmarkFolderContainer::isContainerEmpty()
{
    return folderContainer.empty();
}

void BookmarkFolderContainer::setNewSize()
{
    int h = 6;
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

void BookmarkFolderContainer::unSelectedChildren()
{
    for (std::vector<ScopedPointer<BookmarkFolder> >::iterator it = folderContainer.begin();
        it != folderContainer.end(); ++it)
    {
        if (! (*it)->getBounds().contains(getMouseXYRelative()))
        {
            (*it)->unSelected();
        }
    }
}

void BookmarkFolderContainer::onSelectedFolderChanged(std::vector<String> selectedFolder)
{
    unSelectedChildren();
    if (selectedFolderListener != nullptr)
    {
        selectedFolderListener->onSelectedFolderChanged(selectedFolder);
    }
    else
    {
        ((BookmarkFolder*)getParentComponent())->onSelectedFolderChanged(selectedFolder);
    }
}

void BookmarkFolderContainer::addFolderChangeListener(BookMarkFolerListener* fLsner)
{
    selectedFolderListener = fLsner;
}

var BookmarkFolderContainer::getChildrenFolderTree()
{
    var varChild;
    for (std::vector<ScopedPointer<BookmarkFolder> >::iterator it = folderContainer.begin();
        it != folderContainer.end(); ++it)
    {
        varChild.append((*it)->getBookmarkFolderTree());
    }
    return varChild;
}