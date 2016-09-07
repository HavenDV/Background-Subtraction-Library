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

AdaptiveMedianBGS::AdaptiveMedianBGS() :
    m_i( 0 ),
    m_low_threshold( 40U ),
    m_high_threshold( 80U ),
    m_samplingRate( 7 ),
    m_learning_frames( 30 )
{}

AdaptiveMedianBGS::~AdaptiveMedianBGS()
{}

void    AdaptiveMedianBGS::getBackgroundImage( cv::OutputArray backgroundImage ) const
{
    m_median.copyTo( backgroundImage );
}

void    AdaptiveMedianBGS::InitModel( const RgbImage& data )
{
    // initialize the background model
    m_median = data.clone();
}

void AdaptiveMedianBGS::Update( int frame_num, const RgbImage& data, const BwImage& update_mask )
{
    auto check1 = ( frame_num % m_samplingRate ) == 1;
    auto check2 = frame_num < m_learning_frames;
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
        auto check2 = frame_num < m_learning_frames;
        if( !check1 && !check2 )
        {
            return;
        }

        for( int ch = 0; ch < m_median.channels(); ++ch )
        {
            if( data( r, c )[ ch ] > m_median( r, c )[ ch ] )
            {
                ++m_median( r, c )[ ch ];
            }
            else if( data( r, c )[ ch ] < m_median( r, c )[ ch ] )
            {
                --m_median( r, c )[ ch ];
            }
        }
    } );
}

void    AdaptiveMedianBGS::apply( cv::InputArray image, cv::OutputArray fgmask, double learningRate )
{
    cv::Mat frame_data = image.getMat();
    if( m_i == 0 )
    {
        // initialize background model to first frame of video stream
        InitModel( frame_data );
    }

    BwImage low_threshold_mask;
    BwImage high_threshold_mask;

    Subtract( m_i, frame_data, low_threshold_mask, high_threshold_mask );

    // update background subtraction
    Update( m_i, frame_data, low_threshold_mask );

    low_threshold_mask.copyTo( fgmask );

    ++m_i;
}

void    AdaptiveMedianBGS::SubtractPixel( int r, int c, const RgbPixel & pixel,
                                          unsigned char & low_threshold,
                                          unsigned char &  high_threshold )
{
    // perform background subtraction
    low_threshold = FOREGROUND;
    high_threshold = FOREGROUND;

    int diffR = cv::abs( pixel[ 0 ] - m_median( r, c )[ 0 ] );
    int diffG = cv::abs( pixel[ 1 ] - m_median( r, c )[ 1 ] );
    int diffB = cv::abs( pixel[ 2 ] - m_median( r, c )[ 2 ] );

    auto low = m_low_threshold;
    if( diffR <= low && diffG <= low &&  diffB <= low )
    {
        low_threshold = BACKGROUND;
    }

    auto high = m_high_threshold;
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
void AdaptiveMedianBGS::Subtract( int frame_num, const RgbImage& data,
                                  BwImage& low_threshold_mask, BwImage& high_threshold_mask )
{
    //ADD CHECK FOR SOME SIZE AND SOME TYPE
    //ADD CHECK FOR NON EMPTY
    low_threshold_mask.create( data.size() );
    high_threshold_mask.create( data.size() );

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

cv::Ptr< AdaptiveMedianBGS > Algorithms::BackgroundSubtraction::createAdaptiveMedianBGS( unsigned char low_threshold, unsigned char high_threshold, int samplingRate, int learning_frames )
{
    cv::Ptr< AdaptiveMedianBGS > bgs( new AdaptiveMedianBGS() );
    bgs->setLowThreshold( low_threshold );
    bgs->setHighThreshold( high_threshold );
    bgs->setSamplingRate( samplingRate );
    bgs->setLearningFrames( learning_frames );
    return bgs;
}
