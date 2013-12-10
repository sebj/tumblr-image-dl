
//  utilities.cpp

//  Created by Seb Jachec on 09/12/2013.
//  Copyright (c) 2013 Seb Jachec. All rights reserved.


#include "utilities.h"

#pragma mark - Strings

long indexOfLast(string original, char toFind) {
    char str[1024];
    strcpy(str, original.c_str());
    
    char *pch;
    pch = strrchr(str,toFind);
    
    return pch-str+1;
}

//http://stackoverflow.com/a/18681293/447697
bool endsWith(const string &s, const string &suffix) {
    return s.rfind(suffix) == (s.size()-suffix.size());
}

void lowercase(string &str) {
	const unsigned long length = str.length();
    
	for (int i=0; i < length; ++i) {
		str[i] = tolower(str[i]);
	}
}

//http://stackoverflow.com/a/5252645/447697
string replaceChar(string text, char find, char replacement) {
    for (string::iterator it = text.begin(); it != text.end(); ++it) {
        if (*it == find) {
           *it = replacement;
        }
    }
    return text;
}

string appendToPath(string path, string toAppend) {
    string newPath = path;
    
    if (endsWith(newPath, "/")) {
        newPath += toAppend;
    } else {
        newPath += "/"+toAppend;
    }
    
    return newPath;
}

#pragma mark - Directories

//http://stackoverflow.com/a/2336245/447697
void makeDir(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;
    
    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    
    for (p = tmp + 1; *p; p++)
        if(*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    
    mkdir(tmp, S_IRWXU);
}