#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#ifdef __unix__
#include<dirent.h>
#endif
#ifdef _WIN32
#include<windows.h>
#endif

#include<common/Logging.h>

#include<colorer/ParserFactory.h>
#include<colorer/viewer/TextLinesStore.h>
#include<colorer/handlers/DefaultErrorHandler.h>
#include<colorer/handlers/FileErrorHandler.h>
#include<colorer/parsers/HRCParserImpl.h>
#include<colorer/parsers/TextParserImpl.h>
#include<common/io/InputSource.h>
#include<common/io/FileInputSource.h>

#ifndef __TIMESTAMP__
#define __TIMESTAMP__ "28 May 2006"
#endif


void ParserFactory::init()
{
    hrcParser = null;
    fileErrorHandler = null;
    const byte* stream = null;
    try
    {
        //catalogFIS = InputSource::newInstance(catalogPath);
        catalogFIS = new FileInputSource(catalogPath, null);
        catalog = docbuilder.parse(catalogFIS);        
    }
    catch (Exception& e)
    {
        throw ParserFactoryException(*e.getMessage());
    };

    Node* elem = catalog->getDocumentElement();

    if (elem == null || *elem->getNodeName() != "catalog")
    {
        throw ParserFactoryException(DString("bad catalog structure"));
    }

    elem = elem->getFirstChild();
    while (elem != null)
    {
        // hrc locations
        if (elem->getNodeType() == Node::ELEMENT_NODE &&
            *elem->getNodeName() == "hrc-sets")
        {

            const String* logLocation = ((Element*)elem)->getAttribute(DString("log-location"));

            if (logLocation != null)
            {
                InputSource* dfis = InputSource::newInstance(logLocation, catalogFIS);
                try
                {
                    fileErrorHandler = new FileErrorHandler(dfis->getLocation(), Encodings::ENC_UTF8, false);
                    colorer_logger_set_target(dfis->getLocation()->getChars());
                }
                catch (Exception& e)
                {
                    fileErrorHandler = null;
                };
                delete dfis;
            };

            if (fileErrorHandler == null)
            {
                fileErrorHandler = new DefaultErrorHandler();
            };

            Node* loc = elem->getFirstChild();
            while (loc != null)
            {
                if (loc->getNodeType() == Node::ELEMENT_NODE &&
                    *loc->getNodeName() == "location")
                {
                    hrcLocations.addElement(((Element*)loc)->getAttribute(DString("link")));
                }
                loc = loc->getNextSibling();
            }
        }
        // hrd locations
        if (elem->getNodeType() == Node::ELEMENT_NODE && *elem->getNodeName() == "hrd-sets")
        {
            Node* hrd = elem->getFirstChild();
            while (hrd != null)
            {
                if (hrd->getNodeType() == Node::ELEMENT_NODE && *hrd->getNodeName() == "hrd")
                {

                    const String* hrd_class = ((Element*)hrd)->getAttribute(DString("class"));
                    const String* hrd_name = ((Element*)hrd)->getAttribute(DString("name"));
                    if (hrd_class == null || hrd_name == null)
                    {
                        hrd = hrd->getNextSibling();
                        continue;
                    };

                    const String* hrd_descr = ((Element*)hrd)->getAttribute(DString("description"));
                    if (hrd_descr == null)
                    {
                        hrd_descr = hrd_name;
                    }
                    hrdDescriptions.put(&(StringBuffer(hrd_class) + "-" + hrd_name), hrd_descr);

                    Hashtable<Vector<const String*>*>* hrdClass = hrdLocations.get(hrd_class);
                    if (hrdClass == null)
                    {
                        hrdClass = new Hashtable<Vector<const String*>*>;
                        hrdLocations.put(hrd_class, hrdClass);
                        hrdClass->put(hrd_name, new Vector<const String*>);
                    };
                    Vector<const String*>* hrdLocV = hrdClass->get(hrd_name);
                    if (hrdLocV == null)
                    {
                        hrdLocV = new Vector<const String*>;
                        hrdClass->put(hrd_name, hrdLocV);
                    };

                    Node* loc = hrd->getFirstChild();
                    while (loc != null)
                    {
                        if (loc->getNodeType() == Node::ELEMENT_NODE && *loc->getNodeName() == "location")
                        {
                            hrdLocV->addElement(((Element*)loc)->getAttribute(DString("link")));
                        };
                        loc = loc->getNextSibling();
                    };
                };
                hrd = hrd->getNextSibling();
            };
        };
        elem = elem->getNextSibling();
    };
};

