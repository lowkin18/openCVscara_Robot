#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
class object_item
{

private:
	String _name;
public:
	int _H_MIN;
	int _H_MAX;
	int _S_MIN;
	int _S_MAX;
	int _V_MIN;
	int _V_MAX;

	unsigned int _y_pos;
	unsigned int _x_pos;

	Scalar  get_scalar_min();
	Scalar  get_scalar_max();
	object_item();
	~object_item();
	String getName();
	void reset();
	void set_object(int *data);
};

