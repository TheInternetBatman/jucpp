#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0700	// Change this to the appropriate value to target other versions of IE.
#endif

#ifdef MFC		//�����MFC������Ҫ����MFC��
#include <afx.h>
#include <afxwin.h>         // MFC ��������ͱ�׼���
#else
#include <winsock2.h>
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <tchar.h>
#include <crtdbg.h>
#endif

#define ALERT_FASTMEM_FAILED

#if defined(_UNICODE)||defined(UNICODE)
#else
#error ���̱�������Ϊ Unicode �ַ���
#endif
#ifdef JUBASE_EXPORTS
//�⹤��
#if defined(JUBASE_STATIC)
//��̬�ⲻ��Ҫ��������
#define JUBASE_API	
#define JUBASETL_API
#else
//��̬����Ҫ��������
#define JUBASE_API __declspec(dllexport)
#define JUBASETL_API __declspec(dllexport)
#endif
#else
//���������Ĺ��̣���̬�������Ѿ�ɾ����ֻ֧�־�̬��
#ifdef USE_DYNAMIC_CONCISE
#define JUBASE_API __declspec(dllimport)		//������󵼳�����
#else
#define JUBASE_API __declspec(dllexport)		//������󵼳�����
#endif
#define JUBASETL_API __declspec(dllexport)			//ģ����󵼳�����, ģ���಻��ʹ�õ������.
#endif

//���ݰ�װĿ¼������·��.
#ifndef JUBASE_EXPORTS
#ifdef _DEBUG
#pragma comment(lib,"jubaseD.lib")
#else
#pragma comment(lib,"jubase.lib")
#endif
#endif
