// OpenCV offline
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <highgui.h>


using namespace cv;
using namespace std;


Vec<int,4> coordinates;
int min_dist_slider_max = 100;
int min_dist_slider;
int min_dist;

int canny_threshold_slider_max = 255;
int canny_threshold_slider;
int canny_threshold;

int center_threshold_slider_max = 255;
int center_threshold_slider;
int center_threshold;

int min_radius_slider_max = 99;
int min_radius_slider;
int min_radius;

int max_radius_slider_max = 100;
int max_radius_slider;
int max_radius;

int med_blur_slider_max = 255;
int med_blur_slider;
int med_blur;

int bin_threshold_slider_max = 255;
int bin_threshold_slider;
int bin_threshold;

bool isDrawing = false;
Point start, end;

void drawBox(Point start, Point end, Mat& img){

Scalar color = (0,255,0);
rectangle(img, start, end, color, 1, 8, 0);
imshow("set", img);
return;
}

void mouseEvent(int evt, int x, int y, int flags, void* param){
	if(isDrawing){
		if(evt==CV_EVENT_LBUTTONUP){
	    		printf("up %d %d\n",x,y);
	        	isDrawing = false;
		        end.x = x;
		        end.y = y;
		        cv::Mat* image  = static_cast<cv::Mat *>(param);
			drawBox(start, end, *image);
			coordinates[2] = end.x-start.x;
			coordinates[3] = end.y-start.y;
	        	return;
    		}
	}
	else{
		if(evt==CV_EVENT_LBUTTONDOWN){
	        	printf("down %d %d\n",x,y);
		        isDrawing = true;
		        start.x = x;
		        start.y = y;
			coordinates[0] = start.x;
			coordinates[1] = start.y;
        		return;
    		}
	}
}

void min_dist_trackbar(int, void*){
	if (min_dist_slider==0){
		min_dist_slider=1;
	}
	min_dist = (int) min_dist_slider;
}

void canny_threshold_trackbar(int, void*){
	if (canny_threshold_slider==0){
		canny_threshold_slider=1;
	}
	canny_threshold = (int) canny_threshold_slider;
}

void center_threshold_trackbar(int, void*){
	if (center_threshold_slider==0){
		center_threshold_slider=1;
	}
	center_threshold = (int) center_threshold_slider;
}

void min_radius_trackbar(int, void*){
	if (min_radius_slider==0){
		min_radius_slider=1;
	}
	min_radius = (int) min_radius_slider;
}

void max_radius_trackbar(int,void*){
	max_radius = (int) max_radius_slider;
	min_radius_slider_max = max_radius-1;
}

void med_blur_trackbar(int,void*){
	if (med_blur_slider % 2 == 0){
		med_blur_slider=med_blur_slider+1;
	}
	if (med_blur_slider < 1){
		med_blur_slider=1;
	}
	med_blur = (int) med_blur_slider;
}

void bin_threshold_trackbar(int,void*){
	bin_threshold = (int) bin_threshold_slider;
}

