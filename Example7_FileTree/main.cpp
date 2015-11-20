#include "../juwnd.h"
#pragma usexpstyle
//用来接收文件信息的回调函数。
void __stdcall onList(ju::ListData* ld){
	//ld->stop = true; 终止搜索 
	ju::Json* json = ld->json;
	if(ld->isFile()){
		//如果是一个文件，保存名称和文件长度信息到 Json
		json->SetPropertyStr(L"name",ld->data->cFileName);
		//使用 FileTime 结构计算文件长度。
		ju::FileTime ft;
		ft.HighValue = ld->data->nFileSizeHigh;
		ft.LowValue = ld->data->nFileSizeLow;
		json->SetPropertyInt64(L"length",ft.Value);
	}else{
		//ld->skip = true; 跳过当前文件夹，只有当 isPreCal 返回true时有效。
	}
}
WINMAIN{
	//声明文件搜索组件
	ju::FileSearch ft;
	//绑定一个回调函数
	ft.OnList.BindStd(&onList);
	//设置当搜索到文件夹时，在开始返回它的子文件之前呼叫回调函数一次，这是默认行为
	ft.SetPreCall(true);
	//设置当搜索到文件夹时，在所以子文件返回后呼叫回调函数一次，默认此值为 false
	ft.SetAfterCall(true);
	//如果上面两项都为 true ，则一个文件夹会返回两次，如果都设为 false，则一次也不返回。
	//设置搜索子文件夹，默认值为 false。
	ft.SetSearchSub(true);

	ju::StringMemList* filter = ft.GetFilter();
	//filter->Add("cpp"); 如果过滤列表不为空，只有指定类型的文件会返回，否则返回所有文件，文件类型过滤不要使用通配符和“.”，直接书写后缀，且不区分大小写。
	//一般来说，直接在回调函数里面过滤自己需要的文件更方便灵活

	//设置一个保存文件信息的 Json
	ju::Json json;
	//搜索当前文件夹
	ft.Search(L".",0,&json);
	//保存搜索结果。
	json.SaveToFile(L"search.js");
}
