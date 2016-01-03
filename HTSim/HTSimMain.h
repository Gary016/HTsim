#pragma once
/*=======================================================================================
   file HTSimmMain.h
   
   Author:
   Date:
  
  ======================================================================================*/
namespace HTS {
	HTSim_EXT_CLASS void TNM_HelloWorld();    //sample export function Hellwo

	class HTSim_EXT_CLASS TNM_HelloWorldClass {
	public:
		TNM_HelloWorldClass();
		virtual ~TNM_HelloWorldClass();
		void    PrintData();

	protected:
			int   m_data;
	};
}
