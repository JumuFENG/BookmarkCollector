/*
  ==============================================================================

    BookMarkListener.h
    Created: 22 Mar 2014 6:31:05pm
    Author:  传峰

  ==============================================================================
*/

#ifndef BOOKMARKLISTENER_H_INCLUDED
#define BOOKMARKLISTENER_H_INCLUDED

class BookMarkListener{
public:
    BookMarkListener(){}
    virtual ~BookMarkListener(){}
    enum BookmarkMessageType{
        kCancle,
        kDone,
        kDel,
        kRefreshUI
    };
    virtual void onMessageTriggered(BookmarkMessageType tp, const String& cmdMsg = String::empty) = 0;
};

class BookMarkFolerListener{
public:
    BookMarkFolerListener(){}
    virtual ~BookMarkFolerListener(){}

    virtual void onSelectedFolderChanged(std::vector<String> curSelected) = 0;
        
};

#endif  // BOOKMARKLISTENER_H_INCLUDED
