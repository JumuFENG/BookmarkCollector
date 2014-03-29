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
#include <vector>


class LoadDtdData{
public:
    LoadDtdData(){}

    ~LoadDtdData(){}

    juce_DeclareSingleton(LoadDtdData, false);

    void parseDtdFile(const String& file)
    {
        if (file.isEmpty())
        {
            return;
        }
        dtdStrings = File::getCurrentWorkingDirectory().getChildFile(file).loadFileAsString();
    }

    String getEntityFromDtds(const String& prop)
    {
        String src = LoadDtdData::dtdStrings;
        int startPos = src.indexOf(prop);
        startPos = src.indexOf(startPos + 1, "\"") + 1;
        size_t endPos = src.indexOf(startPos + 1, "\"");
        return src.substring(startPos, endPos);
    }

private:
    String dtdStrings;
};

class BookmarkFileIO {
public:
    BookmarkFileIO(){}
    ~BookmarkFileIO(){}

    juce_DeclareSingleton(BookmarkFileIO, false);

    void init(const String& file = String::empty)
    {
        filename = file;
        if (filename.isNotEmpty())
        {
            File jsonFile = File::getCurrentWorkingDirectory().getChildFile(filename);
            if (jsonFile.exists())
            {
                bookmarkOrigin = JSON::parse(jsonFile);
                bookmarks = bookmarkOrigin["base"];
                mergeChanges(bookmarkOrigin["changerecords"]);
            }
        }
        if (bookmarks == var())
        {
            var toolbar = JSON::parse("{}");
            toolbar.getDynamicObject()->setProperty("name", LoadDtdData::getInstance()->getEntityFromDtds("bookmark.toolbar"));
            toolbar.getDynamicObject()->setProperty("content", String::empty);
            var noclass = JSON::parse("{}");
            noclass.getDynamicObject()->setProperty("name", LoadDtdData::getInstance()->getEntityFromDtds("bookmark.notclassify"));
            noclass.getDynamicObject()->setProperty("content", String::empty);

            bookmarks.append(toolbar);
            bookmarks.append(noclass);

            bookmarkOrigin = JSON::parse("{}");
            bookmarkOrigin.getDynamicObject()->setProperty("base", bookmarks);
            bookmarkOrigin.getDynamicObject()->setProperty("changerecords", String::empty);
        }
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

    void saveToFile(const String& strFile = String::empty )
    {
        filename = strFile.isNotEmpty() ? strFile : filename;
        File::getCurrentWorkingDirectory().getChildFile(filename)
            .replaceWithText(JSON::toString(bookmarkOrigin));
    }

    bool isContentHasSubfolder(var content)
    {
        int cSize = content.size();
        bool beFolder = false;
        for (int i = 0; i < cSize; ++i)
        {
            if (content[i]["content"].isArray())
            {
                beFolder = true;
                break;
            }
        }
        return beFolder;
    }

private:
    String   filename;
    var      bookmarkOrigin;
    var      bookmarks;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookmarkFileIO)
};

#endif  // BOOKMARKFILE_H_INCLUDED
