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

class BookmarkNode{
public:
    BookmarkNode(const String& n = String::empty)
        : name(n)
    {};

    ~BookmarkNode(){};

    void parseVar(var root)
    {
        if (!root.isArray())
        {
            return;
        }
        for (int i = 0; i < root.size(); ++i)
        {
            if (root[i]["content"].isUndefined())
            {
                val.push_back(JSON::toString(root[i], true));
            }
            else
            {
                std::shared_ptr<BookmarkNode> bn =
                    std::shared_ptr<BookmarkNode>(new BookmarkNode(root[i]["name"]));
                bn->parseVar(root[i]["content"]);
                content.push_back(bn);
            }
        }
    }

    var  toVar()
    {
        var root;
        if (name.isEmpty())
        {
            for (std::vector<String>::iterator vIt = val.begin(); 
                vIt != val.end(); ++vIt)
            {
                root.append(*vIt);
            }
            for (std::vector<std::shared_ptr<BookmarkNode> >::iterator vbIt = content.begin();
                vbIt != content.end(); ++vbIt)
            {
                root.append((*vbIt)->toVar());
            }
        }
        else
        {
            root = JSON::parse("{}");
            root.getDynamicObject()->setProperty("name", name);
            var troot;
            for (std::vector<String>::iterator vIt = val.begin(); 
                vIt != val.end(); ++vIt)
            {
                troot.append(*vIt);
            }
            for (std::vector<std::shared_ptr<BookmarkNode> >::iterator vbIt = content.begin();
                vbIt != content.end(); ++vbIt)
            {
                troot.append((*vbIt)->toVar());
            }
            root.getDynamicObject()->setProperty("content", troot);
        }
        return root;
    }

    String Name()
    {
        return name;
    }

    std::vector<std::shared_ptr<BookmarkNode> >& Content()
    {
        return content;
    }

    void addABookmark(std::vector<String> path, var varval)
    {
        if (name.isNotEmpty())
        {
            if (path.empty())
            {
                val.push_back(JSON::toString(varval, true));
                return;
            }
        if (path.size() == 1) 
        {
            if (name.compare(path.front()) != 0)
            {
                return;
            }
            val.push_back(JSON::toString(varval, true));
            return;
        }
        }
        for (std::vector<std::shared_ptr<BookmarkNode> >::iterator
            vit = content.begin(); vit != content.end(); ++vit)
        {
            if ((*vit)->Name().compare(path.front()) == 0)
            {
                path.erase(path.begin());
                return (*vit)->addABookmark(path, varval);
            }
        }
        std::shared_ptr<BookmarkNode> bknode = std::shared_ptr<BookmarkNode>( new BookmarkNode(path.front()) );
        path.erase(path.begin());
        bknode->addABookmark(path, varval);
    }

    void mergeFolders(BookmarkNode* folderNodes)
    {
        size_t curNum = content.size();
        std::vector<std::shared_ptr<BookmarkNode> > newNodes = folderNodes->Content();
        content.insert(content.end(), newNodes.begin(), newNodes.end());
        if (content.empty())
        {
            return;
        }
        for (size_t j = curNum; j < content.size(); ++j)
        {
            for (size_t i = 0; i < curNum; ++i)
            {
                if (content[j]->Name().compare(content[i]->Name()) == 0)
                {
                    content[i]->mergeFolders(content[j].get());
                    content.erase(content.begin() + j);
                    --j;
                    break;
                }
            }
        }
    }

