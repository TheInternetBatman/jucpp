
//https://github.com/makerdiary/Espruino

#include "../juwnd.h"
#pragma usexpstyle

WINMAIN{
	LPCSTR str1 = "{\"username\":\"621122100001\",\"password\":\"123987\",\"dwbh\":\"621122100001\",\"dwmc\":\"陇西测试单位\",\"details\":[{\"yangpinbianhao\":\"201504081349114\",\"yangpinmingcheng\":\"白菜\",\"shoujiandanwei\":\"人人乐超市\",\"jainceriqi\":\"2015 - 4 - 8 14:43 : 19\",\"jiancexiangmu\":\"农药残留\",\"jiancejieguo\":\"4.8%\",\"jianyanjielun\":\"合格\",\"jianceren\":\"刘伟\"}]}";
	LPCSTR str2 = "{\"username\":\"621122100001\",\"password\":\"123987\",\"dwbh\":\"621122100001\",\"dwmc\":\"陇西测试单位\",\"details\":[{\"yangpinbianhao\":\"201504081349114\",\"yangpinmingcheng\":\"白菜\",\"shoujiandanwei\":\"人人乐超市\",\"jianceriqi\":\"2016 - 4 - 8 14:43 : 19\",\"jiancexiangmu\":\"农药残留\",\"jiancejieguo\":\"4.8%\",\"jianyanjielun\":\"合格\",\"jianceren\":\"黄灯\"}]}";
	int len = strlen(str1);
	int index = 0;
	for(; index < len; index++) {
		if(str1[index] != str2[index]) {
			break;
		}
	}
	return index;
	ju::Frame main;
	main.Param->Text = "Espruino";
	main.Create();
	ju::MsgLoop::Start();
}
