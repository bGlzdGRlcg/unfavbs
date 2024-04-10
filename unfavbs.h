#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <zlib.h>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <map>
#include <io.h>

using namespace std;
using namespace cv;

unsigned int readbinhelf(ifstream &binfile){
	unsigned short int val;
	binfile.read((char *) &val, 2);
	return val;
}

unsigned int readbin(ifstream &binfile){
	unsigned int val;
	binfile.read((char *) &val, 4);
	return val;
}

unsigned int readpng(istringstream &pngfile){
	unsigned int val;
	pngfile.read((char *) &val, 4);
	return val;
}

unsigned int readpnghelf(istringstream &pngfile){
	unsigned short int val;
	pngfile.read((char *) &val, 2);
	return val;
}

string ShiftJISToUTF8(const string& shiftjis) {
    int len = MultiByteToWideChar(932,0,shiftjis.c_str(),-1,NULL,0);
    if (len == 0) return "";
    wstring wstr(len,0);
    MultiByteToWideChar(932,0,shiftjis.c_str(),-1,&wstr[0],len);
    len = WideCharToMultiByte(CP_UTF8,0,&wstr[0],-1,NULL,0,NULL,NULL);
    if (len == 0) return "";
    std::string utf8(len,0);
    WideCharToMultiByte(CP_UTF8,0,&wstr[0],-1,&utf8[0],len,NULL,NULL);
    return utf8;
}

string ShiftJISToGBK(const string& shiftjis) {
    int len = MultiByteToWideChar(932, 0, shiftjis.c_str(), -1, NULL, 0);
    if (len == 0) return "";
    std::wstring wstr(len, 0);
    MultiByteToWideChar(932, 0, shiftjis.c_str(), -1, &wstr[0], len);
    len = WideCharToMultiByte(936, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (len == 0) return "";
    string gbk(len, 0);
    WideCharToMultiByte(936, 0, wstr.c_str(), -1, &gbk[0], len, NULL, NULL);
    return gbk;
}

string getbinname(ifstream &binfile){
	string str="";
	char ch;
    while (binfile.get(ch)) {
        if (int(ch) == 0) break;
        str += ch;
    }
    return ShiftJISToGBK(str);
}

bool cmp(int a[],int b[]) {
    return a[0] < b[0];
}

void unbinout(ifstream &binfile,ofstream &outfile, streampos start, streampos end) {
    binfile.seekg(start);
    char buffer[1024];
    streampos remaining = end - start;
    while (remaining > 0) {
        size_t bytesToRead = min(remaining, static_cast<streampos>(sizeof(buffer)));
        binfile.read(buffer, bytesToRead);
        outfile.write(buffer, bytesToRead);
        remaining -= bytesToRead;
    }
    outfile.close();
}

istringstream decompressData(ifstream &inputFile, int startOffset, int uncompressedSize) {
    inputFile.seekg(startOffset);
    vector<char> buffer(uncompressedSize);
    inputFile.read(buffer.data(), uncompressedSize);
    vector<char> uncompressedBuffer(uncompressedSize * 2);
    uLongf destLen = uncompressedSize * 2;
    int result = uncompress((Bytef*)uncompressedBuffer.data(), &destLen, (const Bytef*)buffer.data(), uncompressedSize);
    if(result != Z_OK) {
        cerr<<"Failed to decompress data. Error code: " <<result<<endl;
        return istringstream();
        //return "";
    }
    string decompressedData(uncompressedBuffer.begin(), uncompressedBuffer.begin() + destLen);
    //return decompressedData;
	istringstream iss(decompressedData);
    return iss;
}

void overlayImages(const Mat &background, const Mat &foreground, Mat &output, int x, int y){
    output = background.clone();
    cv::Rect roi(x, y, foreground.cols, foreground.rows);
    roi &= cv::Rect(0, 0, background.cols, background.rows);
    cv::Mat roi_output = output(roi);
    cv::Mat roi_foreground = foreground(cv::Rect(0, 0, roi.width, roi.height));
    for (int i = 0; i < roi.height; ++i){
        for (int j = 0; j < roi.width; ++j){
            cv::Vec4b pixel_foreground = roi_foreground.at<cv::Vec4b>(i, j);
            cv::Vec4b &pixel_output = roi_output.at<cv::Vec4b>(i, j);
            double alpha_foreground = pixel_foreground[3] / 255.0;
            double alpha_background = 1.0 - alpha_foreground;
            for (int k = 0; k < 3; ++k){
                pixel_output[k] = static_cast<uchar>(alpha_foreground * pixel_foreground[k] + alpha_background * pixel_output[k]);
            }
            pixel_output[3] = static_cast<uchar>((alpha_foreground * 255) + (alpha_background * pixel_output[3]));
        }
    }
}

