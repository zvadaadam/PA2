//
//  main.cpp
//  RotatePicture
//
//  Created by Adam Zvada on 12.03.17.
//  Copyright © 2017 Adam Zvada. All rights reserved.
//

#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <memory>
#include <set>
#include <queue>


using namespace std;

#endif /* __PROGTEST__ */

#define ENDIAN_LITTLE 0x4949
#define ENDIAN_BIG 0x4d4d

#define MATRIX_SIZE 64
#define MATRIX_EDGE 8

#define BSWAP16(n) ((n) << 8 | ((n) >> 8 & 0x00FF))


uint16_t changeEndian(uint16_t n) {
    return  ((n) << 8 | ((n) >> 8 & 0x00FF));
}


struct HeaderFile {
    uint16_t endian;
    uint16_t widthImage;
    uint16_t heightImage;
    
    uint16_t formatPixel;
    uint16_t numChannels;
    uint16_t bitsPerChannel;
    
    bool readHeader(ifstream & inputFile);
    
    bool isLittleEndian();
    bool isBigEndian();

    void printHeader();
private:
    bool setChannelAndBits(uint8_t channelAndBits);
    bool setChannelAndBits(uint16_t channelAndBits);
    bool parseBitsPerChannel();
};

bool HeaderFile::isLittleEndian() {
    if (ENDIAN_LITTLE == endian) {
        return true;
    }
    return false;
}

bool HeaderFile::isBigEndian() {
    if (ENDIAN_BIG == endian) {
        return true;
    }
    return false;
}

bool HeaderFile::parseBitsPerChannel() {
    
    switch (bitsPerChannel) {
        case 0:
            bitsPerChannel = 1;
            break;
        case 2:
            bitsPerChannel = 4;
            break;
        case 3:
            bitsPerChannel = 8;
            break;
        case 4:
            bitsPerChannel = 16;
            break;
        default:
            return false;
    }
    
    return true;
}

bool HeaderFile::setChannelAndBits(uint16_t channelAndBits) {
    
    if ((channelAndBits >> 8) != 0) {
        //cout << "Header, error zero padding." << endl;
        return false;
    }
    
    this->bitsPerChannel = (channelAndBits & 0x00F0) >> 4;
    this->numChannels = (channelAndBits & 0x000F);
    
    if (numChannels < 1 || numChannels > 15) return false;
    
    if(!parseBitsPerChannel()) return false;
    
    return true;
}


bool HeaderFile::readHeader(ifstream & inputFile) {
    
    if (!inputFile.read((char*)&endian, sizeof(uint16_t))) return false;
    
    if (!inputFile.read((char*)&widthImage, sizeof(uint16_t))) return false;
    
    if (!inputFile.read((char*)&heightImage, sizeof(uint16_t))) return false;
    
    if (!inputFile.read((char*)&formatPixel, sizeof(uint16_t))) return false;
    
    if(endian == ENDIAN_BIG) {
        widthImage = changeEndian(widthImage);
        heightImage = changeEndian(heightImage);
        formatPixel = changeEndian(formatPixel);
    }
    
    if (heightImage <= 0 || widthImage <= 0) return false;
    
    setChannelAndBits(formatPixel);
    
    
    return true;
}

void HeaderFile::printHeader() {
    cout << "Lil. Endian: " << isLittleEndian() << endl;
    cout << "Width: " << widthImage << endl;
    cout << "Height: " << heightImage << endl;
    cout << "BitsPerChannel: "<< bitsPerChannel << endl;
    cout << "NumChannels: " << numChannels << endl;
}
//--------------------------------------------------------------

//--------------------------------------------------------------

struct Channel {
    int bitSize;
    uint16_t bitVal;
    
    Channel();
    
    void setBitSize(int bitSize);
};

void Channel::setBitSize(int bitSize) {
    this->bitSize = bitSize;
}

Channel::Channel() {
    this->bitVal = 0;
    this->bitSize = 0;
}

//--------------------------------------------------------------

//--------------------------------------------------------------


struct Pixel {
    int numChannels;
    Channel * channels;
    
    void setAndInitChannel(int numChannel);
};

void Pixel::setAndInitChannel(int numChannel) {
    this->numChannels = numChannel;
    Channel * channels = new Channel[numChannels];
    this->channels = channels;
}


//--------------------------------------------------------------

//--------------------------------------------------------------


