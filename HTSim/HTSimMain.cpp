#include "stdafx.h"
#include "HTSimMain.h"

namespace HTS {
	void TNM_HelloWorld()
	{
		std::cout << "Hello world!" << std::endl;
	}

	TNM_HelloWorldClass::TNM_HelloWorldClass()
	{
		m_data = 1;
	}
	TNM_HelloWorldClass::~TNM_HelloWorldClass()
	{
	}
	void TNM_HelloWorldClass::PrintData()
	{
		std::cout << "Hello world, your data is = " << m_data << std::endl;
	}
}
