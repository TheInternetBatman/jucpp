#pragma once
//加速键的4个风格.
#define aclAlt			FALT
#define aclShift		FSHIFT
#define aclControl		FCONTROL
#define aclVirtKey		FVIRTKEY
namespace cs{
/*	1. 如果对应的组合键已经被别的线程(包括其它程序)注册,这个函数返回 0.（现在发现有时会被替换，也就是两个程序注册同一热键，包括ID相同，hWnd为0，
	此时，两个程序都会注册成功，但是接收消息的是最后一个注册的进程，而如果这个进程退出后，原来注册的热键对第一个进程仍然有效。）
	2. 同一线程注册相同的组合键,前一个会被替换.
	3. 同一线线程注册的不同组合键,但ID相同,hWnd 相同,则函数返回0.只要一个不同就可以注册成功.
	4. 组合键实际上可以是4个辅助键同时按（Shift,Alt,Ctrl,Win），Win 键没有任何特殊。
	5. 热键可以是单纯的辅助键（虚拟键传 0 值）或者单纯的虚拟键。
	
	每一个热键都必须对应一个 Hotkey 实例，如果实例被销毁，注册的热键会自动解除.对一个已经 Register() 成功的实例，后续的 Register() 调用直接返回 0.
	WM_HOTKEY是进程相关的，同一进程，不管是那个线程里注册，所有的消息循环里都可以接收到热键按下的消息
	
	回调函数依赖进程的 MsgLoop，如果没有使用MsgLoop，回调不会被调用

	MSDN关于ID说明如下：Specifies the identifier of the hot key. No other hot key in 
	the calling thread should have the same identifier. An application must specify a
	value in the range 0x0000 through 0xBFFF. A shared DLL must specify a value in 
	the range 0xC000 through 0xFFFF (the range returned by the GlobalAddAtom function).
	To avoid conflicts with hot-key identifiers defined by other shared DLLs,
	a DLL should use the GlobalAddAtom function to obtain the hot-key identifier.
	
	Hotkey对 id=0 的情况做了特殊处理，所以如果令 id=0，注册不会成功。
	*/
	class CONCISE_API  Hotkey : public _class
	{
	protected:
		WORD _Identity;
		bool _CallBack(MSG* msg);
	public:
		Function<void,int /*id*/> OnPress;	//这个成员用于设置响应消息函数
		////////////////////////////////////////////
		Hotkey();
		~Hotkey();
		//注册热键.
		bool Register(WORD id,int key,bool shift,bool ctrl = 0,bool alt = 0,bool win = 0);
		//取消热键注册.
		bool UnRegister();
	};
	struct Accel : public _class
	{
		//添加一个加速键,key是虚拟键,一般为键盘对应字母的大写.id消息ID,ctrl是否包含CTRL键,alt是否包含ALT键,shift是否包含
		//SHIFT键,这个函数会自动添加FVIRTKEY风格.不能添加已经存在的组合键(返回false),但是不同组合键可以有相同ID.
		bool Add(short key,short id,bool ctrl,bool alt,bool shit);
		//添加一个组合键.这个函数须手动添加FVIRTKEY风格.
		bool Add(short key,short id,char flag);
		//删除一个已经添加的组合键.
		bool Delete(short key,short id,char flag);
		//删除指定索引的组合键.
		bool Delete(int index);
		//组合键个数
		int Count(){return _AccelList.Count();}
		//获取组合键数组的句柄(用于CreateAcceleratorTable函数的参数).
		inline ACCEL* GetHandle(){return (LPACCEL)_AccelList.GetHandle();}
		//这个类型可以直接做为LPACCEL类型的参数.
		operator ACCEL* (){return (LPACCEL)_AccelList.GetHandle();}
	private:
		List<ACCEL> _AccelList;
	};
	class CONCISE_API  AccelTable : public HandleType<HACCEL>
	{
	public:
		//生成加速键表.这三个加载函数可以动态调用.
		bool Create(LPACCEL pAcl,int count);
		bool Create(Accel& acl);
		//从资源加载.
		bool LoadResource(ResID res);

		//参数是一个从主消息循环得到的调用.
		bool Start();
		//和调用Start时必须是同一个调用.
		bool Stop();
	private:
		bool _CallBack(MSG* msg);
	};
}
