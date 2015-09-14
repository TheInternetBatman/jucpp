#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0700	// Change this to the appropriate value to target other versions of IE.
#endif

#ifdef MFC		//如果是MFC程序，需要定义MFC宏
#include <afx.h>
#include <afxwin.h>         // MFC 核心组件和标准组件
#else
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#endif

#define ALERT_FASTMEM_FAILED

#if defined(_UNICODE)||defined(UNICODE)
#else
#error 工程必须设置为 Unicode 字符。
#endif
#ifdef JUBASE_EXPORTS
//库工程
#if defined(JUBASE_STATIC)
//静态库不需要导出符号
#define JUBASE_API	
#define JUBASETL_API
#else
//动态库需要导出符号
#define JUBASE_API __declspec(dllexport)
#define JUBASETL_API __declspec(dllexport)
#endif
#else
//引用这个库的工程，动态库配置已经删除，只支持静态库
#ifdef USE_DYNAMIC_CONCISE
#define JUBASE_API __declspec(dllimport)		//常规对象导出符号
#else
#define JUBASE_API __declspec(dllexport)		//常规对象导出符号
#endif
#define JUBASETL_API __declspec(dllexport)			//模版对象导出符号, 模版类不能使用导入符号.
#endif

//根据安装目录来更改路径.
#ifndef JUBASE_EXPORTS
#ifdef _DEBUG
#pragma comment(lib,"jubaseD.lib")
#else
#pragma comment(lib,"jubase.lib")
#endif
#endif
