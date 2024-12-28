#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <zlib.h>
#include <filesystem>
#include <iconv.h>

#define fn inline const auto
#define let const auto

inline std::string ShiftJISToUTF8(const std::string& shiftjis) {
    if (shiftjis.empty()) {
        return "";
    }
    iconv_t conv = iconv_open("UTF-8", "SHIFT-JIS");
    if (conv == (iconv_t)-1) {
        std::cerr << "iconv_open failed: " << strerror(errno) << std::endl;
        return "";
    }
    std::unique_ptr<void, decltype(&iconv_close)> conv_guard(conv, iconv_close);
    size_t in_left = shiftjis.length();
    size_t out_size = in_left * 3 + 1;
    std::string result(out_size, '\0');
    char* in_buf = const_cast<char*>(shiftjis.c_str());
    char* out_buf = &result[0];
    size_t out_left = out_size;
    while (in_left > 0) {
        size_t ret = iconv(conv, &in_buf, &in_left, &out_buf, &out_left);
        if (ret == (size_t)-1) {
            if (errno == E2BIG) {
                size_t curr_size = result.size();
                result.resize(curr_size + in_left * 3);
                out_buf = &result[curr_size - out_left];
                out_left += in_left * 3;
                continue;
            }
            std::cerr << "iconv failed: " << strerror(errno) << std::endl;
            return "";
        }
    }
    result.resize(out_size - out_left);
    return result;
}

fn readbinhelf(std::ifstream &binfile){
	unsigned short int val;
	binfile.read((char *) &val, 2);
	return val;
}

fn readbin(std::ifstream &binfile){
	unsigned int val;
	binfile.read((char *) &val, 4);
	return val;
}

fn readpng(std::istringstream &pngfile){
	unsigned int val;
	pngfile.read((char *) &val, 4);
	return val;
}

fn readpnghelf(std::istringstream &pngfile){
	unsigned short int val;
	pngfile.read((char *) &val, 2);
	return val;
}

fn getbinname(std::ifstream &binfile){
	std::string str="";
	char ch;
    while (binfile.get(ch)) {
        if (int(ch) == 0) break;
        str += ch;
    }
    return ShiftJISToUTF8(str);
}

fn cmp(int a[],int b[]) {
    return a[0] < b[0];
}

fn unbinout(std::ifstream &binfile,std::ofstream &outfile, std::streampos start, std::streampos end) {
    binfile.seekg(start);
    char buffer[1024];
    std::streampos remaining = end - start;
    while (remaining > 0) {
        size_t bytesToRead = min(remaining, static_cast<std::streampos>(sizeof(buffer)));
        binfile.read(buffer, bytesToRead);
        outfile.write(buffer, bytesToRead);
        remaining -= bytesToRead;
    }
    outfile.close();
}

fn decompressData(std::ifstream &inputFile, int startOffset, int uncompressedSize) {
    inputFile.seekg(startOffset);
    std::vector<char> buffer(uncompressedSize);
    inputFile.read(buffer.data(), uncompressedSize);
    std::vector<char> uncompressedBuffer(uncompressedSize * 2);
    uLongf destLen = uncompressedSize * 2;
    int result = uncompress((Bytef*)uncompressedBuffer.data(), &destLen, (const Bytef*)buffer.data(), uncompressedSize);
    if(result != Z_OK) {
        std::cerr<<"Failed to decompress data. Error code: " <<result<<std::endl;
        return std::istringstream();
        //return "";
    }
    std::string decompressedData(uncompressedBuffer.begin(), uncompressedBuffer.begin() + destLen);
    //return decompressedData;
	std::istringstream iss(decompressedData);
    return iss;
}

fn overlayImages(const cv::Mat &background, const cv::Mat &foreground, cv::Mat &output, int x, int y){
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

fn endsWithFace(std::string str) {
    int len = str.length();
    if(str.substr(len-6,6) == "表情"){
        return 1;
    }
    return 0;
}