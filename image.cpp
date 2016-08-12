
#include <stdio.h>
#include <iostream>
#include <fstream>

unsigned char* readBMP(char* filename, std::ofstream* log)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    for(i = 0; i < size; i += 3)
    {
            unsigned char tmp = data[i];
            data[i] = data[i+2];
            data[i+2] = tmp;
    }

    return data;
}

int main(int argc, char *argv[])
{
    std::ofstream log;
    log.open ("log.txt");
    char imagePath[] = "Screenshot.bmp";
    unsigned char* pixels;
    
    pixels = readBMP(imagePath, &log);
    
    // pixels is a pointer that points to first element of the array. e.g Get 2nd element by either pixels[1] or *(pixels + 1), which retrieves the next address.
    log << "RED " <<(int)pixels[0] << "\n";
    log << "GREEN " <<(int)pixels[1] << "\n";
    log << "BLUE " <<(int)pixels[2] << "\n";
    log.close();
    
    delete pixels;
    
    return 0;
}