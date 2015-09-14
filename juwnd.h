#pragma once
//动态库配置已经删除
#ifndef JUWND_EXPORTS
	//外部引用
	#ifdef USE_DYNAMIC_CONCISEUI
		#define JUWND_API __declspec(dllimport)
		#define USE_DYNAMIC_BASE	//使用动态base库
	#else
		#define JUWND_API
	#endif
#else
	//本工程导出
	#ifdef JUWND_STATIC
		#define JUWND_API
	#else
		#define USE_DYNAMIC_BASE	//使用动态base库
		#define JUWND_API __declspec(dllexport)
	#endif
#endif
#include "jubase.h"

#include "juwnd/window.h"
#include "juwnd/logwnd.h"
#include "juwnd/usermenu.h"
#include "juwnd/comdlg.h"
#include "juwnd/dlg.h"
#include "juwnd/ictrl.h"
#include "juwnd/ctrl.h"

#include "juwnd/Layout.h"
#include "juwnd/notifyicon.h"
#include "juwnd/wndPos.h"
//根据安装目录来更改路径.
#ifndef CONCISEUI_EXPORTS
	#ifdef _DEBUG
		#pragma comment(lib,"juwndD.lib")
	#else
		#pragma comment(lib,"juwnd.lib")
	#endif
#endif