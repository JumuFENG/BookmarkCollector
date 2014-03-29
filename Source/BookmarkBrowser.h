/*
  ==============================================================================

    BookmarkBrowser.h
    Created: 26 Mar 2014 5:03:03pm
    Author:  terry

  ==============================================================================
*/

#ifndef BOOKMARKBROWSER_H_INCLUDED
#define BOOKMARKBROWSER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "BookmarkFile.h"

//==============================================================================
/*
*/

class FolderIcon : public Component
{
public:
	FolderIcon(Image* icoImg = nullptr) : img(icoImg)
	{
	}

	void setImage(Image* icoImg)
	{
		img = icoImg;
	}

	~FolderIcon()
	{

	}

	void paint(Graphics& g)
	{
		g.fillAll (Colours::white);   // clear the background
		if (img != nullptr)
		{
			g.drawImage(*img, 0, 0, getWidth(), getHeight(), 0, 0, 16, 16);
		}
	}
private:
	Image* img;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FolderIcon)
};

class BookmarkFolderContainer;

class BookmarkFolder
    : public ButtonListener
    , public Component
    , public LabelListener
{
public:
    BookmarkFolder(var itm, bool canRename = true);

    ~BookmarkFolder();

    void paint(Graphics& g);

    void resized();

    void showChildren();

    void buttonClicked(Button* btnThatClicked);

    void labelTextChanged(Label* labelThatHasChanged);

    void mouseUp(const MouseEvent& event);

    void mouseEnter(const MouseEvent& event);

    void mouseExit(const MouseEvent& event);


private:
    var    item;
    Image  img;
    ScopedPointer<ArrowButton> triBtn;
    ScopedPointer<FolderIcon>  icon;
    ScopedPointer<Label>       label;

    ScopedPointer<BookmarkFolderContainer> childFolders;

    int    iconWidth;
    int    triWidth, triHeight;
    bool   bExpanded;
    bool   bHover;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookmarkFolder)

};

class BookmarkFolderContainer    : public Component
{
public:
    BookmarkFolderContainer();
    BookmarkFolderContainer(var folders);
    ~BookmarkFolderContainer();

    void paint (Graphics&);
    void resized();
    void addChildrenFolders(var folders);

private:
	std::vector<ScopedPointer<BookmarkFolder> >  folderContainer;
    var  varfolders;   // array

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookmarkFolderContainer)
};


#endif  // BOOKMARKBROWSER_H_INCLUDED
