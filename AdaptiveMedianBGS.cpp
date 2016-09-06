/****************************************************************************
*
* AdaptiveMedianBGS.cpp
*
* Purpose: Implementation of the simple adaptive median background 
*		  		 subtraction algorithm described in:
*	  			 "Segmentation and tracking of piglets in images"
* 						by McFarlane and Schofield
*
* Author: Donovan Parks, September 2007
*
******************************************************************************/

#include "AdaptiveMedianBGS.hpp"

using namespace Algorithms::BackgroundSubtraction;

void    AdaptiveMedianBGS::Initalize(const BgsParams& params)
{
	m_params = static_cast< const AdaptiveMedianParams & >( params );
    //m_median.create( m_params.Width(), m_params.Height() );
    //m_median = BACKGROUND;
}

void    AdaptiveMedianBGS::getBackgroundImage( cv::OutputArray backgroundImage ) const
{
    backgroundImage.assign( m_median );
}

void    AdaptiveMedianBGS::InitModel(const RgbImage& data)
{
	// initialize the background model
    m_median = data.clone();
}

void AdaptiveMedianBGS::Update(int frame_num, const RgbImage& data,  const BwImage& update_mask)
{
    auto check1 = ( frame_num % m_params.SamplingRate() ) == 1;
    auto check2 = frame_num < m_params.LearningFrames();
    if( !check1 && !check2 )
    {
        return;
    }

	// update background model
    data.forEach( [ & ]( auto && color, auto && position ) {
        auto r = position[ 0 ];
        auto c = position[ 1 ];

        // perform conditional updating only if we are passed the learning phase
        auto check1 = update_mask( r, c ) == BACKGROUND;
        auto check2 = frame_num < m_params.LearningFrames();
        if( !check1 && !check2 )
        {
            return;
        }
        
        for( int ch = 0; ch < NUM_CHANNELS; ++ch )
        {
            if( data( r, c )[ ch ] > m_median( r, c )[ ch ] )
            {
                m_median( r, c )[ ch ]++;
            }
            else if( data( r, c )[ ch ] < m_median( r, c )[ ch ] )
            {
                m_median( r, c )[ ch ]--;
            }
         }
    } );
}

void    AdaptiveMedianBGS::SubtractPixel( int r, int c, const RgbPixel & pixel, 
										  unsigned char & low_threshold, 
                                          unsigned char&  high_threshold )
{
	// perform background subtraction
	low_threshold = FOREGROUND;
    high_threshold = FOREGROUND;
	
	int diffR = cv::abs( pixel[ 0 ] - m_median( r,c )[ 0 ] );
	int diffG = cv::abs( pixel[ 1 ] - m_median( r,c )[ 1 ] );
	int diffB = cv::abs( pixel[ 2 ] - m_median( r,c )[ 2 ] );
	
    auto low = m_params.LowThreshold();
	if( diffR <= low && diffG <= low &&  diffB <= low )
	{
		low_threshold = BACKGROUND;
	}

    auto high = m_params.HighThreshold();
	if( diffR <= high && diffG <= high &&  diffB <= high )
	{
		high_threshold = BACKGROUND;
	}
}

///////////////////////////////////////////////////////////////////////////////
//Input:
//  data - a pointer to the image data
//Output:
//  output - a pointer to the data of a gray value image
//					(the memory should already be reserved) 
//					values: 255-foreground, 0-background
///////////////////////////////////////////////////////////////////////////////
void AdaptiveMedianBGS::Subtract(int frame_num, const RgbImage& data, 
																	BwImage& low_threshold_mask, BwImage& high_threshold_mask)
{
    //ADD CHECK FOR SOME SIZE AND SOME TYPE
    //ADD CHECK FOR NON EMPTY

    // update each pixel of the image
    data.forEach( [ & ]( auto && color, auto && position ) {
        auto r = position[ 0 ];
        auto c = position[ 1 ];

        unsigned char low_threshold, high_threshold;
        SubtractPixel( r, c, color, low_threshold, high_threshold );

        // setup silhouette mask
        low_threshold_mask( r, c ) = low_threshold;
        high_threshold_mask( r, c ) = high_threshold;
    } );
}

