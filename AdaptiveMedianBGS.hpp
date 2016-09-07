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

/****************************************************************************
*
* AdaptiveMedianBGS.hpp
*
* Purpose: Implementation of the simple adaptive median background 
*		  		 subtraction algorithm described in:
*	  			 "Segmentation and tracking of piglets in images"
* 						by McFarlane and Schofield
*
* Author: Donovan Parks, September 2007

Example:
    auto bgs = Algorithms::BackgroundSubtraction::createAdaptiveMedianBGS();
    bgs->setLowThreshold( 40 );
    bgs->setHighThreshold( 80 );
    bgs->setSamplingRate( 7 );
    bgs->setLearningFrames( 30 );

    cv::Mat data;
    cv::Mat fgmask;
    bgs->apply( data, fgmask );
******************************************************************************/
#ifndef _ADAPTIVE_MEDIAN_BGS_H_		
#define _ADAPTIVE_MEDIAN_BGS_H_		

#include <opencv2/video.hpp>
#include "Image.hpp"

namespace Algorithms
{
namespace BackgroundSubtraction
{

// --- Adaptive Median BGS algorithm ---
class AdaptiveMedianBGS : public cv::BackgroundSubtractor
{
public:
    AdaptiveMedianBGS();
    ~AdaptiveMedianBGS();

    void    setLowThreshold( unsigned char low_threshold ) { m_low_threshold = low_threshold; }
    void    setHighThreshold( unsigned char high_threshold ) { m_high_threshold = high_threshold; }
    void    setSamplingRate( int samplingRate ) { m_samplingRate = samplingRate; }
    void    setLearningFrames( int learning_frames ) { m_learning_frames = learning_frames; }

    unsigned char   getLowThreshold() const { return m_low_threshold; }
    unsigned char   getHighThreshold() const { return m_high_threshold; }
    int             getSamplingRate() const { return m_samplingRate; }
    int             getLearningFrames() const { return m_learning_frames; }

    void    apply( cv::InputArray image, cv::OutputArray fgmask, double learningRate = -1 );
    void    getBackgroundImage( cv::OutputArray backgroundImage ) const;

private:
    void    InitModel( const RgbImage& data );
	void    SubtractPixel( int r, int c, const RgbPixel & pixel, 
                           unsigned char & low_threshold, unsigned char & high_threshold );
    void    Subtract( int frame_num, const RgbImage& data,
                      BwImage& low_threshold_mask, BwImage& high_threshold_mask );
    void    Update( int frame_num, const RgbImage& data, const BwImage& update_mask );

    int             m_i;
    unsigned char   m_low_threshold;
    unsigned char   m_high_threshold;
    int             m_samplingRate;
    int             m_learning_frames;
	RgbImage        m_median;

};

//Note: high threshold is used by post - processing
cv::Ptr< AdaptiveMedianBGS >    createAdaptiveMedianBGS( 
    unsigned char low_threshold = 40U, unsigned char high_threshold = 80U,
    int samplingRate = 7, int learning_frames = 30 );

};
};

#endif