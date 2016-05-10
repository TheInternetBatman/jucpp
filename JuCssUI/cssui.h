#pragma once
#include "../juwnd.h"
namespace ju {
	/* �� CSS ���ֵĲ��
	�ӿؼ�����Ӱ�츸�ؼ�����Ϊ�����Ǹ��ؼ��ĵ�����Ӱ���ӿؼ���*/
	//DivParam
	struct JUWND_API DivParam : public _struct {
		enum {
			VERTICAL = 0,		//��ֱ����: ��Ԫ�ش�ֱ��������, ˮƽ�Զ�.
			HORIZONTAL = 1		//ˮƽ����: ��Ԫ��ˮƽ��������, ��ֱ�Զ�.
		};
		Rect		Margin,Padding;
		float		Weight;
		int			Length;
		int			Style;

		void SetVertical();
		void SetHorizontal();
		inline bool IsHorizontal() { return (Style&HORIZONTAL) == HORIZONTAL; }
		inline bool IsVertical() { return (Style&HORIZONTAL) == 0; }
		void Reset();
		void SetDimen(int length, float weight);
		void SetMargin(int left, int top, int right, int bottom);
		void SetParam(int length, float weight = 1, int left = 0, int top = 0, int right = 0, int bottom = 0, bool hor = false);
		DivParam();
	};
	/**����ϵͳ�������£�
	1. �����������ǳ������ڣ����������߽磬����weight��length���ԡ���Ϊ����Divֻ����һ����
	2. һ�����ֵĶ���Ӳ��ְ����������Horizontal����Vertical������ˮƽ����ֱ˳�����С�
	3. �Ӳ��ֵĴ�С�����¹���ˮƽ���֣������Ӳ��ֵĸ���һ���ģ���ֱ���֣������Ӳ��ֵ�
	����һ���ģ����Ǽ��ϱ߽��Ⱥ���������֡�ˮƽ����ʱ�Ӳ��ֵĿ�ȷ���ʹ�ֱ�����Ӳ��ֵ�
	�߶ȷ��䣬����ѭ�ȷ���ÿ���Ӳ���ָ�����Ⱥͱ߽�ֵ��ʣ��ߴ簴Ȩ�أ�weight�������ÿ����
	���ֵ�ԭ��ע�⣺��ʣ��ߴ�Ϊ���������������Ȩ����Сÿ���Ӳ��ֵĳߴ硣
	4. �����Ӳ���Ȩ���ܺ�Ϊ 0 �������С��0.00001�����Ӳ��ֳߴ簴ָ�����ȷ��䣬�����Ӹ����ֵĳߴ磬�����������
	���ܵ����Ӳ��ֳ��������ֵ����Ρ�

	��ǰ���ֳߴ�������ȡ����ͬ���Ĳ��ֵĲ�����Param���͸����ֵĳߴ磬�������������Ӳ��ֵ�
	Ӱ�죬Ҳ�����Ӳ��ֵ��κβ�������Ӱ�츸���ֵĳߴ硣

	���磺������ʵ�ʷֵĵĳ���Ϊ110����2���Ӳ��֣��Ӳ��ֵĳ��ȶ���10��Ȩ�طֱ�Ϊ1.0��2.0��
	��ô��110 - ��10 + 10�� = 90�����90���ǰ�Ȩ�ط���Ŀɵ��ߴ磬��������Ȩ��1.0�Ĳ��֣���
	��30��Ȩ��2.0�Ĳ��ֵַ�60�����ǵ�ʵ�ʳߴ�ֱ�Ϊ10+30��10+60��������ǵı߽�ֵ������ 0 ��
	��ô��Ҫ��90�����ȥ�߽�ֵ��ʣ�µĳߴ��ȥ���䡣*/
	//Div Object use for View layout,but pure Div object not bind a View object.
	class JUWND_API Div : public TPTree<Div> {
		friend class DivBinder;
	protected:
		Twin16	_size;
		Twin16	_location;

		Twin _calcMinSize(Div* lay, bool parentHor);
		//set realize dimension of this object,and his child will be resize
		void _setRect(int left, int top, int width, int height);
		//return fix size
		int _setChildHor(int left, int top, int width, int height);
		//return fix size
		int _setChildVer(int left, int top, int width, int height);
		//create all control in this object or his child.
		void _createControlTree(HWND parent);
	public:
		String			Name;
		DivParam		Param;

