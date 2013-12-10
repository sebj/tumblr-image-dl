
//  utilities.h

//  Created by Seb Jachec on 09/12/2013.
//  Copyright (c) 2013 Seb Jachec. All rights reserved.


#ifndef __sj__utilities__
#define __sj__utilities__
#endif /* defined(__sj__utilities__) */

#define write(f,s) f << s << endl

#include <string>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

long indexOfLast(string original, char toFind);
bool endsWith(const string &s, const string &suffix);
void lowercase(string &str);
string replaceChar(string text, char find, char replacement) ;

string appendToPath(string path, string toAppend);

void makeDir(const char *dir);