#ifndef IMAGE_H
#define IMAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cstring>
#include <QDebug>


// Class structure of Image
// Use to encapsulate an RGB image
class MyImage
{

private:
    int		Width;					// Width of Image
    int		Height;					// Height of Image
    char	ImagePath[_MAX_PATH];	// Image location
    char*	Data;					// RGB data of the image
    bool    flag;			    // already get data or not

public:
    // Constructor
    MyImage();
    // Copy Constructor
    MyImage( MyImage *otherImage);
    // Destructor
    ~MyImage();

    // operator overload
    MyImage & operator= (const MyImage & otherImage);

    // Reader & Writer functions
    void	setWidth( const int w)  { Width = w; };
    void	setHeight(const int h) { Height = h; };
    void	setImageData( const char *img ) { Data = (char *)img; };
    void	setImagePath( const char *path) { strcpy_s(ImagePath, path); }
    int		getWidth() { return Width; };
    int		getHeight() { return Height; };
    char*	getImageData() { return Data; };
    char*	getImagePath() { return ImagePath; }
    void	setFlag() { flag = 1; }
    bool	isInit() { return flag; }

    // Input Output operations
    bool	ReadImage();
    bool	WriteImage();

    // Modifications
    bool	Modify();

};

#endif // IMAGE_H
