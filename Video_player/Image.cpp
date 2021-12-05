#include "Image.h"

//{"Links":[[{"AimVideoFrameNumber":1134,"AimVideoName":"E:/AIFilmOne","BottomRightPointX":240,
//"BottomRightPointY":204,"HyperLinkName":"aaa","PrimaryVideoFrameNumber":1,"TopLeftPointX":116,"TopLeftPointY":138}

// Constructor and Desctructors
MyImage::MyImage()
{
    Data = NULL;
    Width = -1;
    Height = -1;
    flag = 0;
    ImagePath[0] = 0;
}

MyImage::~MyImage()
{
    if ( Data )
        delete Data;
}


// Copy constructor
MyImage::MyImage( MyImage *otherImage)
{
    Height = otherImage->Height;
    Width  = otherImage->Width;
    Data   = new char[Width*Height*3];
    flag = 0;
    strcpy(otherImage->ImagePath, ImagePath );

    for ( int i=0; i<(Height*Width*3); i++ )
    {
        Data[i]	= otherImage->Data[i];
    }
}



// = operator overload
MyImage & MyImage::operator= (const MyImage &otherImage)
{
    Height = otherImage.Height;
    Width  = otherImage.Width;
    Data   = new char[Width*Height*3];
    flag = 0;
    strcpy( (char *)otherImage.ImagePath, ImagePath );

    for ( int i=0; i<(Height*Width*3); i++ )
    {
        Data[i]	= otherImage.Data[i];
    }

    return *this;

}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

    // Verify ImagePath
    if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
    {
        fprintf(stderr, "Image or Image properties not defined");
        fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
        return false;
    }

    // Create a valid output file pointer
    FILE *IN_FILE;
    IN_FILE = fopen(ImagePath, "rb");
    if ( IN_FILE == NULL )
    {
        fprintf(stderr, "Error Opening File for Reading");
        return false;
    }

    // Create and populate RGB buffers
    int i;
    char *Rbuf = new char[Height*Width];
    char *Gbuf = new char[Height*Width];
    char *Bbuf = new char[Height*Width];

    for (i = 0; i < Width*Height; i ++)
    {
        Rbuf[i] = fgetc(IN_FILE);
    }
    for (i = 0; i < Width*Height; i ++)
    {
        Gbuf[i] = fgetc(IN_FILE);
    }
    for (i = 0; i < Width*Height; i ++)
    {
        Bbuf[i] = fgetc(IN_FILE);
    }

    // Allocate Data structure and copy
    Data = new char[Width*Height*3];
    for (i = 0; i < Height*Width; i++)
    {
        Data[3*i]	= Rbuf[i];
        Data[3*i+1]	= Gbuf[i];
        Data[3*i+2]	= Bbuf[i];
    }

    // Clean up and return
    delete[] Rbuf;
    delete[] Gbuf;
    delete[] Bbuf;
    fclose(IN_FILE);

    return true;

}



// MyImage functions defined here
bool MyImage::WriteImage()
{
    // Verify ImagePath
    // Verify ImagePath
    if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
    {
        fprintf(stderr, "Image or Image properties not defined");
        return false;
    }

    // Create a valid output file pointer
    FILE *OUT_FILE;
    OUT_FILE = fopen(ImagePath, "wb");
    if ( OUT_FILE == NULL )
    {
        fprintf(stderr, "Error Opening File for Writing");
        return false;
    }

    // Create and populate RGB buffers
    int i;
    char *Rbuf = new char[Height*Width];
    char *Gbuf = new char[Height*Width];
    char *Bbuf = new char[Height*Width];

    for (i = 0; i < Height*Width; i++)
    {
        Bbuf[i] = Data[3*i];
        Gbuf[i] = Data[3*i+1];
        Rbuf[i] = Data[3*i+2];
    }


    // Write data to file
    for (i = 0; i < Width*Height; i ++)
    {
        fputc(Rbuf[i], OUT_FILE);
    }
    for (i = 0; i < Width*Height; i ++)
    {
        fputc(Gbuf[i], OUT_FILE);
    }
    for (i = 0; i < Width*Height; i ++)
    {
        fputc(Bbuf[i], OUT_FILE);
    }

    // Clean up and return
    delete[] Rbuf;
    delete[] Gbuf;
    delete[] Bbuf;
    fclose(OUT_FILE);

    return true;

}