class Image {
public:
    HeaderFile * header;
    Pixel ** pixelMatrix; //decoded pixel matrix
    uint16_t actualWidth;
    uint16_t actualHeight;
    
    uint16_t byteOrder;
    
    Image(ifstream * inputfile, uint16_t byteOrder);
    
    bool parseHeader();
    bool decodeImg();
    ofstream & encodeImg(ofstream & outputFile);
    
    bool rotateMatrix(int angle);
    
    void printMatrixPixel() const;
private:
    ifstream * inputFile;
    
    bool setPixelMatrixForChannel(int indexChannel, int indexMatrixHeight, int indexMatrixWidth);
    
    bool writeToFileFromPixelMatrix(ofstream & outputFile, int indexChannel, int indexMatrixHeight, int indexMatrixWidth);
    bool writeHeaderToFile(ofstream & outputFile);
    
    bool rotateMatrixClockwise();
    bool rotateMatrixAntiClockwise();
    bool rotateMatrix180();
    
    Pixel ** createNewEmptyMatrix(uint16_t height, uint16_t width);
    bool initMatrixPixel();
    
    bool superCoolRead(int i, int j, int k);
    bool superCoolWrite(ofstream & outputFile, int i, int j, int k, unsigned int byteOrder);
    
    void swap(uint16_t & a, uint16_t & b);
    
};

Image::Image(ifstream * inputFile, uint16_t byteOrder) {
    HeaderFile * header = new HeaderFile();
    this->header = header;
    this->inputFile = inputFile;
    this->byteOrder = byteOrder;
}

bool Image::parseHeader() {
    if(!header->readHeader(*inputFile)) return false;
    
    if (!(header->endian == ENDIAN_LITTLE || header->endian == ENDIAN_BIG)) return false;
    
    //Set sizes to mupltiple of size of MATRIX_EDGE
    if (header->widthImage % MATRIX_EDGE != 0) this->actualWidth = header->widthImage + (MATRIX_EDGE - (header->widthImage % MATRIX_EDGE));
    else this->actualWidth = header->widthImage;
    
    if (header->heightImage % MATRIX_EDGE != 0) this->actualHeight = header->heightImage + (MATRIX_EDGE - (header->heightImage % MATRIX_EDGE));
    else this->actualHeight = header->heightImage;
    
    
    return true;
}


bool Image::decodeImg() {
    
    int numMatrixHeight = actualHeight / MATRIX_EDGE;
    int numMatrixWidth = actualWidth / MATRIX_EDGE;
    
    
    initMatrixPixel();
    
    //TODO, Set to pos to start of input file
    
    
    //All 8x8 MATRIX of image
    for (int i = 0; i < numMatrixHeight; i++) {
        
        for (int j = 0; j < numMatrixWidth; j++) {
            
            //All channels
            for (int k = 0; k < header->numChannels; k++) {
                
                setPixelMatrixForChannel(k, i, j);
            }
        }
    }
    
    return true;
}

ofstream & Image::encodeImg(ofstream & outputFile) {
    
    int numMatrixHeight = actualHeight / MATRIX_EDGE;
    int numMatrixWidth = actualWidth / MATRIX_EDGE;
    
    //TODO, Set to pos to start of input file
    
    writeHeaderToFile(outputFile);
    
    for (int i = 0; i < numMatrixHeight; i++) {
        
        for (int j = 0; j < numMatrixWidth; j++) {
            
            for (int k = 0; k < header->numChannels; k++) {
                
                writeToFileFromPixelMatrix(outputFile, k, i, j);
                
            }
        }
    }
    
    return outputFile;
}

bool Image::rotateMatrix(int angle) {
    
    int angleBase = angle % 360;
    if (angleBase < 0) {
        angleBase = 360 + angleBase;
    }
    
    switch (angleBase) {
        case 0:
            break;
        case 90:
            rotateMatrixClockwise();
            break;
        case 180:
            rotateMatrix180();
            break;
        case 270:
            rotateMatrixAntiClockwise();
            break;
        default:
            return false;
    }
    
    
    return true;
}

bool Image::rotateMatrixClockwise() {
    
    Pixel ** matrixRotated = createNewEmptyMatrix(actualWidth, actualHeight);
    
    
    for(int i = 0; i < header->heightImage; i++) {
        for(int j = 0; j < header->widthImage; j++) {
            matrixRotated[j][header->heightImage-1-i] = pixelMatrix[i][j];
        }
    }
    
    //LEAK, Delete prev pixelMatrix
    this->pixelMatrix = matrixRotated;
    
    swap(actualHeight, actualWidth);
    swap(header->heightImage, header->widthImage);
    
    return true;
}

