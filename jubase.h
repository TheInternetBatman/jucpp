//这个文件是外部程序引用库的头文件
#pragma once

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include "jubase/macro.h"
#include "jubase/base.h"
#include "jubase/struct.h"
#include "jubase/mem.h"
#include "jubase/list.h"
#include "jubase/proc.h"
#include "jubase/function.h"
#include "jubase/text.h"
#include "jubase/console.h"
#include "jubase/sys.h"
#include "jubase/debugoutput.h"
#include "jubase/kernel.h"
#include "jubase/json.h"
#include "jubase/io.h"
#include "jubase/filemanage.h"

#include "jubase/WinHttp.h"
#include "jubase/gzip.h"
#include "jubase/xyssl.h"
#include "jubase/ecc.h"
#include "jubase/registry.h"
#include "jubase/msgloop.h"
#include "jubase/user.h"
#include "jubase/res.h"
#include "jubase/gdi.h"
#include "jubase/img.h"
#include "jubase/key.h"
#include "jubase/web.h"

#include "jubase/datatree.h"
#include "jubase/precitimer.h"
//根据安装目录来更改路径.
#ifndef CONCISEUI_EXPORTS
#ifdef _DEBUG
#pragma comment(lib,"jubaseD.lib")
#else
#pragma comment(lib,"jubase.lib")
#endif
#endif