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
* Eigenbackground.cpp
*
* Purpose: Implementation of the Eigenbackground background subtraction 
*					 algorithm developed by Oliver et al.
*
* Author: Donovan Parks, September 2007
*
* "A Bayesian Computer Vision System for Modeling Human Interactions"
*   Nuria Oliver, Barbara Rosario, Alex P. Pentland 2000
*
******************************************************************************/

#include "Eigenbackground.hpp"

using namespace Algorithms::BackgroundSubtraction;

Eigenbackground::Eigenbackground()
{}

Eigenbackground::~Eigenbackground()
{}

void Eigenbackground::Initalize(const BgsParams& param)
{
	m_params = (EigenbackgroundParams&)param;
	
	//m_background = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 3);
	//m_background.Clear();
    m_background.create( m_params.Width(), m_params.Height() );
    m_background.setTo( RgbPixel( BACKGROUND, BACKGROUND, BACKGROUND ) );
}

void Eigenbackground::InitModel(const RgbImage& data)
{
    m_pcaData.release();
    m_pcaAvg.release();
    m_eigenValues.release();
    m_eigenVectors.release();

	m_pcaData.create(m_params.HistorySize(), m_params.Size()*3, CV_8UC1);

	//m_background.Clear();
    m_background.setTo( RgbPixel( BACKGROUND, BACKGROUND, BACKGROUND ) );
}

void Eigenbackground::Update(int frame_num, const RgbImage& data,  const BwImage& update_mask)
{
	// the eigenbackground model is not updated (serious limitation!)
}

void Eigenbackground::Subtract(int frame_num, const RgbImage& data,  
																BwImage& low_threshold_mask, BwImage& high_threshold_mask)
{
	// create eigenbackground
	if(frame_num == m_params.HistorySize())
	{
		// create the eigenspace
		m_pcaAvg.create( 1, m_pcaData.cols, CV_32F );
		m_eigenValues.create( m_pcaData.rows, 1, CV_32F );
		m_eigenVectors.create( m_pcaData.rows, m_pcaData.cols, CV_32F );
		cvCalcPCA(m_pcaData.data, m_pcaAvg.data, m_eigenValues.data, m_eigenVectors.data, CV_PCA_DATA_AS_ROW);

		int index = 0;
		for(unsigned int r = 0; r < m_params.Height(); ++r)
		{
			for(unsigned int c = 0; c < m_params.Width(); ++c)
			{
				for(int ch = 0; ch < m_background.channels(); ++ch)
				{
					m_background.at< RgbPixel >(r,c)[0] = (unsigned char)(cvmGet((CvMat*)&m_pcaAvg,0,index)+0.5); // FIXME ch ???
					index++;
				}
			}
		}
	}

	if(frame_num >= m_params.HistorySize())
	{
		// project new image into the eigenspace
		int w = data.cols;
    int h = data.rows;
		int ch = data.channels();
    CvMat* dataPt = cvCreateMat(1, w*h*ch, CV_8UC1); 
		CvMat data_row;
    cvGetRow(dataPt, &data_row, 0);
    cvReshape(&data_row, &data_row, 3, h ); 
    cvCopy(data.data, &data_row); 

		CvMat* proj = cvCreateMat(1, m_params.EmbeddedDim(), CV_32F);
		cvProjectPCA(dataPt, m_pcaAvg.data, m_eigenVectors.data, proj);

		// reconstruct point
		CvMat* result = cvCreateMat(1, m_pcaData.cols, CV_32F);
		cvBackProjectPCA(proj, m_pcaAvg.data, m_eigenVectors.data, result);

		// calculate Euclidean distance between new image and its eigenspace projection
		int index = 0;
		for(unsigned int r = 0; r < m_params.Height(); ++r)
		{
			for(unsigned int c = 0; c < m_params.Width(); ++c)
			{
				double dist = 0;
				bool bgLow = true;
				bool bgHigh = true;
				for(int ch = 0; ch < 3; ++ch)
				{
					dist = (data.at< RgbPixel >(r,c)[ch] - cvmGet(result,0,index))*(data.at< RgbPixel >(r,c)[ch] - cvmGet(result,0,index));
					if(dist > m_params.LowThreshold())
						bgLow = false;
					if(dist > m_params.HighThreshold())
						bgHigh = false;
					index++;
				}
				
				if(!bgLow)
				{
					low_threshold_mask.at< uchar >(r,c) = FOREGROUND;
				}
				else
				{
					low_threshold_mask.at< uchar >(r,c) = BACKGROUND;
				}

				if(!bgHigh)
				{
					high_threshold_mask.at< uchar >(r,c) = FOREGROUND;
				}
				else
				{
					high_threshold_mask.at< uchar >(r,c) = BACKGROUND;
				}
			}
		}
		
		cvReleaseMat(&result);
		cvReleaseMat(&proj);		
		cvReleaseMat(&dataPt);
	}
	else 
	{
		// set entire image to background since there is not enough information yet
		// to start performing background subtraction
		for(unsigned int r = 0; r < m_params.Height(); ++r)
		{
			for(unsigned int c = 0; c < m_params.Width(); ++c)
			{
				low_threshold_mask.at< uchar >(r,c) = BACKGROUND;
				high_threshold_mask.at< uchar >(r,c) = BACKGROUND;
			}
		}
	}

	UpdateHistory(frame_num, data);
}

void Eigenbackground::UpdateHistory(int frame_num, const RgbImage& new_frame)
{
	if(frame_num < m_params.HistorySize())
	{
		cv::Mat src_row;
        //cvGetRow(m_pcaData.data, &src_row, frame_num);
        //cvReshape(&src_row, &src_row, 3, new_frame.rows); 
        //new_frame.copyTo( &src_row );
	}
}