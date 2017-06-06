#include "opencv_capture.h"
#include <string>
#include <cmath>

void opencv_capture::morphOps(int object)
{
	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle
	if (object == 0)
	{
		threshold_obj.copyTo(thresh);
	}
	else if (object == 1)
	{
		threshold_robo.copyTo(thresh);
	}
	else if (object == 2)
	{
		threshold_work.copyTo(thresh);
	}
	else
	{
		return;
	}
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);
	
}

void opencv_capture::trackObject(int object)
{
	//these two vectors needed for output of findContours
	if (state == 0)
	{
		return;
	}
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	
	//use moments method to find our filtered object
	double refArea = 0;
	int largestIndex = 0;
	bool objectFound = false;

	if (object < 2)
	{
		findContours(thresh, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		if (hierarchy.size() > 0) {
			int numObjects = hierarchy.size();
			//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
			if (numObjects < 2) {
				for (int index = 0; index >= 0; index = hierarchy[index][0]) {

					Moments moment = moments((cv::Mat)contours[index]);
					double area = moment.m00;

					//if the area is less than 20 px by 20px then it is probably just noise
					//if the area is the same as the 3/2 of the image size, probably just a bad filter
					//we only want the object with the largest area so we save a reference area each
					//iteration and compare it to the area in the next iteration.
					if (area > 200 && area<250000 && area>refArea) {
						_xPosition = moment.m10 / area;
						_yPosition = moment.m01 / area;
						objectFound = true;
						refArea = area;
						//save index of largest contour to use with drawContours
						largestIndex = index;
						if (object == 1)
						{
							tool_found_flag = 1;
						}
					}
					else objectFound = false;


				}
				//let user know you found an object
				if (objectFound == true) {
					putText(_original_img, "Tracking Object", Point(0, 100), 2, 1, Scalar(0, 255, 160), 2);
					//draw object location on screen
					drawObject(object);
					//draw largest contour
					//drawContours(cameraFeed, contours, largestIndex, Scalar(0, 255, 255), 2);
				}

			}
			else putText(_original_img, "can't find adjust", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
		}
	}
	if (object == 1 && !objectFound)
	{
		tool_found_flag = 0;

	}
	else if(object == 2 && state == 3)
	{
	
		//blur(thresh, thresh, Size(3, 3));
		/// Find contours
		double largest_area = 0;
		int largest_contour_index;
		findContours(thresh, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		Point2f rect_points[4];
		Rect bounding_rect;
		int flag = 1;
		std::vector<RotatedRect> minRect(1);
		for (size_t i = 0; i < contours.size(); i++) // iterate through each contour.
		{
			double area = contourArea(contours[i]);  //  Find the area of contour

			if (area > largest_area)
			{
				largest_area = area;
				largest_contour_index = i;  
				flag = 0;
					//Store the index of largest contour
				//bounding_rect = boundingRect(Mat(contours[i]));
			}
		}
		if (flag == 1)
		{
			return;
		}
			minRect[0] = minAreaRect(contours[largest_contour_index]);
			minRect[0].points(rect_points);
			for (int j = 0; j < 4; j++)
				line(_original_img, (rect_points[j]), (rect_points[(j + 1) % 4]), Scalar(0, 255, 255), 2, 8);
		
		
			find_origin(rect_points,0);

	}
		
	
}	
		


void opencv_capture::drawObject(int object)
{
	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

	int R, G, B;
	//'if' and 'else' statements to prevent
	//memory errors from writing off the screen (ie. (-25,-25) is not within the window)
	switch (object)
	{
	case 0:
		R = 0;
		B = 255;
		G = 125;
		break;
	case 1:
		R = 255;
		B = 125;
		G = 125;
		break;
	case 2:
		R = 100;
		B = 125;
		G = 255;
		break;
	default:
		R = 255;
		B = 255;
		G = 255;
	}
	circle(_original_img, Point(_xPosition, _yPosition), 20, Scalar(R, G, B), 2);
	if (_yPosition - 25>0)
		line(_original_img, Point(_xPosition, _yPosition), Point(_xPosition, _yPosition - 25), Scalar(R, G, B), 2);
	else line(_original_img, Point(_xPosition, _yPosition), Point(_xPosition, 0), Scalar(R, G, B), 2);
	if (_yPosition + 25<480)
		line(_original_img, Point(_xPosition, _yPosition), Point(_xPosition, _yPosition + 25), Scalar(R, G, B), 2);
	else line(_original_img, Point(_xPosition, _yPosition), Point(_xPosition, 480), Scalar(R, G, B), 2);
	if (_xPosition - 25>0)
		line(_original_img, Point(_xPosition, _yPosition), Point(_xPosition - 25, _yPosition), Scalar(R, G, B), 2);
	else line(_original_img, Point(_xPosition, _yPosition), Point(0, _yPosition), Scalar(R, G, B), 2);
	if (_xPosition + 25<640)
		line(_original_img, Point(_xPosition, _yPosition), Point(_xPosition + 25, _yPosition), Scalar(R, G, B), 2);
	else line(_original_img, Point(_xPosition, _yPosition), Point(640, _yPosition), Scalar(R, G, B), 2);

	putText(_original_img, std::to_string(_xPosition) + "," + std::to_string(_yPosition), Point(_xPosition, _yPosition + 30), 1, 1, Scalar(R, G, B), 2);


	switch (object)
	{
	case 0:
		_object._x_pos = _xPosition;
		_object._y_pos = _yPosition;
		break;
	case 1:
		_roboArm._x_pos = _xPosition;
		_roboArm._y_pos = _yPosition;
		break;
	}
		
}



int opencv_capture::get_state()
{
	return state;
}

// FUNCTION TO FIND THE ORIGIN IN THE BOTTOM MOST CORNER
Point2f * opencv_capture::find_origin(Point2f * rect, int corner)
{

	int origin_x =0;
	int x_axis = 0;
	int y_axis=0;
	double corner_x;		//CORNER X POINT
	double corner_y;		//CORNER Y POINT
		
	switch
		(corner)
	{
	case 0:			//BOTTOM LEFT
			corner_x = 0;
			corner_y = 540;
		break;		
		case 1:		//TOP LEFT
			corner_x = 0;
			corner_y = 0;
		case 2:		//BOTTOM RIGHT
			corner_x = 640;
			corner_y = 540;
		case 3:		//TOP RIGHT
			corner_x = 480;
			corner_y = 0;

	}

	double temp_distance = 10000;
	double temp_distancey = 10000;
	double distancex;
	double distance_angle = 0;
	double temp_x = 0;
	double distancey;
	double distanceTotal;
	for (int j = 0; j < 4; j++)
	{
		
		distancex =	abs(rect[j].x - corner_x);
		distancey = abs(rect[j].y - corner_y);

		distanceTotal = sqrt((distancex * distancex) + (distancey * distancey)); // find the smallest vector to point;


		if (distanceTotal < temp_distance)		
		{
			origin_x = j;
			temp_distance = distanceTotal;
		}
	}

	temp_distance = 90;
	for (int i = 0; i < 4; i++)
	{
		if (i == origin_x)
		{

		}
		else {
			distancex = abs(rect[i].x - corner_x);
			distancey = abs(rect[i].y - corner_y);
			
			distance_angle = atan2(distancex, distancey);
			if (distance_angle < temp_distance)
			{
				temp_distance = distance_angle;
				y_axis = i;

			}
			if (distance_angle > temp_x)
			{
				temp_x = distance_angle;
				x_axis = i;

			}

		}
	}

	float x1 = ((rect[origin_x].x - rect[x_axis].x) / 2) + rect[x_axis].x;
	float y1 = ((rect[origin_x].y - rect[x_axis].y) / 2) + rect[x_axis].y;
	Point2f xaxis;
	xaxis.x = x1;
	xaxis.y = y1;
	x1 = ((rect[origin_x].x - rect[y_axis].x) / 2) + rect[y_axis].x;
	y1 = ((rect[origin_x].y - rect[y_axis].y) / 2) + rect[y_axis].y;
	Point2f yaxis;
	yaxis.x = x1;
	yaxis.y = y1;
	arrowedLine(_original_img, (rect[origin_x]), xaxis, Scalar(0, 0, 255), 4, 8, 0, 0.1);
	arrowedLine(_original_img, (rect[origin_x]), yaxis, Scalar(0, 255, 0), 4, 8, 0, 0.1);

	putText(_original_img,"X", Point(xaxis.x, (xaxis.y+40)), 1, 1, Scalar(0,60, 255), 5);
	putText(_original_img, "Y", Point((yaxis.x-40), yaxis.y), 1, 1, Scalar(0, 255, 60),5);

	_work_object._x_pos = rect[0].x;
	_work_object._y_pos = rect[0].y;


	putText(_original_img, std::to_string(_work_object._x_pos) + "," + std::to_string(_work_object._y_pos), Point(_work_object._x_pos, _work_object._y_pos), 1, 1, Scalar(120, 120, 255), 2);


	_work_object._x_pos = rect[origin_x].x;
	_work_object._y_pos = rect[origin_x].y;

	return nullptr;
}

void opencv_capture::state_write()
{
	std::string state_string = "current_state:";
	state_string += std::to_string(state);

	putText(_original_img, state_string, cvPoint(240, 15),FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 0, 250), 1, CV_AA);

}

int * opencv_capture::getPosition(int object)
{
	int coords[2];
	switch (object)
	{
	case 0:
		coords[0] = _object._x_pos;
		coords[1] = _object._y_pos;
		break;
	case 1:
		coords[0] = _roboArm._x_pos;
		coords[1] = _roboArm._y_pos;
		break;
	case 2:
		coords[0] = _work_object._x_pos;
		coords[1] = _work_object._y_pos;
		break;

	}
	
	return coords;
}

char * opencv_capture::getPositionChar(int object)
{
	char coords[6];
	int x_origin = 0;
	int y_origin = 480;
	switch (object)
	{
		case 0:
		x_origin = _object._x_pos - x_origin;
		y_origin = _object._y_pos - y_origin;
		coords[0] = (char)(x_origin>>8);
		coords[1] = (char)x_origin;
		coords[2] = (char)(y_origin>>8);
		coords[3] = (char)y_origin;
		coords[4] = (char)0;
		coords[5] = (char)0;
		break;
	case 1:
		x_origin = _roboArm._x_pos - x_origin;
		y_origin = _roboArm._y_pos - y_origin;
		coords[0] = (char)(x_origin>>8);
		coords[1] = (char)x_origin;
		coords[2] = (char)(y_origin>>8);
		coords[3] = (char)y_origin;
		coords[4] = (char)0;
		coords[5] = (char)0;
		break;
	case 2:
		coords[0] = (char)_work_object._x_pos>>7;
		coords[1] = (char)_work_object._x_pos;
		coords[2] = (char)_work_object._y_pos>>7;
		coords[3] = (char)_work_object._y_pos;
		coords[4] = (char)0;
		coords[5] = (char)0;
		break;
	}


	return coords;
}

void opencv_capture::machine_vision()
{
	this->_video_source.read(_original_img);
	recordHSV_VALUE();
	state_write();
	for(int i = 0; i < num_objects;i++)
	{
			morphOps(i);
			trackObject(i);
	}
	if (state == 8)
	{
		draw_contours();
	}
	else
	{
		imshow("ROBO GUI", _original_img);
	}
	char c =waitKey(15);
	if (c == 99)
	{
		calibrationMode = true;
	}
}

void opencv_capture::draw_contours()
{
	RNG rng(12345);
	Mat canny_output;
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;

	Canny(_original_img, canny_output, 100, 100*2, 3);
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
	}
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", drawing);
}

char * opencv_capture::home_robot()
{
	home_flag++;
	char coords[6];
	if (tool_found_flag)
	{
		
		if ( !(_roboArm._x_pos >= 312 &&_roboArm._x_pos <= 327))
		{
				int roboPos_X = _roboArm._x_pos;
				int roboPos_Y = _roboArm._y_pos;

				int robox_offset = (320 - roboPos_X) / 2;
				int roboy_offset = 240 - roboPos_Y;

				coords[0] = 0x01;
				coords[1] = 0xF8;
				coords[2] = (char)(roboy_offset >> 8);
				coords[3] = (char)roboy_offset;
				coords[4] = (char)(robox_offset >> 8);
				coords[5] = (char)robox_offset;

				homing = 1;
				return coords;
			
		}
		else 
		{
				coords[0] = 0x01;
				coords[1] = 0xF7;
				coords[2] = 0x00;
				coords[3] = 0x00;
				coords[4] = 0x00;
				coords[5] = 0x00;
				homing = 1;
			return coords;
		}
	}
	else
	{
		if (home_flag > 50000)
		{
			home_flag = 1;
		}
		homing = 0;
		char * null =  0;
		return null;
	}




}

opencv_capture::opencv_capture(int camera)
{
	
	this->_video_source.open(camera);
	namedWindow("ROBO GUI", WINDOW_NORMAL);
	this->_object = object_item();
	this->_max_objects = 3;
	this->state = 0;
	this->home_flag = 0;
}

void opencv_capture::object_hsv_set(int * data)
{

	switch (state)
	{
	case 1:
		_object.set_object(data);
		break;
	case 2:
		_roboArm.set_object(data);
		break;
	case 3:
		_work_object.set_object(data);
		break;
	case 4:
		_roboArm.set_object(data);
		break;
	default:
		_object.set_object(data);
		break;
	}
}

void opencv_capture::object_hsv_reset()
{
	_object.reset();
}

void opencv_capture::recordHSV_VALUE()
{

		int data[6];
		Mat HSV;
		
		//save HSV values for ROI that user selected to a vector
		if (mouseMove == false && rectangleSelected == true) {
			
			cvtColor(_original_img, HSV, COLOR_BGR2HSV);


			//clear previous vector values
			if (H_ROI.size()>0) H_ROI.clear();
			if (S_ROI.size()>0) S_ROI.clear();
			if (V_ROI.size()>0)V_ROI.clear();
			//if the rectangle has no width or height (user has only dragged a line) then we don't try to iterate over the width or height
			if (rectangleROI.width < 1 || rectangleROI.height < 1);//cout << "Please drag a rectangle, not a line" << endl;
			else {
				for (int i = rectangleROI.x; i<rectangleROI.x + rectangleROI.width; i++) {
					//iterate through both x and y direction and save HSV values at each and every point
					for (int j = rectangleROI.y; j<rectangleROI.y + rectangleROI.height; j++) {
						//save HSV value at this point
						H_ROI.push_back((int)HSV.at<cv::Vec3b>(j, i)[0]);
						S_ROI.push_back((int)HSV.at<cv::Vec3b>(j, i)[1]);
						V_ROI.push_back((int)HSV.at<cv::Vec3b>(j, i)[2]);
					}
				}
			}
			//reset rectangleSelected so user can select another region if necessary
			rectangleSelected = false;
			//set min and max HSV values from min and max elements of each array

			if (H_ROI.size()>0) {
				//NOTE: min_element and max_element return iterators so we must dereference them with "*"
				data[0] = *std::min_element(H_ROI.begin(), H_ROI.end());
				data[1]= *std::max_element(H_ROI.begin(), H_ROI.end());
				//cout << "MIN 'H' VALUE: " << H_MIN << endl;
				//cout << "MAX 'H' VALUE: " << H_MAX << endl;
			}
			if (S_ROI.size()>0) {
				data[2]= *std::min_element(S_ROI.begin(), S_ROI.end());
				data[3] = *std::max_element(S_ROI.begin(), S_ROI.end());
				//cout << "MIN 'S' VALUE: " << S_MIN << endl;
				//cout << "MAX 'S' VALUE: " << S_MAX << endl;
			}
			if (V_ROI.size()>0) {
				data[4] = *std::min_element(V_ROI.begin(), V_ROI.end());
				data[5] = *std::max_element(V_ROI.begin(), V_ROI.end());
				//	cout << "MIN 'V' VALUE: " << V_MIN << endl;
				//cout << "MAX 'V' VALUE: " << V_MAX << endl;
			}

			if (num_objects < 3)
			{
				num_objects++;
			}
			object_hsv_set(data);
		}
		



		if(mouseMove == true) 
		{
			//if the mouse is held down, we will draw the click and dragged rectangle to the screen
			rectangle(_original_img, initialClickPoint, cv::Point(currentMousePoint.x, currentMousePoint.y), cv::Scalar(0, 255, 0), 1, 8, 0);
		}
		if(num_objects>0)
		{
			cvtColor(_original_img, HSV, COLOR_BGR2HSV);
			inRange(HSV, _object.get_scalar_min(), _object.get_scalar_max(), threshold_obj);
			if (num_objects > 1)
			{
				inRange(HSV, _roboArm.get_scalar_min(), _roboArm.get_scalar_max(), threshold_robo);
			}
			if (state == 3)
			{
				inRange(HSV, _work_object.get_scalar_min(), _work_object.get_scalar_max(), threshold_work);
			}
		}
		//imshow("newWINDOW", HSV);
		//waitKey(0);
}

opencv_capture::~opencv_capture()
{
}