ParserFactory::ParserFactory(const String* catalogPath)
{
    fileErrorHandler = null;
    this->catalogPath = new SString(catalogPath);
    init();
};
ParserFactory::~ParserFactory()
{
    for (Hashtable<Vector<const String*>*>* hrdClass = hrdLocations.enumerate();
        hrdClass;
        hrdClass = hrdLocations.next())
    {
        for (Vector<const String*>* hrd_name = hrdClass->enumerate(); hrd_name; hrd_name = hrdClass->next())
        {
            delete hrd_name;
        };
        delete hrdClass;
    };
    docbuilder.free(catalog);
    delete hrcParser;
    delete catalogPath;
    delete catalogFIS;
    delete fileErrorHandler;
};

const char* ParserFactory::getVersion()
{
    return "Colorer-take5 Library be5 ";
};



const String* ParserFactory::enumerateHRDClasses(int idx)
{
    return hrdLocations.key(idx);
};
const String* ParserFactory::enumerateHRDInstances(const String& classID, int idx)
{
    Hashtable<Vector<const String*>*>* hash = hrdLocations.get(&classID);
    if (hash == null) return null;
    return hash->key(idx);
};
const String* ParserFactory::getHRDescription(const String& classID, const String& nameID)
{
    return hrdDescriptions.get(&(StringBuffer(classID) + "-" + nameID));
};

HRCParser* ParserFactory::getHRCParser()
{
    if (hrcParser != null) return hrcParser;
    hrcParser = new HRCParserImpl();
    hrcParser->setErrorHandler(fileErrorHandler);
    for (int idx = 0; idx < hrcLocations.size(); idx++)
    {
        if (hrcLocations.elementAt(idx) != null)
        {
            const String* relPath = hrcLocations.elementAt(idx);
            const String* path = null;
            if (InputSource::isRelative(relPath))
            {
                path = InputSource::getAbsolutePath(catalogPath, relPath);
                const String* path2del = path;
                if (path->startsWith(DString("file://"))) path = new SString(path, 7, -1);
                if (path->startsWith(DString("file:/"))) path = new SString(path, 6, -1);
                if (path->startsWith(DString("file:"))) path = new SString(path, 5, -1);
                if (path != path2del) delete path2del;
            }
            else
                path = new SString(relPath);

            struct stat st;
            int ret = stat(path->getChars(), &st);

            if (ret != -1 && (st.st_mode & S_IFDIR))
            {
#ifdef _WIN32
                WIN32_FIND_DATAA ffd;
                HANDLE dir = FindFirstFileA((StringBuffer(path) + "\\*.*").getChars(), &ffd);
                if (dir != INVALID_HANDLE_VALUE)
                {
                    while (true)
                    {
                        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                        {
                            InputSource* dfis = InputSource::newInstance(&(StringBuffer(relPath) + "\\" + ffd.cFileName), catalogFIS);
                            try
                            {
                                hrcParser->loadSource(dfis);
                                delete dfis;
                            }
                            catch (Exception& e)
                            {
                                if (fileErrorHandler != null)
                                {
                                    fileErrorHandler->fatalError(StringBuffer("Can't load hrc: ") + dfis->getLocation());
                                    fileErrorHandler->fatalError(*e.getMessage());
                                };
                                delete dfis;
                            };
                        };
                        if (FindNextFileA(dir, &ffd) == FALSE) break;
                    };
                    FindClose(dir);
                };
#endif
#ifdef __unix__
                DIR* dir = opendir(path->getChars());
                dirent* dire;
                if (dir != null)
                {
                    while ((dire = readdir(dir)) != null)
                    {
                        stat((StringBuffer(path) + "/" + dire->d_name).getChars(), &st);
                        if (!(st.st_mode & S_IFDIR))
                        {
                            InputSource* dfis = InputSource::newInstance(&(StringBuffer(relPath) + "/" + dire->d_name), catalogFIS);
                            try
                            {
                                hrcParser->loadSource(dfis);
                                delete dfis;
                            }
                            catch (Exception& e)
                            {
                                if (fileErrorHandler != null)
                                {
                                    fileErrorHandler->fatalError(StringBuffer("Can't load hrc: ") + dfis->getLocation());
                                    fileErrorHandler->fatalError(*e.getMessage());
                                }
                                delete dfis;
                            }
                        }
                    }
                }
#endif
            }
            else
            {
                try
                {
                    InputSource* dfis = InputSource::newInstance(hrcLocations.elementAt(idx), catalogFIS);
                    hrcParser->loadSource(dfis);
                    delete dfis;
                }
                catch (Exception& e)
                {
                    if (fileErrorHandler != null)
                    {
                        fileErrorHandler->fatalError(DString("Can't load hrc: "));
                        fileErrorHandler->fatalError(*e.getMessage());
                    };
                };
            };
            delete path;
        };
    };
    return hrcParser;
};

