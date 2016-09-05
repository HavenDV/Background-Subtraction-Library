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

#include "Image.hpp"

void    DensityFilter( const BwImage & image, BwImage & filtered, int minDensity, unsigned char fgValue )
{
    for( int r = 1; r < image.rows - 1; ++r )
    {
        for( int c = 1; c < image.cols - 1; ++c )
        {
            int count = 0;
            if( image.at< uchar >( r, c ) == fgValue )
            {
                if( image.at< uchar >( r - 1, c - 1 ) == fgValue )
                    count++;
                if( image.at< uchar >( r - 1, c ) == fgValue )
                    count++;
                if( image.at< uchar >( r - 1, c + 1 ) == fgValue )
                    count++;
                if( image.at< uchar >( r, c - 1 ) == fgValue )
                    count++;
                if( image.at< uchar >( r, c + 1 ) == fgValue )
                    count++;
                if( image.at< uchar >( r + 1, c - 1 ) == fgValue )
                    count++;
                if( image.at< uchar >( r + 1, c ) == fgValue )
                    count++;
                if( image.at< uchar >( r + 1, c + 1 ) == fgValue )
                    count++;

                if( count < minDensity )
                    filtered.at< uchar >( r, c ) = 0;
                else
                    filtered.at< uchar >( r, c ) = fgValue;
            }
            else
            {
                filtered.at< uchar >( r, c ) = 0;
            }
        }
    }
}