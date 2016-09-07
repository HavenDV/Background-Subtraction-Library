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

#include <iostream>
#include <opencv2\videoio.hpp>

#include "AdaptiveMedianBGS.hpp"
#include "GrimsonGMM.hpp"
#include "ZivkovicAGMM.hpp"
#include "MeanBGS.hpp"
#include "WrenGA.hpp"
#include "PratiMediodBGS.hpp"
#include "Eigenbackground.hpp"

int     main( int argc, const char* argv[] )
{
    // read data from AVI file
    cv::VideoCapture reader( "examples/fountain.avi" );
    if( !reader.isOpened() )
    {
        std::cerr << "Could not open AVI file." << std::endl;
        return 0;
    }

    // retrieve information about AVI file
    int width = static_cast< int >( reader.get( cv::CAP_PROP_FRAME_WIDTH ) );
    int height = static_cast< int >( reader.get( cv::CAP_PROP_FRAME_HEIGHT ) );
    double fps = reader.get( cv::CAP_PROP_FPS );
    unsigned int num_frames = static_cast< unsigned int >( reader.get( cv::CAP_PROP_FRAME_COUNT ) );

    // setup writer:
    cv::VideoWriter writer( "output/results.avi", -1, fps, cv::Size( width, height ), false );

    // setup background subtraction algorithm
    auto bgs = Algorithms::BackgroundSubtraction::createAdaptiveMedianBGS();

    /*
    Algorithms::BackgroundSubtraction::GrimsonParams params;
    params.SetFrameSize(width, height);
    params.LowThreshold() = 3.0f*3.0f;
    params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing
    params.Alpha() = 0.001f;
    params.MaxModes() = 3;

    Algorithms::BackgroundSubtraction::GrimsonGMM bgs;
    bgs.Initalize(params);
    //*/

    /*
    Algorithms::BackgroundSubtraction::ZivkovicParams params;
    params.SetFrameSize(width, height);
    params.LowThreshold() = 5.0f*5.0f;
    params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing
    params.Alpha() = 0.001f;
    params.MaxModes() = 3;

    Algorithms::BackgroundSubtraction::ZivkovicAGMM bgs;
    bgs.Initalize(params);
    //*/

    /*
    Algorithms::BackgroundSubtraction::MeanParams params;
    params.SetFrameSize(width, height);
    params.LowThreshold() = 3*30*30;
    params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing
    params.Alpha() = 1e-6f;
    params.LearningFrames() = 30;

    Algorithms::BackgroundSubtraction::MeanBGS bgs;
    bgs.Initalize(params);
    //*/

    /*
    Algorithms::BackgroundSubtraction::WrenParams params;
    params.SetFrameSize(width, height);
    params.LowThreshold() = 3.5f*3.5f;
    params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing
    params.Alpha() = 0.005f;
    params.LearningFrames() = 30;

    Algorithms::BackgroundSubtraction::WrenGA bgs;
    bgs.Initalize(params);
    //*/

    /*
    Algorithms::BackgroundSubtraction::PratiParams params;
    params.SetFrameSize( width, height );
    params.LowThreshold() = 30;
    params.HighThreshold() = 2 * params.LowThreshold();	// Note: high threshold is used by post-processing 
    params.SamplingRate() = 5;
    params.HistorySize() = 16;
    params.Weight() = 5;

    Algorithms::BackgroundSubtraction::PratiMediodBGS bgs;
    bgs.Initalize( params );
    //*/

    /*
    Algorithms::BackgroundSubtraction::EigenbackgroundParams params;
    params.SetFrameSize(width, height);
    params.LowThreshold() = 15*15;
    params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing
    params.HistorySize() = 100;
    params.EmbeddedDim() = 20;

    Algorithms::BackgroundSubtraction::Eigenbackground bgs;
    bgs.Initalize(params);
    //*/

    // perform background subtraction of each frame
    // setup buffer to hold individual frames from video stream
    RgbImage frame_data;
    for( unsigned int i = 0; i < num_frames - 1; ++i )
    {
        if( i % 100 == 0 )
        {
            std::cout << "Processing frame " << i << " of " << num_frames << "..." << std::endl;
        }

        // grad next frame from input video stream
        reader >> frame_data;
        if( frame_data.empty() )
        {
            std::cerr << "Could not grab AVI frame." << std::endl;
            return 0;
        }

        // setup marks to hold results of low thresholding
        BwImage low_threshold_mask;

        // perform background subtraction
        bgs->apply( frame_data, low_threshold_mask );

        // save results
        writer.write( low_threshold_mask );
    }

    return 0;
}
