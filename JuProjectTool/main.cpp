/**
	工程创建工具，生成一个基本的工程模版，此工程在可执行文件的上一级目录
	生成一个工程文件夹，极优库包括 lib 文件必须在工程同级目录。
*/
#include "../juwnd.h"
#pragma usexpstyle

class Main : public ju::Frame{
protected:
	ju::GroupBox	gBox;
	ju::Button		btnOk;
	ju::Edit		editName;
	ju::TopLayout	layout;
	void onCreate(ju::IWnd*){
		gBox.Param->Text = L"填写工程名称";
		gBox.Create(_Handle);
		//把 gBox 绑定到 layout 上，以可以跟随主窗口尺寸变化。
		layout.SetControl(gBox);
		//设置边界，注意：因为 layout 是 TopLayout，SetParam 的第一和第二个参数（length，weight）是无效的。
		//顶级布局对象（TopLayout）一定会随着它绑定的窗口变化。
		layout.SetParam(0,1,10,10,10,10);

		ju::Layout* editLay = layout.Add();
		editName.Param->Text = L"";
		editName.Create(_Handle);
		editLay->SetControl(editName);
		editLay->SetParam(0,1,20,30,20,20);

		ju::Layout* btnLay = layout.Add();
		btnOk.Param->Text = L"创建工程";
		btnOk.Create(_Handle);
		btnLay->SetControl(btnOk);
		btnLay->SetParam(0,1,20,0,20,20);
	}
	void onCommand(WORD id,WORD type,ju::IWnd* wnd){
		if(wnd==&btnOk){//点击了创建按钮
			ju::LocalString name;
			editName.GetText(name);
			if(name.Length()==0){
				alertError(L"需要输入工程名称");
				return;
			}
			if(createProject(name)){
				alertOk(L"创建成功");
				Close();
			}
		}
	}
	void alertError(LPCWSTR msg){
		::MessageBox(_Handle,msg,L"Error",MB_ICONERROR);
	}
	void alertOk(LPCWSTR msg){
		::MessageBox(_Handle,msg,L"Error",MB_ICONINFORMATION);
	}
	bool createProject(LPCWSTR name){
		ju::FileStream fs;
		if(!fs.OpenExist(L"proj_temp.txt")){
			alertError(L"没有找到 proj_temp.txt 文件");
			return false;
		}
		ju::String proj;
		int len = fs.ReadString(proj);
		fs.Close();

		if(!len){
			alertError(L"proj_temp.txt 读取失败");
			return false;
		}
		ju::String rep = L"{proj}";
		int pos1 = proj.Find(rep);
		if(pos1<0){
			alertError(L"proj_temp.txt 格式错误");
			return false;
		}
		int pos2 = proj.Find(rep,pos1+5);
		if(pos2<0){
			alertError(L"proj_temp.txt 格式错误");
			return false;
		}
		proj.Replace(name,pos2,rep.Length());
		proj.Replace(name,pos1,rep.Length());

		ju::String new_proj;
		new_proj.Format(L"../%s",name);
		if(!ju::CreateFolder(new_proj)){
			alertError(L"无法创建工程文件夹");
			return false;
		}
		ju::String proj_file;
		proj_file.Format(L"../%s/%s.vcproj",name,name);
		if(!fs.Create(proj_file,CREATE_NEW)){
			alertError(L"同名工程已经存在，请另外选一个名称或者删除存在的工程文件夹");
			return false;
		}
		fs.WriteString(proj);
		fs.Close();

		proj_file.Format(L"../%s/main.cpp",name);
		if(!fs.Create(proj_file,CREATE_NEW)){
			alertError(L"无法创建 main.cpp，此文件已经存在");
			return false;
		}
		LPCWSTR _main = L"\
#include \"../juwnd.h\"\r\n\
#pragma usexpstyle\r\n\
\r\n\
WINMAIN{\r\n\
	ju::Frame main;\r\n\
	main.Param->Text = \"%s\";\r\n\
	main.Create();\r\n\
	ju::MsgLoop::Start();\r\n\
}\r\n";
		ju::String main;
		main.Format(_main,name);
		fs.WriteString(main);
		return true;
	}
public:
	Main(){
		Param->Text = L"jucpp 工程创建工具";		//窗口标题
		ju::SetWndRememberPos(this,L"main_wnd");	//设置自动保存窗口位置
		layout.SetContainer(this);					//设置此窗口的布局对象
		ju::Font* font = ju::GetDefaultFont();		//修改缺省字体
		font->SetSize(20);
		font->Create();

		OnCreate.Add(this,&Main::onCreate);			//设置 WM_CREATE 回调
		OnCommand.Add(this,&Main::onCommand);		//设置 WM_COMMAND 回调
	}
};
WINMAIN{
	Main main;
	main.Create();
	ju::MsgLoop::Start();
};