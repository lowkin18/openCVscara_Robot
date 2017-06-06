// arduinoSerialPort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Serial_comPort.h"
#include <iostream>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include "opencv_capture.h"
#include "object_item.h"


Serial * ptr_msp_connection;
int gripper_flag;

static void onMouse(int event, int x, int y, int flags, void*param);

static void onMouse(int event, int x , int y, int flags, void*param)
{

	opencv_capture * _machine_vision = (opencv_capture*)param;
	if (_machine_vision->calibrationMode == true) 
	{
		
		
	}
		if (event == CV_EVENT_LBUTTONDOWN && _machine_vision->mouseIsDragging == false)
		{
			//keep track of initial point clicked
			_machine_vision->initialClickPoint = cv::Point(x, y);
			//user has begun dragging the mouse
			_machine_vision->mouseIsDragging = true;
		}
		/* user is dragging the mouse */
		if (event == CV_EVENT_MOUSEMOVE && _machine_vision->mouseIsDragging == true)
		{
			//keep track of current mouse point
			_machine_vision->currentMousePoint = cv::Point(x, y);
			//user has moved the mouse while clicking and dragging
			_machine_vision->mouseMove = true;
		}
		/* user has released left button */
		if (event == CV_EVENT_LBUTTONUP && _machine_vision->mouseIsDragging == true)
		{
			//set rectangle ROI to the rectangle that the user has selected
			_machine_vision->rectangleROI = Rect(_machine_vision->initialClickPoint, _machine_vision->currentMousePoint);

			//reset boolean variables
			_machine_vision->mouseIsDragging = false;
			_machine_vision->mouseMove = false;
			_machine_vision->rectangleSelected = true;
		}

		if (event == CV_EVENT_RBUTTONDOWN) 
		{
			//user has clicked right mouse button
			//Reset HSV Values
			//_machine_vision->object_hsv_reset();

			
			char position_data[6];
			char * ptr_position_data = position_data;

			position_data[0] = 0x01;
			if (gripper_flag == 0)
			{
				position_data[1] = 0xF5;
			}
			else
			{
				position_data[1] = 0xF6;
			}
			position_data[2] = 0x00;
			position_data[3] = 0x00;
			position_data[4] = (char)0;
			position_data[5] = (char)100;

			ptr_position_data = position_data;
			 ptr_msp_connection->WriteData(ptr_position_data, 6);
			

			gripper_flag = 1 - gripper_flag;
		}
		if (event == CV_EVENT_MBUTTONDOWN) {

			_machine_vision->state++;


			if (_machine_vision->state >= 10)
			{
				_machine_vision->state = 0;
			}
			//user has clicked middle mouse button
			//enter code here if needed.
		}
		if (event == EVENT_LBUTTONDBLCLK)
		{

			_machine_vision->state--;


			if (_machine_vision->state <= -1)
			{
				_machine_vision->state = 10;
			}



		}

	}


using namespace cv;
using namespace std;
int main()
{
	int state = -1;
	char position_data[6];
	char *ptr_position_data;
	gripper_flag = 0;
	ptr_position_data = (char*)position_data;
	char *portName = "COM4";
	Serial msp_connection = Serial(portName);
	ptr_msp_connection = &msp_connection;
	char Position[6];
	opencv_capture machine_vision = opencv_capture(0);

	setMouseCallback("ROBO GUI", onMouse, &machine_vision);

	while (1)
	{
		machine_vision.machine_vision();

		state = machine_vision.get_state();


		if (state == 5)
		{
		
			ptr_position_data = machine_vision.getPositionChar(0);
			msp_connection.WriteData(ptr_position_data,6);			
		}

		if (state == 4)
		{
			ptr_position_data = machine_vision.home_robot();
			if (machine_vision.homing == 1)
			{
				msp_connection.WriteData(ptr_position_data, 6);
			}
		}
		if (state == 6)
		{
			int x, y, z;
			cout << "please enter X coordinates";
			cin >> x;
			cout << "please enter Y coordinates";
			cin >> y;
			cout << "please enter Z coordinates";
			cin >> z;
			
			position_data[0] = (x >> 8);
			position_data[1] = x;
			position_data[2] = (y >> 8);
			position_data[3] = y;
			position_data[4] = (z>>8);
			position_data[5] = z;
			
			ptr_position_data = position_data;

			msp_connection.WriteData(ptr_position_data, 6);
		}

	}
		return 0;
}