		Div();
		~Div();
		//this will make Div create control (when Refresh call) and delete it when destroy himself.-1 is to make it default value.
		//�����ڲ����ɿؼ�, �⺯�������󶨵Ŀؼ�, ���������Ѿ����ɵĿؼ�, �¿ؼ���Ҫ����refresh������.
		void SetControl(LPCWSTR className, WORD id, LPCWSTR text, DWORD style = -1, DWORD exstyle = -1);
		//set a created window as bind window,this will destroy inner control of _createWnd.
		void SetControl(HWND ctrl);
		//get realize location of object
		Twin16 GetLocation() { return _location; }
		//get realize size of object
		Twin16 GetSize() { return _size; }
		//a point whether in this Div.
		bool PtInDiv(int x, int y);
		//clear all child and set object to default state
		void Reset();
		//convert to a json object.
		void ToJson(Json* json);
		//from a xml to create Div
		bool FromJson(Json* json);
		//when Param is changed or SetControl is called,call this function to refresh it and if parent != 0,will create control.
		virtual void Refresh(HWND parent);
		//save to a file normaly a .Div file
		bool Save(LPCWSTR file, DWORD code = CP_UTF8);
		//load from a .Div file.
		bool Load(LPCWSTR file, DWORD code = CP_UTF8);
		//load from resource .
		bool Load(ResID res, ResID rt = L"RT_LAYOUT", DWORD code = CP_UTF8);
		//return a layout that name is required.
		Div* GetElementByName(LPCWSTR name);
		//�ƶ���ǰ Div������ƶ��ı����ǵ���������һ������һ��ͬ�� Div �� Length ���ԣ��������һ�� Div û��ͬ������һ������һ�� Div�����޷�����λ�õġ�
		//����λ�õķ��ȣ�����һ������һ��ͬ�� Div �� Length ���Բ�С�� 0 ��
		//����ʵ�ʵ�����λ�÷��ȡ�
		int Move(int pix);
		//�������� DivParam ����
		void SetParam(int length, float weight = 1, int left = 0, int top = 0, int right = 0, int bottom = 0, bool hor = false);
	};
	//TopDiv ����հ󶨴��ڵ� WM_SIZE ��Ϣ���Ӷ����� Div �Ĳ��ֽ��е�����TopDiv �Ƕ��� Div �������������� Div ���� Div��
	class JUWND_API TopDiv : public Div {
	protected:
		IWnd* _container;
		void _onSize(Message* msg, IWnd*);
	public:
		TopDiv();
		//��ȡ������������³ߴ硣
		Twin GetMinSize() {
			Param.Length = 0;
			return _calcMinSize(this, Param.IsHorizontal());
		}
		//this can be set before view is created.if view is a dlg set isdlg = 1 else set isdlg to 0
		void SetContainer(IWnd* view);
		//create all control and calc layout size,allways set parent param is 0,it is not use,this only to make it a override virtual function.
		//call this function in initialize,because no wm_size message was send,so every layout size has not calc.
		virtual void Refresh(HWND parent = 0);
		//clear all child and set object to default state
		void Reset();
	};
	class JUWND_API DivBinder {
	protected:
		IWnd*	_container;
		bool	_containerIsScrollView;
		void _onSize(Message* msg, IWnd*);
	public:
		Div Lay;
		DivBinder();
		//��ȡ�����������С�ߴ硣
		Twin GetMinSize() {
			Lay.Param.Length = 0;
			return Lay._calcMinSize(&Lay, Lay.Param.IsHorizontal());
		}
		//this can be set before view is created.if view is a dlg set isdlg = 1 else set isdlg to 0
		void Bind(IWnd* view, bool isScrollView = false);
		//create all control and calc layout size,allways set parent param is 0,it is not use,this only to make it a override virtual function.
		//call this function in initialize,because no wm_size message was send,so every layout size has not calc.
		//�������ͨ������Ҫ���ã��������������ڳߴ�仯ʱ��Div ���Զ������С��
		//���� Div �����Զ����ɿؼ��������Զ����������ڵ���С�����ߴ磨����������ScrollView��ʱ�򣩣�����
		//ֻ��WM_SIZE��Ϣ������ʱ����Div�Ĵ�С�Ż���¡��������Div�������仯ʱ�����������仯����Div���ɾ����
		//�������¿ռ䣬��Ҫ����Refresh��ˢ�±仯��ͨ���������������ɾ�͸�������Div����û�������Զ����ɿؼ���
		//�Ͳ���Ҫ���������ڵ���С�����ߴ磬���Բ����� Refresh��
		virtual void Refresh(HWND parent = 0);
		//clear all child and set object to default state
		void Reset();
	};
}