bool Image::rotateMatrixAntiClockwise() {
    
    Pixel ** matrixRotated = createNewEmptyMatrix(actualWidth, actualHeight);
    
    for(int i = 0; i < header->heightImage; i++) {
        for(int j = 0; j < header->widthImage; j++) {
            matrixRotated[header->widthImage-1-j][i] = pixelMatrix[i][j];
        }
    }

    //LEAK, Delete prev pixelMatrix
    this->pixelMatrix = matrixRotated;
    
    swap(actualHeight, actualWidth);
    swap(header->heightImage, header->widthImage);

    
    return true;
}

bool Image::rotateMatrix180() {
    
    Pixel ** matrixRotated = createNewEmptyMatrix(actualHeight, actualWidth);
    
    for(int i = 0; i < header->heightImage; i++) {
        for(int j = 0; j < header->widthImage; j++) {
            matrixRotated[header->heightImage-1-i][header->widthImage-1-j] = pixelMatrix[i][j];
        }
    }
    
    //LEAK, Delete prev pixelMatrix
    this->pixelMatrix = matrixRotated;
    
    return true;
}

Pixel ** Image::createNewEmptyMatrix(uint16_t height, uint16_t width) {
    
    Pixel ** matrix = new Pixel*[height];
    for (int i = 0; i < height; i++) {
        matrix[i] = new Pixel[width];
    }
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            matrix[i][j].setAndInitChannel(header->numChannels);
        }
    }
    

    return matrix;
}

bool Image::initMatrixPixel() {
    
    pixelMatrix = new Pixel*[actualHeight];
    for (int i = 0; i < actualHeight; i++) {
        pixelMatrix[i] = new Pixel[actualWidth];
    }
    
    for (int i = 0; i < actualHeight; i++) {
        for (int j = 0; j < actualWidth; j++) {
            pixelMatrix[i][j].setAndInitChannel(header->numChannels);
        }
    }
    
    return true;
}


bool Image::writeToFileFromPixelMatrix(ofstream & outputFile, int indexChannel, int indexMatrixHeight, int indexMatrixWidth) {
    
    int matrixOffsetHeight = MATRIX_EDGE*indexMatrixHeight;
    int matrixOffsetWidth = MATRIX_EDGE*indexMatrixWidth;
    
    for (int i = 0; i < 2 * MATRIX_EDGE - 1; ++i) {
        int z = i < MATRIX_EDGE ? 0 : i - MATRIX_EDGE + 1;
        for (int j = z; j <= i - z; ++j) {
            
            //cout << j+matrixOffsetHeight << "," << (i-j)+matrixOffsetWidth << " ";
            
            //outputFile.write((char*)&pixelMatrix[j+matrixOffsetHeight][(i-j)+matrixOffsetWidth].channels[indexChannel].bitVal, sizeof(uint8_t));
            
            superCoolWrite(outputFile, j+matrixOffsetHeight, (i-j)+matrixOffsetWidth, indexChannel, byteOrder);
            
        }
        //cout << endl;
    }
    
    return true;

    
    return true;
}

bool Image::writeHeaderToFile(ofstream & outputFile) {
    
    if(byteOrder == ENDIAN_BIG) {
        outputFile.seekp(0, ios::beg);
        
        uint16_t endian = ENDIAN_BIG;
        uint16_t width = changeEndian(header->widthImage);
        uint16_t height = changeEndian(header->heightImage);
        uint16_t format = changeEndian(header->formatPixel );
        
        outputFile.write((char*)&endian, sizeof(uint16_t));
        
        outputFile.write((char*)&width, sizeof(uint16_t));
        
        outputFile.write((char*)&height, sizeof(uint16_t));
        
        outputFile.write((char*)&format, sizeof(uint16_t));
    } else {
        outputFile.seekp(0, ios::beg);
        
        uint16_t endian = ENDIAN_LITTLE;
        outputFile.write((char*)&endian, sizeof(uint16_t));
        outputFile.write((char*)&header->widthImage, sizeof(uint16_t));
        outputFile.write((char*)&header->heightImage, sizeof(uint16_t));
        outputFile.write((char*)&header->formatPixel, sizeof(uint16_t));
    }
    
    
    return true;
}

