#pragma once
#include <opencv2/opencv.hpp>
#include "object_item.h"
#include <vector>

using namespace cv;
class opencv_capture
{
	

private:
	VideoCapture _video_source;
	object_item _object;
	object_item _roboArm;
	object_item _work_object;
	Mat _original_img;
	Mat _proccesed_img;
	Mat _canny_output;
	int _xPosition;
	int _yPosition;
	int _max_objects;
	int num_objects;
	int home_flag;
	int tool_found_flag;
	

	void morphOps(int object);
	void trackObject(int object);
	void drawObject(int object);
public:
	/////////////////////////////////////////////////
	/*public class variables*/
	int state;
	int homing;
	bool mouseIsDragging;
	bool mouseMove;
	bool rectangleSelected;
	bool calibrationMode;
	cv::Point initialClickPoint, currentMousePoint;
	cv::Rect rectangleROI;
	std::vector<int> H_ROI, S_ROI, V_ROI;
	Mat threshold_obj;
	Mat threshold_robo;
	Mat threshold_work;
	Mat thresh;
	///////////////////////////////////////////////////////////
	/*public functions*/
	int  get_state();
	Point2f * find_origin(Point2f * rect,int corner);		//FUNCTION TO FIND ORIGIN OF SQUARE
	void state_write();										//FUNCTION THAT WRITES THE STATE OF PROGRAM
	int * getPosition(int object);							//FUNCTION THAT RETURNS POSITION OF OBJECT
	char * getPositionChar(int object);
	void machine_vision();									//FUNCTION THAT FINDS OBJECTS
	void draw_contours();									//DRAWS CONTOURS
	char * home_robot();
	opencv_capture(int camera);
	void object_hsv_set(int * data);						//AUTO DETECT HSV OF OBJECT YOU WANT TO ACQUIRE
	void object_hsv_reset();								//RESET HSV
	void recordHSV_VALUE();									//DETECT HSV IN USER DEFINED SQUARE
	~opencv_capture();
};