int main()
{
	// Load video
	string filename = "test.avi";
	VideoCapture cap(filename);
	Mat tmp;
	cap.grab();
	cap.retrieve(tmp);
	namedWindow("set");
	imshow("set",tmp);	
	char kb = 0;
	while(kb != 'r'){
	        cvSetMouseCallback("set", mouseEvent, &tmp); 
		kb = cvWaitKey(30);	
	}
	
	// debug
	for( int i = 0; i < 4; i=i+1 ){
		cout << coordinates[i];
		cout << " ";
	}

	double fps = cap.get(CV_CAP_PROP_FPS);
	int dp = 1;
	//int min_dist = sqrt( pow(tmp.rows,2) + pow(tmp.cols,2));
	min_dist = 1;
	min_dist_slider = 1;
	canny_threshold = 10;
	canny_threshold_slider = 10;
	center_threshold = 10;
	center_threshold_slider = 10;
	
	max_radius = 140;
	max_radius_slider = 100;
	max_radius_slider_max = min(coordinates[2]-coordinates[0],coordinates[3]-coordinates[1]);
	min_radius = 75;
	min_radius_slider = 75;
	min_radius_slider_max = max_radius_slider_max-1;
	bin_threshold = 21;
	bin_threshold_slider = 21;
	med_blur = 75;
	med_blur_slider_max = min(coordinates[2]-coordinates[0],coordinates[3]-coordinates[1])-10;
	med_blur_slider = 75;
	Mat window;
	Rect myROI(coordinates[0],coordinates[1],coordinates[2],coordinates[3]);
	Vec<float,2> offset;
	offset[0] = coordinates[0];
	offset[1] = coordinates[1];
	namedWindow("window",1);
	namedWindow("filtered",1);
	// caputer loop
	char key = 0;
	bool refresh = true;
	
	VideoWriter demo;
	int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));
	cout << ex;
	Size S = Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH), (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	demo.open("eye-tracking-demo.avi",ex,15,S,true);
	
	// make sliders
	createTrackbar("Min Distance", "filtered", &min_dist_slider,min_dist_slider_max,min_dist_trackbar);
	createTrackbar("Canny Threshold", "filtered", &canny_threshold_slider, canny_threshold_slider_max, canny_threshold_trackbar);
	createTrackbar("Center Threshold", "filtered", &center_threshold_slider,center_threshold_slider_max, center_threshold_trackbar);
	createTrackbar("Min Radius", "filtered", &min_radius_slider,min_radius_slider_max, min_radius_trackbar);
	createTrackbar("Max Radius", "filtered", &max_radius_slider,max_radius_slider_max, max_radius_trackbar);
	createTrackbar("Median blur", "filtered", &med_blur_slider, med_blur_slider_max, med_blur_trackbar);
	createTrackbar("Bin Threshold", "filtered", &bin_threshold_slider, bin_threshold_slider_max, bin_threshold_trackbar);
	for(;;)
	{
		//reinitialize
		key = 0;
		
		

		Mat image;
		Mat i1, i2;
		if(refresh)
		{
			cap.grab();
			cap.retrieve(i1);
			cap.grab();
			cap.retrieve(i2);
		} 
		//cap.retrieve(image);
		addWeighted(i1, 0.5, i2, 0.5, 0.0, image);
		
		// convert to gray
		
		Mat image_gray;
		image_gray = image(myROI);

		//medianBlur(image_gray, image_gray, 75);		
		cvtColor( image_gray, image_gray, CV_BGR2GRAY);
		medianBlur(image_gray,image_gray,med_blur);
		
		

		//image_gray.convertTo( image_gray, -1, 3, 0);
		threshold(image_gray, image_gray, bin_threshold, 255, THRESH_BINARY);
		GaussianBlur( image_gray, image_gray, Size(9, 9), 2, 2);
		vector<Vec3f> circles;
		
		//Apply the Hough Transform to find the circles
		HoughCircles( image_gray, circles, CV_HOUGH_GRADIENT, dp, min_dist, canny_threshold, center_threshold, min_radius, max_radius);
		
		//Draw circles detected
		float x=0;
		float y=0;
		float r=0;
		if (circles.size()>0){
			for( size_t i=0; i< circles.size(); i++)
			{
				x=x+circles[i][0];
				y=y+circles[i][1];
				r=r+circles[i][2];
			}
		
			float centerX=x/circles.size()+offset[0];
			float centerY=y/circles.size()+offset[1];
			//float centerX=circles[0][0]+offset[0];
			//float centerY=circles[0][1]+offset[1];	
		
			Point center(cvRound(centerX), cvRound(centerY));
			//int radius = cvRound(circles[0][2]);
			int radius = cvRound(r/circles.size());
			// circle center
			circle(image,center,3,Scalar(0,255,0),-1,8,0);
			// circle outline
			circle(image,center,radius,Scalar(0,0,255),3,8,0);
		}
		else{
			cout << "eye-blink";
		}	
		demo.write(image);
		
		imshow("window",image);
		imshow("filtered",image_gray);
		key = waitKey(30);
		if(key>=0)
		{
			if(key=='q')
			{
				cout << dp;
				cout << min_dist;
				cout << canny_threshold;
				cout << center_threshold;
				cout << min_radius;
				cout << max_radius;
				break;
			}
			else if(key == 'd')
			{
				dp = dp+1;
				refresh = false;
				
			}
			else if(key == 'c')
			{
				if(dp>1)
				{
					dp = dp-1;
				}
				refresh = false;
			}
			else if(key == 'f')
			{
				min_dist = min_dist + 30;
				refresh = false;
			}
			else if(key == 'v')
			{
				if(min_dist > 30);
				{
					min_dist = min_dist - 30;
				}
				refresh = false;
			}
			else if(key == 'g')
			{
				canny_threshold = canny_threshold + 1;
				refresh = false;
			}
			else if(key == 'b')
			{
				if(canny_threshold>1)
				{
					canny_threshold = canny_threshold - 1;
				}
				refresh = false;
			}
			else if(key == 'h')
			{
				center_threshold = center_threshold + 5;
				refresh = false;
			}
			else if(key == 'n')
			{
				if(center_threshold>5)
				{
					center_threshold = center_threshold-5;
				}
				refresh = false;
			}
			else if(key == 'j')
			{
				min_radius = min_radius + 10;
				refresh = false;
			}
			else if(key == 'm')
			{
				if(min_radius > 10)
				{
					min_radius = min_radius - 10;
				}
				refresh = false;
			}
			else if(key == 'k')
			{
				max_radius = max_radius + 10;
				refresh = false;
			}
			else if(key == 'l')
			{
				if(max_radius > 10+min_radius)
				{
					max_radius = max_radius - 10;
				}
				refresh = false;
			}
			else
			{
				refresh = true;
			}
		}
	}
	cout << dp;
        cout << min_dist;
        cout << canny_threshold;
        cout << center_threshold;
        cout << min_radius;
        cout << max_radius;
	return 0;
	
}