bool Image::superCoolRead(int i, int j, int k) {
    if (header->bitsPerChannel == 8) {
        inputFile->read((char*)&pixelMatrix[i][j].channels[k].bitVal, sizeof(uint8_t));
    } else if (header->bitsPerChannel == 16) {
        inputFile->read((char*)&pixelMatrix[i][j].channels[k].bitVal, sizeof(uint16_t));
    }
    
    if(header->isBigEndian()) {
        pixelMatrix[i][j].channels[k].bitVal = changeEndian(pixelMatrix[i][j].channels[k].bitVal);
    }
    
    return true;
}

bool Image::superCoolWrite(ofstream & outputFile, int i, int j, int k, unsigned int byteOrder) {

    if (byteOrder == ENDIAN_BIG) {
        pixelMatrix[i][j].channels[k].bitVal = changeEndian(pixelMatrix[i][j].channels[k].bitVal);
    }
    
    if (header->bitsPerChannel == 8) {
        outputFile.write((char*)&pixelMatrix[i][j].channels[k].bitVal, sizeof(uint8_t));
    } else if (header->bitsPerChannel == 16) {
        outputFile.write((char*)&pixelMatrix[i][j].channels[k].bitVal, sizeof(uint16_t));
    }

    return true;
}



bool Image::setPixelMatrixForChannel(int indexChannel, int indexMatrixHeight, int indexMatrixWidth) {
    
    int matrixOffsetHeight = MATRIX_EDGE*indexMatrixHeight;
    int matrixOffsetWidth = MATRIX_EDGE*indexMatrixWidth;
    
    for (int i = 0; i < 2 * MATRIX_EDGE - 1; ++i) {
        int z = i < MATRIX_EDGE ? 0 : i - MATRIX_EDGE + 1;
        for (int j = z; j <= i - z; ++j) {

            //cout << j+matrixOffsetHeight << "," << (i-j)+matrixOffsetWidth << " ";
            
            //inputFile->read((char*)&pixelMatrix[j+matrixOffsetHeight][(i-j)+matrixOffsetWidth].channels[indexChannel].bitVal, sizeof(uint8_t));
            
            superCoolRead(j+matrixOffsetHeight, (i-j)+matrixOffsetWidth, indexChannel);
            
        }
        //cout << endl;
    }
    
    return true;
}

void Image::swap(uint16_t & a, uint16_t & b) {
    uint16_t tmp = a;
    a = b;
    b = tmp;
}

void Image::printMatrixPixel() const {
    
        
    for (int i = 0; i < actualHeight;i++) {
        for (int j = 0; j < actualWidth; j++) {
            for (int k = 0; k < header->numChannels; k++) {
                cout << (int)pixelMatrix[i][j].channels[k].bitVal << " ";
            }
        }
        cout << endl;
    }

}

//--------------------------------------------------------------

//--------------------------------------------------------------


bool rotateImage(const char * srcFileName, const char * dstFileName, int angle, unsigned int byteOrder) {
    
    ifstream inputFile;
    inputFile.open(srcFileName, ios::binary);
    if(inputFile.fail()) {
        return false;
    }
    
    ofstream outputFile;
    outputFile.open(dstFileName, ios::binary);
    if(outputFile.fail()) {
        inputFile.close();
        return false;
    }
    
    Image * img = new Image(&inputFile, byteOrder);
    if(!img->parseHeader()) return false;
    
    //img->header->printHeader();
    
    if (!img->decodeImg()) return false;

    //img->printMatrixPixel();
    
    if (!img->rotateMatrix(angle)) return false;
    
    //img->printMatrixPixel();
    
    img->encodeImg(outputFile);
    
    inputFile.seekg(0, ios::end);
    //cout << "SIZE: "<< inputFile.tellg() << "  VS  " << (img->actualHeight * img->actualWidth * img->header->numChannels + 8) << endl;
    //does not compute with byte size
    if (inputFile.tellg() != (img->actualHeight * img->actualWidth * img->header->numChannels * (img->header->bitsPerChannel/8) + 8)) {
        return false;
    }
    
    if (!inputFile.good() || !outputFile.good() || inputFile.get() != EOF) {
        outputFile.close();
        inputFile.close();
        return false;
    }
    
    outputFile.close();
    inputFile.close();
    
    return true;
}


#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1, const char * fileName2 ) {
    
    string str = "diff ";
    str += fileName1;
    str += " ";
    str += fileName2;
    
    if (system(str.c_str()) == 0) return true;
    
    return false;
}

