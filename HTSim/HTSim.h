#ifndef HTSim_TOOLKIT_H
#define HTSim_TOOLKIT_H
//#ifdef _MANAGED
//#pragma managed(push, off)
//#endif
//#pragma unmanaged
#ifndef HTSimDLL_H
	#define HTSimDLL_H

	#ifndef HTSim_EXT_CLASS 
		#ifdef _HTSim_DLL
			#define HTSim_EXT_CLASS  _declspec(dllexport)
		#else
			#define HTSim_EXT_CLASS  _declspec(dllimport)
		#endif
	#endif
	#if defined(_M_IX86)
		#ifndef _SHUTHTSimMATLAB
			#define _ENABLE_MATLAB_ENGINE
		#endif
	#endif
	   
	#ifndef _HTSim_DLL
	#ifndef _HTSim_DLL_LOADED
	#define _HTSim_DLL_LOADED
		#if defined(_M_IX86)
			#ifdef _DEBUG
				#pragma message("     _Adding library: HTSimd.lib: debug, Win32" ) 
				#pragma comment(lib, "HTSimd.lib")
			#else
				#pragma message("     _Adding library: HTSim_win32.lib: release, Win32" ) 
				#pragma comment(lib, "HTSim_win32.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma message("     _Adding library: HTSimd64.lib: debug, Win64" ) 
				#pragma comment(lib, "HTSimd64.lib")
			#else
				#pragma message("     _Adding library: HTSim_x64.lib: release, Win64" ) 
				#pragma comment(lib, "HTSim_x64.lib")
			#endif
		#endif
	#endif
	#endif
#endif
#include "HTSimMain.h"
#endif