
//https://github.com/makerdiary/Espruino

#include "../juwnd.h"
#pragma usexpstyle

WINMAIN{
	LPCSTR str1 = "{\"username\":\"621122100001\",\"password\":\"123987\",\"dwbh\":\"621122100001\",\"dwmc\":\"¤�����Ե�λ\",\"details\":[{\"yangpinbianhao\":\"201504081349114\",\"yangpinmingcheng\":\"�ײ�\",\"shoujiandanwei\":\"�����ֳ���\",\"jainceriqi\":\"2015 - 4 - 8 14:43 : 19\",\"jiancexiangmu\":\"ũҩ����\",\"jiancejieguo\":\"4.8%\",\"jianyanjielun\":\"�ϸ�\",\"jianceren\":\"��ΰ\"}]}";
	LPCSTR str2 = "{\"username\":\"621122100001\",\"password\":\"123987\",\"dwbh\":\"621122100001\",\"dwmc\":\"¤�����Ե�λ\",\"details\":[{\"yangpinbianhao\":\"201504081349114\",\"yangpinmingcheng\":\"�ײ�\",\"shoujiandanwei\":\"�����ֳ���\",\"jianceriqi\":\"2016 - 4 - 8 14:43 : 19\",\"jiancexiangmu\":\"ũҩ����\",\"jiancejieguo\":\"4.8%\",\"jianyanjielun\":\"�ϸ�\",\"jianceren\":\"�Ƶ�\"}]}";
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