int main ( void ) {
    
    assert (  rotateImage ( "input_00.img", "output_00.img", 0, ENDIAN_LITTLE )
            && identicalFiles ( "output_00.img", "ref_00.img" ) );
    
    assert (  rotateImage ( "input_01.img", "output_01.img", 90, ENDIAN_LITTLE )
            && identicalFiles ( "output_01.img", "ref_01.img" ) );
    
    assert (  rotateImage ( "input_02.img", "output_02.img", 540, ENDIAN_LITTLE )
            && identicalFiles ( "output_02.img", "ref_02.img" ) );
    
    assert (  rotateImage ( "input_03.img", "output_03.img", -900450, ENDIAN_LITTLE )
            && identicalFiles ( "output_03.img", "ref_03.img" ) );
    
    assert (  rotateImage ( "input_04.img", "output_04.img", 90, ENDIAN_LITTLE )
            && identicalFiles ( "output_04.img", "ref_04.img" ) );
    
    assert (  rotateImage ( "input_05.img", "output_05.img", 270, ENDIAN_LITTLE )
            && identicalFiles ( "output_05.img", "ref_05.img" ) );

    assert (  rotateImage ( "input_06.img", "output_06.img", 90, ENDIAN_LITTLE )
            && identicalFiles ( "output_06.img", "ref_06.img" ) );
    
    assert (  rotateImage ( "input_07.img", "output_07.img", 270, ENDIAN_LITTLE )
            && identicalFiles ( "output_07.img", "ref_07.img" ) );
    
    assert ( ! rotateImage ( "input_08.img", "output_08.img", 270, ENDIAN_LITTLE ) );
    
    assert ( ! rotateImage ( "input_09.img", "output_09.img", -275, ENDIAN_LITTLE ) );
    
    // extra inputs (optional & bonus tests)
    assert ( rotateImage ( "extra_input_00.img", "extra_out_00.img", 90, ENDIAN_LITTLE )
            && identicalFiles ( "extra_out_00.img", "extra_ref_00.img" ) );
    assert ( rotateImage ( "extra_input_01.img", "extra_out_01.img", 90, ENDIAN_BIG )
            && identicalFiles ( "extra_out_01.img", "extra_ref_01.img" ) );
    assert ( rotateImage ( "extra_input_02.img", "extra_out_02.img", 180, ENDIAN_BIG )
            && identicalFiles ( "extra_out_02.img", "extra_ref_02.img" ) );
    assert ( rotateImage ( "extra_input_03.img", "extra_out_03.img", 270, ENDIAN_LITTLE )
            && identicalFiles ( "extra_out_03.img", "extra_ref_03.img" ) );
    assert ( rotateImage ( "extra_input_04.img", "extra_out_04.img", 90, ENDIAN_LITTLE )
            && identicalFiles ( "extra_out_04.img", "extra_ref_04.img" ) );
    assert ( rotateImage ( "extra_input_05.img", "extra_out_05.img", 270, ENDIAN_LITTLE )
            && identicalFiles ( "extra_out_05.img", "extra_ref_05.img" ) );
    assert ( rotateImage ( "extra_input_06.img", "extra_out_06.img", 180, ENDIAN_BIG )
            && identicalFiles ( "extra_out_06.img", "extra_ref_06.img" ) );
    assert ( rotateImage ( "extra_input_07.img", "extra_out_07.img", 0, ENDIAN_BIG )
            && identicalFiles ( "extra_out_07.img", "extra_ref_07.img" ) );
    assert ( rotateImage ( "extra_input_08.img", "extra_out_08.img", 90, ENDIAN_LITTLE )
            && identicalFiles ( "extra_out_08.img", "extra_ref_08.img" ) );
    assert ( rotateImage ( "extra_input_09.img", "extra_out_09.img", 270, ENDIAN_LITTLE )
            && identicalFiles ( "extra_out_09.img", "extra_ref_09.img" ) );
    assert ( rotateImage ( "extra_input_10.img", "extra_out_10.img", 180, ENDIAN_BIG )
            && identicalFiles ( "extra_out_10.img", "extra_ref_10.img" ) );
    assert ( rotateImage ( "extra_input_11.img", "extra_out_11.img", 0, ENDIAN_BIG )
            && identicalFiles ( "extra_out_11.img", "extra_ref_11.img" ) );
    return 0;
}
#endif /* __PROGTEST__ */



