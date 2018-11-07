//
// Created by xujijun on 12/31/17.
//


//以下内容感谢  《解析bmp图片及vc源码》   https://download.csdn.net/download/enlaihe/10638491
//42 4D 					     //位图文件的类型，在Windows中，此字段的值总为‘BM’  (1-2字节）
//B6 E5 00 00 				 //位图文件的大小，以字节为单位 （3-6字节，低位在前）
//00 00 						 //位图文件保留字，必须为0  (7-8字节）
//00 00 						 //位图文件保留字，必须为0  (9-10字节）
//76 00 00 00 				//位图数据的起始位置，以相对于位图 （11-14字节，低位在前）//
//28 00 00 00 				//本结构所占用字节数 （15-18字节）
//C0 01 00 00 				//位图的宽度，以像素为单位 （19-22字节）
//06 01 00 00 				//位图的高度，以像素为单位（23-26字节）
//01 00 						//目标设备的级别，必须为1  (27-28字节）
//04 00 						//每个像素所需的位数，必须是1（双色） （29-30）
//00 00 00 00					//位图压缩类型，必须是0（不压缩）  （31-34）
//40 E5 00 00 				//（35-38）位图的大小(其中包含了为了补齐行数是4的倍数而添加的空字节)，以字节为单位
//00 00 						//位图水平分辨率，像素数（ 39-40 ）
//00 00 00 00          （41-44）
//00 00 00 00          （45-48）
//00 00 00 00 00 00 00 00 00 00 00 00 80 00 00 80     （ 49 - 64  ）
//00 00 00 80 80 00 80 00 00 00 80 00 80 00 80 80
//00 00 80 80 80 00 C0 C0 C0 00 00 00 FF 00 00 FF
//00 00 00 FF FF 00 FF 00 00 00 FF 00 FF 00 FF FF
//00 00 FF FF FF 00 FF FF FF FF FF FF FF FF FF FF
//FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 

#include <cstdio>
#include <iostream>
#include <string>
#include <imageIOException.h>
#include "huffmanCompression.h"
#include "imageIOHelper.h"
using namespace std;


#include<time.h>

void encodeImage() {
    string imageFilename, encodedFilename;
    cout << "Please input image filename(BMP format):" << endl;
    cin >> imageFilename;
    if (imageFilename.length() < 5 || imageFilename.substr(imageFilename.length() - 4) != ".bmp") {
        throw ImageIOException("image format wrong!");
    }

    cout << "Please input encoded filename(with extension name \".hfmc\"):" << endl;
    cin >> encodedFilename;
    if (encodedFilename.length() < 6 || encodedFilename.substr(encodedFilename.length() - 5) != ".hfmc") {
        throw ImageIOException("encoded filename format wrong!");
    }

    Image* imagePtr = ImageIOHelper::readImage(imageFilename);

    cout << "Width = " << imagePtr -> getWidth() << endl;
    cout << "Height = " << imagePtr -> getHeight() << endl;
    cout << "Pure dataSize = " << imagePtr -> getDataSize() << " Bytes" << endl;
//    if( imagePtr -> getRawDataSize() == 0 )
//    	imagePtr -> setRawDataSize(   imagePtr -> getDataSize()   );  //  added  by  LK. 201811061542
    cout << "RawDataSize = " << imagePtr -> getRawDataSize() << " Bytes" << endl;

    HuffmanCompression encoder;
    unordered_map<unsigned char, uint32_t> weightMap;
    unsigned char *encodedDataPtr = nullptr;
    uint32_t encodedDataBitSize;
    encoder.getEncodedData(imagePtr -> getData(), imagePtr -> getDataSize(),
                           weightMap, encodedDataPtr, encodedDataBitSize);

    cout << "Encoded data bit size: " << encodedDataBitSize << endl;
    uint32_t encodedDataSize = encodedDataBitSize / 8 + (encodedDataBitSize % 8 == 0 ? 0 : 1);
    cout << "EncodedDataSize: " << encodedDataSize << " Bytes" << endl;

    printf("Compression ratio = %.2f%%\n",
           static_cast<double>(encodedDataSize) /
                   static_cast<double >(imagePtr -> getRawDataSize()) * 100);

    ImageIOHelper::writeEncodeFile(encodedFilename, imagePtr -> getImageInfo(), weightMap,
                                   encodedDataPtr, encodedDataSize, encodedDataBitSize);

    delete [] encodedDataPtr;
    delete imagePtr;
}


double get_utime()
{
    return (double)clock()/CLOCKS_PER_SEC;
}

void decodeImage() {

	double t0 = get_utime();


    string imageFilename, encodedFilename;




    cout << "Please input encoded filename(with extension name \".hfmc\"):" << endl;
    cin >> encodedFilename;
    if (encodedFilename.length() < 6 || encodedFilename.substr(encodedFilename.length() - 5) != ".hfmc") {
        throw ImageIOException("encoded filename format wrong!");
    }

    cout << "Please input image filename(BMP format):" << endl;
    cin >> imageFilename;
    if (imageFilename.length() < 5 || imageFilename.substr(imageFilename.length() - 4) != ".bmp") {
        throw ImageIOException("image format wrong!");
    }

    EncodeFile* encodeFilePtr = ImageIOHelper::readEncodeFile(encodedFilename);
    HuffmanCompression decoder;

    unsigned char *decodedDataPtr = nullptr;
    uint32_t decodedDataSize;

    decoder.getDecodedData(encodeFilePtr -> getData(),
                           encodeFilePtr -> getDataBitSize(),
                           encodeFilePtr -> getWeightMapArr(),
                           decodedDataPtr, decodedDataSize);

    cout << "Decoded data size: " << decodedDataSize << " Bytes" << endl;

    ImageIOHelper::writeImage(imageFilename,
                              encodeFilePtr -> getImageInfo(),
                              decodedDataPtr);

    delete [] decodedDataPtr;
    delete encodeFilePtr;

    double t1 = get_utime();
//	fprintf(msgfile, "Time = %1.3lf secs : %1.0lf KSymbols/sec\n", t1 - t0,
//		(tot_in) / (1024 * (t1 - t0)));
	printf(  "Time = %1.3lf secs   \n", t1 - t0
		     );

}

int main() {
#if 0
	string imageFilename = "D:\\projectsUbun_image_encoder_decoder\\HuffmanImageCompression\\img/rgb384x550.hfmc[otsu].bmp";
	imageFilename = "D:\\projectsUbun_image_encoder_decoder\\HuffmanImageCompression\\img/rgb384x550.hfmc.bmp.hfmc.bmp";
	imageFilename = "D:\\projectsUbun_image_encoder_decoder\\HuffmanImageCompression\\img/car001_384x550.bmp";

	Image* imagePtr = ImageIOHelper::readImage(imageFilename);
//return 0 ;
#endif

#if 1
	const  string mageFilename = "D:\\projectsUbun_image_encoder_decoder\\HuffmanImageCompression\\img/car001_384x550.hfmc";

	EncodeFile * pp =  ImageIOHelper::readEncodeFile(mageFilename);
//return 0 ;
#endif

    string command;
    const string EncodeCmd = "e";
    const string DecodeCmd = "d";
    cout << "Input your choice:(\"e\"for encode, \"d\"for decode): ";
    cin >> command;
    if (command == EncodeCmd) {
        encodeImage();
    } else if (command == DecodeCmd) {
        decodeImage();
    } else {
        cout << endl;
        cout << "Wrong Command!" << endl;
    }

    return 0;
}
