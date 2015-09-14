#pragma once
namespace cs{
	//这个函数生成XP风格控件的Manifest文件,是一个和可执行文件同名的Manifest.
	CONCISE_API bool CreateXPStyleManifest();
	//关闭计算机,
	enum PowerMode
	{
		Power_Logoff = 0,//注销
		Power_ReBoot = 1,//重新启动
		Power_ShutDown = 2,//关闭电源.
		Power_Sleep = 3,//休眠
		Power_Suspend = 4//挂起
	};
	CONCISE_API bool SetPower(PowerMode mode,bool force = 0);
	//如果调用线程阻塞，Timer将无法呼叫设定的回调函数。
	class CONCISE_API  Timer : public HandleType<UINT_PTR>
	{
	protected:
		void _TimerProc(HWND hWnd,UINT uMsg,UINT_PTR idEvent,DWORD);
		Function<void,HWND,UINT,UINT_PTR,DWORD> _Timer;
	public:
		//用于周期性接收回调的函数,UINT_PTR是Timer的句柄.
		Function<void,uint,void*> OnTimer;
		
		Timer();
		//对于一个工作状态的Timer,必须先Stop才能重新Start.
		bool Start(int ms);
		//停止Timer
		void Stop();
		~Timer();
	};
	//这是设置屏幕的简化形式,想获取更高级的形式,参考ChangeDisplaySettings的相关文档.
	class CONCISE_API  Screen : public _class
	{
	public:
		//获取当前设备的所有持的模式,重复的呼叫这个函数,并且每次index加1,直到返回值为0.
		bool GetScreen(int index = ENUM_CURRENT_SETTINGS);//ENUM_REGISTRY_SETTINGS(-2)for save in regisry;
		//设置屏幕模式,必须是获取的数值,不是任意数值都支持.test参数测试是否真得支持要设置的模式,而不改变当前显示模式.
		LONG SetScreen(bool test = 0);
		int Width,			//像素宽度.
			Height,			//像素高度.
			Color,			//颜色深度.
			Frequency;		//刷新频率.
	};
	//设置程序开机启动，keyName：名称，可以指定任意一个名称，但是如果重复会覆盖原来的启动程序，默认0是以文件名（不含后缀)为名字。
	//file：开机启动的程序，可以是可执行文件，也可以是其它文件，默认0指调用程序自身。
	//arg：参数。alluser：是否对所有用户，如果为0，则只有当前用户有效。
	CONCISE_API bool SetProgramAutoRun(bool autorun,LPCWSTR keyName = 0,LPCWSTR file = 0,LPCWSTR arg = 0,bool alluser = 1);
	CONCISE_API bool GetProgramAutoRun(LPCWSTR keyName = 0,LPCWSTR file = 0);
	//这个类的方法全部为静态函数
	class CONCISE_API App{
	public:
		//设置应用程序标识名称, 应用程序统一的名称, 方便一些默认设置命名.
		static void SetAppName(LPCWSTR name);
		//获取应用程序标识名称，默认值是可执行文件名.
		static LPCWSTR GetAppName();
		//设置应用程序目录，此函数和 API 函数 SetCurrentDirectory 没有任何关系，GetCurrentDirecory 也不返回这个目录，默认值是可执行文件所在目录.
		static void SetAppDirectory(LPCWSTR folder);
		//获取应用程序目录，此函数和 API 函数 GetCurrentDirecory没有任何关系，默认是可执行文件所在目录。
		static LPCWSTR GetAppDirectory();
	};
}