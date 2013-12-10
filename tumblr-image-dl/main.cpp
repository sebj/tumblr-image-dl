
//  main.cpp

//  Created by Seb Jachec on 08/12/2013.
//  Copyright (c) 2013 Seb Jachec. All rights reserved.

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <curl/curl.h>
#include "utilities.h"

using namespace std;

//Callback to write to string
//http://stackoverflow.com/a/1636827/447697
size_t strWriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

//Callback to write to file
//http://stackoverflow.com/a/1636415/447697
size_t writeCallback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

CURL* newCurl(string theURL) {
    CURL *newCurl;
    
    newCurl = curl_easy_init();
    if (newCurl) {
        curl_easy_setopt(newCurl, CURLOPT_URL, theURL.c_str());
    }
    
    return newCurl;
}

string tumblrImageData(string tumblrName, string tag) {
    CURL *tumblrCurl;
    string tumblrData = "";
    
    if (tumblrName != "") {
        if (tag == "") {
            tumblrCurl = newCurl("http://"+tumblrName+".tumblr.com/api/read?type=photo&num=50&filter=text");
        } else {
            tumblrCurl = newCurl("http://"+tumblrName+".tumblr.com/api/read?type=photo&num=50&filter=text&tagged="+tag);
        }
        
        CURLcode res;
        
        if (tumblrCurl) {
            curl_easy_setopt(tumblrCurl, CURLOPT_WRITEFUNCTION, strWriteCallback);
            curl_easy_setopt(tumblrCurl, CURLOPT_WRITEDATA, &tumblrData);
            res = curl_easy_perform(tumblrCurl);
            curl_easy_cleanup(tumblrCurl);
        }
    }
    
    return tumblrData;
}

//Download images from last 50 posts with tag "tag", from tumblr "tumblr", saving to "dest"
//Note: Downloads images at largest size available - scaled to 1280px wide
void saveImagesFromTumblr(string tumblr, string tag, string dest) {
    string tumblrData = tumblrImageData(tumblr, tag);
    
    if (tumblrData.length() > 0) {
        // http://[0-9]{2}\\.media\\.tumblr\\.com/.{30,33}/tumblr_.{19,22}_1280.(jpg|png|gif)
        
        // Regular:
        // http://[0-9]{2}\.media\.tumblr\.com/.{30,33}/tumblr_.{19,22}_1280.(jpg|png)
        
        regex word_regex("http://[0-9]{2}\\.media\\.tumblr\\.com/.{30,33}/tumblr_.{19,22}_100.(jpg|jpeg|png|gif)");
        auto words_begin = sregex_iterator(tumblrData.begin(), tumblrData.end(), word_regex);
        auto words_end = sregex_iterator();
        
        long fileCount = distance(words_begin, words_end);
        
        int fileNum = 0;
        
        for (sregex_iterator i = words_begin; i != words_end; ++i) {
            cout << "Downloading image " << fileNum+1 << "/" << fileCount << endl;
            
            smatch match = *i;
            string imageURL = match.str();
            
            //Figure out save location
            long lastDot = indexOfLast(imageURL, '.');
            string extension = imageURL.substr(lastDot);
            string filename = "tumblr_"+to_string(fileNum)+"."+extension;
            string saveLocation = appendToPath(dest, filename);
            
            
            //Download image
            CURL *imageCurl = newCurl(imageURL);
            CURLcode res;
            FILE *fp;
            
            if (imageCurl) {
                fp = fopen(saveLocation.c_str(), "w");
                
                curl_easy_setopt(imageCurl, CURLOPT_WRITEFUNCTION, writeCallback);
                curl_easy_setopt(imageCurl, CURLOPT_WRITEDATA, fp);
                res = curl_easy_perform(imageCurl);
                curl_easy_cleanup(imageCurl);
                
                fclose(fp);
            }
            
            fileNum++;
        }
        
        cout << endl << "Finished downloading images." << endl;
    }
}

void print_usage() {
    cout << "Usage: tumblrname (-t post-tag) -d /destination" << endl;
}

int main(int argc, const char * argv[]) {
    string tumblrName = "";
    string tag = "";
    string dest = "";
    
    if (argc >= 2) {
        //Get arguments
        string firstArg(argv[1]);
        string secondArg(argv[2]);
        string thirdArg = "";
        string fourthArg = "";
        string fifthArg = "";
        
        if (argc > 2) {
            thirdArg = string(argv[3]);
            fourthArg = string(argv[4]);
            fifthArg = string(argv[5]);
        }
        
        
        tumblrName = firstArg;
        
        // Tag argument //
        //-tag, --tag, -t, --t
        if (endsWith(secondArg, "tag") || endsWith(secondArg, "t")) {
            tag = thirdArg;
        } else if (endsWith(fourthArg,  "tag") || endsWith(fourthArg, "t")) {
            dest = thirdArg;
        }
        
        // Destination argument //
        //-destination, --destination, -d, --d
        if (endsWith(secondArg, "destination") || endsWith(secondArg, "d")) {
            dest = thirdArg;
        } else if (endsWith(fourthArg,  "destination") || endsWith(fourthArg, "d")) {
            dest = fifthArg;
        }
        
        if (dest == "") {
            cout << "No destination folder given" << endl;
            print_usage();
            exit(EXIT_FAILURE);
        }
        
    } else {
        print_usage();
        exit(EXIT_FAILURE);
    }
    
    string shouldContinue = "";
    cout << "This will download images in the highest size accessible - with a width of 1280px." << endl << endl << "Continue? (y|n) ";
    getline(cin, shouldContinue);
    cout << endl;
    
    if (shouldContinue == "yes" || shouldContinue == "y") {
        if (tumblrName != "" && dest != "") {
            saveImagesFromTumblr(tumblrName, tag, dest);
        } else {
            exit(EXIT_FAILURE);
        }
    } else {
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

