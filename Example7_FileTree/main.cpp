#include "../juwnd.h"
#pragma usexpstyle
//���������ļ���Ϣ�Ļص�������
void __stdcall onList(ju::ListData* ld) {
	//ld->stop = true; ��ֹ���� 
	/*ju::Json* json = ld->json;
	if(ld->isFile()){
	//�����һ���ļ����������ƺ��ļ�������Ϣ�� Json
	json->SetPropertyStr(L"name",ld->data->cFileName);
	//ʹ�� FileTime �ṹ�����ļ����ȡ�
	ju::FileTime ft;
	ft.HighValue = ld->data->nFileSizeHigh;
	ft.LowValue = ld->data->nFileSizeLow;
	json->SetPropertyInt64(L"length",ft.Value);
	}else{
	//ld->skip = true; ������ǰ�ļ��У�ֻ�е� isPreCal ����trueʱ��Ч��
	}*/
	if(!ld->isFile()) return;
	ju::String src = ld->original;
	ju::FPLinkPath(src, ld->relative);
	ju::String dst = src,title;
	ju::FPLinkPath(src, ld->data->cFileName);
	title = ld->data->cFileName;
	ju::FPToTitle(title);
	ju::FPLinkPath(dst, title);
	ju::FPLinkExt(dst, L"php");
	ju::LogfD(L"%s", ld->data->cFileName);
	ju::FileSystem fs;
	fs.Initialize(src);
	fs.Move(dst);
}
WINMAIN{
	ju::LogShowWindow();
	//�����ļ��������
	ju::FileSearch ft;
	//��һ���ص�����
	ft.OnList.BindStd(&onList);
	//���õ��������ļ���ʱ���ڿ�ʼ�����������ļ�֮ǰ���лص�����һ�Σ�����Ĭ����Ϊ
	ft.SetPreCall(true);
	//���õ��������ļ���ʱ�����������ļ����غ���лص�����һ�Σ�Ĭ�ϴ�ֵΪ false
	//ft.SetAfterCall(true);
	//����������Ϊ true ����һ���ļ��л᷵�����Σ��������Ϊ false����һ��Ҳ�����ء�
	//�����������ļ��У�Ĭ��ֵΪ false��
	ft.SetSearchSub(true);

	ju::StringMemList* filter = ft.GetFilter();
	filter->Add(L"html");
	ft.SetFilterType(1);
	//��������б�Ϊ�գ�ֻ��ָ�����͵��ļ��᷵�أ����򷵻������ļ����ļ����͹��˲�Ҫʹ��ͨ����͡�.����ֱ����д��׺���Ҳ����ִ�Сд��
	//һ����˵��ֱ���ڻص�������������Լ���Ҫ���ļ����������

	//����һ�������ļ���Ϣ�� Json
	ju::Json json;
	//������ǰ�ļ���
	//ft.Search(L".",0,&json);
	ft.Search(L"D:\\Develop\\wamp\\www\\jsuser\\3\\editor");
	//�������������
	//json.SaveToFile(L"search.js");
	ju::MsgLoop::Start();
}