    void removeABookmark(std::vector<String> path)
    {
        if (path.empty())
        {
            return;
        }
        
        if (path.size() == 1)
        {
            for (std::vector<String>::iterator itv = val.begin();
                itv != val.end(); ++itv)
            {
                if (path.front().compare(JSON::parse(*itv)["name"]) == 0)
                {
                    val.erase(itv);
                    return;
                }
            }
        }
        for (std::vector<std::shared_ptr<BookmarkNode> >::iterator itc = content.begin();
            itc != content.end(); ++itc)
        {
            if ((*itc)->Name().compare(path.front()) == 0)
            {
                path.erase(path.begin());
                return (*itc)->removeABookmark(path);
            }
        }
    }

private:
    String                  name;
    std::vector<String>     val;
    std::vector<std::shared_ptr<BookmarkNode> > content;
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
                parseChanges(bookmarkOrigin["changerecords"]);
            }
        }
        if (bookmarks == var())
        {
            var toolbar = JSON::parse("{}");
            toolbar.getDynamicObject()->setProperty("name", LoadDtdData::getInstance()->getEntityFromDtds("bookmark.toolbar"));
            toolbar.getDynamicObject()->setProperty("content", var());
            var noclass = JSON::parse("{}");
            noclass.getDynamicObject()->setProperty("name", LoadDtdData::getInstance()->getEntityFromDtds("bookmark.notclassify"));
            noclass.getDynamicObject()->setProperty("content", var());

            bookmarks.append(toolbar);
            bookmarks.append(noclass);

            bookmarkOrigin = JSON::parse("{}");
            bookmarkOrigin.getDynamicObject()->setProperty("base", bookmarks);
            bookmarkOrigin.getDynamicObject()->setProperty("changerecords", JSON::parse("{}"));
        }
    }

    var getBookmarkLists()
    {
        return bookmarks;
    }

    void addAnRecord(var changes)
    {
        if (bookmarkOrigin["changerecords"].isVoid())
        {
            bookmarkOrigin.getDynamicObject()->setProperty("changerecords", JSON::parse("{}"));
        }
        if (bookmarkOrigin["changerecords"]["add"].isVoid())
        {
            bookmarkOrigin["changerecords"].getDynamicObject()->setProperty("add", changes["add"]);
        }
        else
        {
            for (int i = 0; i < changes["add"].size(); ++i)
            {
                bookmarkOrigin["changerecords"]["add"].append(changes["add"][i]);
            }
        }
        parseChanges(changes);
    }

    void removeRecord(var changes)
    {
        if (bookmarkOrigin["changerecords"].isVoid())
        {
            bookmarkOrigin.getDynamicObject()->setProperty("changerecords", JSON::parse("{}"));
        }
        if (bookmarkOrigin["changerecords"]["removes"].isVoid())
        {
            bookmarkOrigin["changerecords"].getDynamicObject()->setProperty("removes", changes["removes"]);
        }
        else
        {
            for (int i = 0; i < changes["removes"].size(); ++i)
            {
                bookmarkOrigin["changerecords"]["removes"].append(changes["removes"][i]);
            }
        }
        parseChanges(changes);
    }

    bool bookmarkExist(var bookroot, std::vector<String> vecPath)
    {
        int len = bookroot.size();
        for (int i = 0; i < len; ++i)
        {
            if (bookroot[i]["name"].toString().compare(vecPath.front()) == 0)
            {
                vecPath.erase(vecPath.begin());
                return vecPath.empty() ? true : 
                    bookmarkExist(bookroot[i]["content"], vecPath);
            }
        }
        return false;
    }

    void parseChanges(var changes)
    {
        var varAdds = changes["add"];
        for (int i = 0; i < varAdds.size(); ++i)
        {
            String path = varAdds[i]["path"];
            String name = path.substring(path.lastIndexOf("\\") + 1);
            std::vector<String> vecPath;
            int startIdx = 0;
            int endIdx;
            while ((endIdx = path.indexOf(startIdx, "\\") ) > 0)
            {
                vecPath.push_back(path.substring(startIdx, endIdx));
                startIdx = endIdx + 1;
            }
            if (bookmarkExist(bookmarks, vecPath))
            {
                continue;
            }
            NamedValueSet vset = varAdds[i].getDynamicObject()->getProperties();
            var rec = JSON::parse("{}");
            rec.getDynamicObject()->setProperty("name", name);
            for (int j = 0; j < vset.size(); ++j)
            {
                String id = vset.getName(j).toString();
                if (id.compare("path") == 0 || id.compare("time") == 0)
                {
                    continue;
                }
                rec.getDynamicObject()->setProperty(vset.getName(j), vset.getValueAt(j));
            }
            vecPath.pop_back();
            addABookmark( vecPath, rec );
        }
        var varRemoves = changes["removes"];
        for (int i = 0; i < varRemoves.size(); ++i)
        {
            String path = varRemoves[i]["path"];
            std::vector<String> vecPath;
            int startIdx = 0;
            int endIdx;
            while ((endIdx = path.indexOf(startIdx, "\\") ) > 0)
            {
                vecPath.push_back(path.substring(startIdx, endIdx));
                startIdx = endIdx + 1;
            }
            if (!bookmarkExist(bookmarks, vecPath))
            {
                continue;
            }
            removeABookmark( vecPath);
        }
    }

    void addABookmark(std::vector<String> vecPath, var val)
    {
        if (vecPath.empty())
        {
            return;
        }
        BookmarkNode bnode;
        bnode.parseVar(bookmarks);
        bnode.addABookmark(vecPath, val);
        bookmarks = bnode.toVar();
    }

    void removeABookmark(std::vector<String> vecPath)
    {
        if (vecPath.empty())
        {
            return;
        }
        BookmarkNode bnode;
        bnode.parseVar(bookmarks);
        bnode.removeABookmark(vecPath);
        bookmarks = bnode.toVar();
    }

    void saveToFile(const String& strFile = String::empty )
    {
        saveToFile(bookmarkOrigin);
    }

    void saveToFile(var toSave, const String& strFile = String::empty)
    {
        filename = strFile.isNotEmpty() ? strFile : filename;
        File::getCurrentWorkingDirectory().getChildFile(filename)
            .replaceWithText(JSON::toString(toSave));
    }

    void updateBookmarkFolder(var newFolderTree)
    {
        BookmarkNode folderNodeBase;
        folderNodeBase.parseVar(bookmarkOrigin["base"]);
        BookmarkNode newFolder;
        newFolder.parseVar(newFolderTree);
        folderNodeBase.mergeFolders(&newFolder);
        bookmarkOrigin.getDynamicObject()->setProperty("base", folderNodeBase.toVar());
    }

    /*  合并更改到base，删除更改记录
    */
    void mergeChanges()
    {
        bookmarkOrigin.getDynamicObject()->setProperty("base", bookmarks);
        bookmarkOrigin.getDynamicObject()->setProperty("changerecords", JSON::parse("{}"));
    }

private:
    String   filename;
    var      bookmarkOrigin;
    var      bookmarks;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookmarkFileIO)
};

#endif  // BOOKMARKFILE_H_INCLUDED
