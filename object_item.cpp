#include "object_item.h"



Scalar  object_item::get_scalar_min()
{
	return Scalar(_H_MIN, _S_MIN, _V_MIN);
}

Scalar  object_item::get_scalar_max()
{
	return Scalar(_H_MAX, _S_MAX, _V_MAX);
}

object_item::object_item()
{
	_H_MIN = 1;
	_H_MAX = 255;
	_S_MIN = 1;
	_S_MAX = 255;
	_V_MIN = 1;
	_V_MAX = 255;
}


object_item::~object_item()
{

}

String object_item::getName()
{
	return _name;
}

void object_item::reset()
{
	_H_MIN = 0;
	_S_MIN = 0;
	_V_MIN = 0;
	_H_MAX = 255;
	_S_MAX = 255;
	_V_MAX = 255;
}

void object_item::set_object(int * data)
{
	_H_MIN = data[0];
	_S_MIN = data[2];
	_V_MIN = data[4];
	_H_MAX = data[1];
	_S_MAX = data[3];
	_V_MAX = data[5];
}