TextParser* ParserFactory::createTextParser()
{
    return new TextParserImpl();
};

StyledHRDMapper* ParserFactory::createStyledMapper(const String* classID, const String* nameID)
{
    Hashtable<Vector<const String*>*>* hrdClass = null;
    if (classID == null)
        hrdClass = hrdLocations.get(&DString("rgb"));
    else
        hrdClass = hrdLocations.get(classID);

    if (hrdClass == null)
        throw ParserFactoryException(StringBuffer("can't find hrdClass '") + classID + "'");

    Vector<const String*>* hrdLocV = null;
    if (nameID == null)
    {
        char* hrd = getenv("COLORER5HRD");
        hrdLocV = (hrd) ? hrdClass->get(&DString(hrd)) : hrdClass->get(&DString("default"));
        if (hrdLocV == null)
        {
            hrdLocV = hrdClass->get(&DString("default"));
        }
    }
    else
        hrdLocV = hrdClass->get(nameID);
    if (hrdLocV == null)
        throw ParserFactoryException(StringBuffer("can't find hrdName '") + nameID + "'");

    StyledHRDMapper* mapper = new StyledHRDMapper();
    for (int idx = 0; idx < hrdLocV->size(); idx++)
        if (hrdLocV->elementAt(idx) != null)
        {
            try
            {
                InputSource* dfis = InputSource::newInstance(hrdLocV->elementAt(idx), catalogFIS);
                mapper->loadRegionMappings(dfis);
                delete dfis;
            }
            catch (Exception& e)
            {
                if (fileErrorHandler != null)
                {
                    fileErrorHandler->error(DString("Can't load hrd: "));
                    fileErrorHandler->error(*e.getMessage());
                };
            };
        };
    return mapper;
};

TextHRDMapper* ParserFactory::createTextMapper(const String* nameID)
{
    // fixed class 'text'
    Hashtable<Vector<const String*>*>* hrdClass = hrdLocations.get(&DString("text"));
    if (hrdClass == null) throw ParserFactoryException(StringBuffer("can't find hrdClass 'text'"));

    Vector<const String*>* hrdLocV = null;
    if (nameID == null)
        hrdLocV = hrdClass->get(&DString("default"));
    else
        hrdLocV = hrdClass->get(nameID);
    if (hrdLocV == null)
        throw ParserFactoryException(StringBuffer("can't find hrdName '") + nameID + "'");

    TextHRDMapper* mapper = new TextHRDMapper();
    for (int idx = 0; idx < hrdLocV->size(); idx++)
        if (hrdLocV->elementAt(idx) != null)
        {
            try
            {
                InputSource* dfis = InputSource::newInstance(hrdLocV->elementAt(idx), catalogFIS);
                mapper->loadRegionMappings(dfis);
                delete dfis;
            }
            catch (Exception& e)
            {
                if (fileErrorHandler != null)
                {
                    fileErrorHandler->error(DString("Can't load hrd: "));
                    fileErrorHandler->error(*e.getMessage());
                };
            };
        };
    return mapper;
};

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Colorer Library.
 *
 * The Initial Developer of the Original Code is
 * Cail Lomecb <cail@nm.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
