// OpenCV offline
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;




Vec<int,4> coordinates;

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
	int min_dist = sqrt( pow(tmp.rows,2) + pow(tmp.cols,2));
	int canny_threshold = 15;
	int center_threshold = 15;
	int min_radius = 75;
	int max_radius = 120;
	Mat window;
	Rect myROI(coordinates[0],coordinates[1],coordinates[2],coordinates[3]);
	Vec<float,2> offset;
	offset[0] = coordinates[0];
	offset[1] = coordinates[1];
	namedWindow("window",1);
	// caputer loop
	char key = 0;
	bool refresh = true;
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
		//cap.retrieve(image_gray);
		//image_gray = image_gray(myROI);	
		image_gray = image(myROI);
		
		cvtColor( image_gray, image_gray, CV_BGR2GRAY);
		
		// find circle for iris:
		// Reduce noise so we avoid false circle detection
		GaussianBlur( image_gray, image_gray, Size(9, 9), 2, 2);
		image_gray.convertTo( image_gray, -1, 3, 0);
		vector<Vec3f> circles;
		
		//Apply the Hough Transform to find the circles
		HoughCircles( image_gray, circles, CV_HOUGH_GRADIENT, dp, min_dist, canny_threshold, center_threshold, min_radius, max_radius);
		
		//Draw circles detected
		//for( size_t i=0; i< circles.size(); i++)
		//{
		float centerX=circles[0][0]+offset[0];
		float centerY=circles[0][1]+offset[1];	
		//Point center(cvRound(add(circles[0][0],offset[0])), cvRound(add(circles[0][1],offset[1])));
		Point center(cvRound(centerX), cvRound(centerY));
		int radius = cvRound(circles[0][2]);
		// circle center
		circle(image,center,3,Scalar(0,255,0),-1,8,0);
		// circle outline
		circle(image,center,radius,Scalar(0,0,255),3,8,0);
			
		//}
		
		imshow("window",image);
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