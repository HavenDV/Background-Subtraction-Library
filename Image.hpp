/****************************************************************************
*                                                                             
*   This program is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by     
*    the Free Software Foundation, either version 3 of the License, or        
*    (at your option) any later version.                                      
*                                                                             
*    This program is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of           
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            
*    GNU General Public License for more details.                             
*                                                                             
*    You should have received a copy of the GNU General Public License        
*    along with this program. If not, see <http://www.gnu.org/licenses/>.     
*                                                                             
******************************************************************************/

#ifndef _IMAGE_H_
#define _IMAGE_H_
#include <opencv2/core.hpp>

// --- Image Iterator ---------------------------------------------------------

template < typename T >
using ImageIterator = cv::MatIterator_< T >;

// --- Constants --------------------------------------------------------------

const unsigned char NUM_CHANNELS = 3;

// --- Pixel Types ------------------------------------------------------------

typedef cv::Vec3b   RgbPixel;
typedef cv::Vec3f   RgbPixelFloat;

// --- Image Types ------------------------------------------------------------

typedef cv::Mat                     BaseImage;
typedef cv::Mat_< cv::Vec3b >       RgbImage;
typedef cv::Mat_< unsigned char >   BwImage;
typedef cv::Mat_< cv::Vec3f >       RgbImageFloat;
typedef cv::Mat_< float >           BwImageFloat;

// --- Image Functions --------------------------------------------------------

void    DensityFilter( const BwImage & image, BwImage & filtered, int minDensity, unsigned char fgValue );

#endif

/*
template <class T>
class ImageIterator
{
public:
ImageIterator(IplImage* image, int x=0, int y=0, int dx= 0, int dy=0) :
i(x), j(y), i0(0)
{
data = reinterpret_cast<T*>(image->imageData);
step = image->widthStep / sizeof(T);

nl= image->height;
if ((y+dy)>0 && (y+dy) < nl)
nl= y+dy;

if (y<0)
j=0;

data += step*j;

nc = image->width;
if ((x+dx) > 0 && (x+dx) < nc)
nc = x+dx;

nc *= image->nChannels;
if (x>0)
i0 = x*image->nChannels;
i = i0;

nch = image->nChannels;
}

*/
/* has next ? */
//bool operator!() const { return j < nl; }

/* next pixel
ImageIterator& operator++()
{
i++;
if (i >= nc)
{
i=i0;
j++;
data += step;
}
return *this;
}

ImageIterator& operator+=(int s)
{
i+=s;
if (i >= nc)
{
i=i0;
j++;
data += step;
}
return *this;
}
*/
/* pixel access
T& operator*() { return data[i]; }

const T operator*() const { return data[i]; }

const T neighbor(int dx, int dy) const
{
return *(data+dy*step+i+dx);
}

T* operator&() const { return data+i; }
*/
/* current pixel coordinates
int column() const { return i/nch; }
int line() const { return j; }

private:
int i, i0,j;
T* data;
int step;
int nl, nc;
int nch;
};
*/