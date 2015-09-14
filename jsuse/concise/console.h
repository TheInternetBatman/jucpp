namespace cs{
	class CONCISE_API Console : public _class{
	protected:
		HANDLE _hRead,_hWrite;
	public:
		Console();
		bool Show();
		inline bool IsShowed(){
			return _hRead!=0;
		}
		void Close();
		inline void Write(LPCWSTR str){
			DWORD len;
			WriteConsole(_hWrite,str,WcsLength(str),&len,NULL);
		}
		inline void Write(LPCSTR str){
			DWORD len;
			WriteConsoleA(_hWrite,str,MbsByteLen(str),&len,NULL);
		}
		void SetCloseMenuDisable(bool disable);
		//设置文本颜色, 控制台指支持 8(0--7) 种颜色, 也就是上面3个颜色的或组合.
		void SetTextColor(int color);
		void WriteLn(LPCWSTR str);
		//控制台读取总是在回车后发生, 也就是这个函数会阻塞, 知道在控制台按了回车, 
		//如果buf太小, 需要读取的字符多于buffer, 这个函数可以连续调用, 知道返回值的末尾是一个回车和一个换行.
		DWORD ReadLn(String& str);
		inline ~Console(){
			Close();
		}
	};
	CONCISE_API Console* GetConsole();
}