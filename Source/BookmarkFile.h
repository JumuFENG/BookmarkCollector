/*
  ==============================================================================

    BookmarkFile.h
    Created: 23 Mar 2014 9:08:33pm
    Author:  传峰

  ==============================================================================
*/

#ifndef BOOKMARKFILE_H_INCLUDED
#define BOOKMARKFILE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class BookmarkFileIO{
public:
    BookmarkFileIO(const String& file = String::empty)
        : filename(file)
    {
        if (filename.isNotEmpty())
        {
            bookmarkOrigin = JSON::parse(File(filename));
            bookmarks = bookmarkOrigin["base"];
            mergeChanges(bookmarkOrigin["changerecords"]);
        }
        else
        {
            bookmarks = JSON::parse("{}");
            bookmarks.getDynamicObject()->setProperty("bm_toolbar", String::empty);
            bookmarks.getDynamicObject()->setProperty("bm_notclassify", String::empty);

            bookmarkOrigin = JSON::parse("{}");
            bookmarkOrigin.getDynamicObject()->setProperty("base", bookmarks);
            bookmarkOrigin.getDynamicObject()->setProperty("changerecords", String::empty);
        }
    }

    BookmarkFileIO::~BookmarkFileIO()
    {

    }

    var getBookmarkLists()
    {
        return bookmarks;
    }

    void addAnRecord(var changes)
    {
        bookmarkOrigin["changerecords"].append(changes);
        mergeChanges(changes);
    }

    void mergeChanges(var changes)
    {

    }

    void saveToFile(const String& filename )
    {
        File file(filename);
        file.replaceWithText(JSON::toString(bookmarkOrigin));
    }


private:
    String   filename;
    var      bookmarkOrigin;
    var      bookmarks;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookmarkFileIO)
}
#endif  // BOOKMARKFILE_H_INCLUDED
