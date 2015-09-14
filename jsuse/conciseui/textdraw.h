#pragma once
namespace cs{
	struct CONCISEUI_API TextDrawParam : public _struct{
		enum {
			format_left = 0,
			format_right = 1,
			format_center = 2,
			format_middle = 4
		};
		UINT	Format; 
		float	LineHeight,
			ParaHeight;
		bool	SingleLine;
		Font	Font;
		Color	Color;
		float	Indent;
		TextDrawParam(){
			Format = format_left;
			LineHeight = 1.4f;
			ParaHeight = 1.8f;
			SingleLine = true;
			Font.LogFont()->lfHeight = 16;
			Color = 0;
			Indent = 0;
		}
	};
	class CONCISEUI_API TextDraw : public _class{
	protected:
		StringList	_lines;
		View*		_view;
		int _w0,_w1,_fontHeight,_drawHeight;
		inline int _getCharWidth(wchar_t ch){
			if(ch<256) return _w0;
			else return _w1;
		}
		void _initCharWidth(DC* dc){
			GetCharWidth32(*dc,'A','A',&_w0);
			GetCharWidth32(*dc,'中','中',&_w1);
		}
		void _drawSingleLine(DC* dc,String& line,int maxW,int lineH,int& x,int& y){
			int width = 0,j;
			for(j=0;j<(int)line.Length();j++){
				width += _getCharWidth(line[j]);
				if(width>maxW){
					width -= _getCharWidth(line[j]);
					break;
				}
			}
			::TextOut(*dc,x,y,line,j);
			y += lineH;
		}
		int _drawMultiLine(DC* dc,String& line,int maxW,int lineH,int height,int _indent,
			int fh,Twin16 sz,Twin16 pos,int& x,int& y,bool calcHeight){
				int j = 0,start = 0,width = _indent,h = 0;
				while(j<(int)line.Length()){
					if((y+fh>sz.y+pos.y)&&!calcHeight) break;
					int count = 0;
					for(;j<(int)line.Length();j++){
						width += _getCharWidth(line[j]);
						if(width>maxW){
							width -= _getCharWidth(line[j]);
							break;
						}
						count++;
					}
					if(count==0){
						count = 1;
						j++;
					}
					if(!calcHeight&&y>=pos.y)::TextOut(*dc,x,y,line+start,count);
					if(h!=0) h += height;
					else h += fh;
					start = j;
					y += height;
					width = 0;
					x = pos.x;
				}
				y += lineH-height;
				return h;
		}
		void _onDraw(DC* dc,View* view){
			if(!Layout) return;
			dc->SetTextColor(Param.Color);
			dc->SetBkTransparent();
			if(Param.Format&TextDrawParam::format_center) dc->SetTextAlign(TA_CENTER);
			else if(Param.Format&TextDrawParam::format_right) dc->SetTextAlign(TA_RIGHT);
			else dc->SetTextAlign(TA_LEFT);
			Param.Font.Create();
			Param.Font.Select(*dc);
			int fh = Param.Font.LogFont()->lfHeight;
			if(fh!=_fontHeight){
				_initCharWidth(dc);
				_fontHeight = fh;
			}
			Twin16 sz = Layout->GetSize();
			Twin16 pos = Layout->GetLocation();
			Twin scr = _view->Scr();
			pos.x -= scr.x;
			pos.y -= scr.y;
			int maxW = sz.x;
			int _indent = (int)(Param.Indent*_w1);
			int x = _indent + pos.x,y = pos.y;
			int height = (int)(Param.LineHeight*fh);
			int lineH = (int)(Param.ParaHeight*fh);
			if(view==0){
				_drawHeight = 0;
			}
			for(uint i=0;i<_lines.Count();i++){
				if((y+fh>sz.y+pos.y)&&view!=0) break;
				String& line = _lines[i];
				x = _indent + pos.x;
				if(Param.SingleLine){
					if(view!=0) _drawSingleLine(dc,line,maxW,lineH,x,y);
					else{
						if(_drawHeight==0) _drawHeight += fh;
						else _drawHeight += lineH;
					}
				}else{
					int h = _drawMultiLine(dc,line,maxW,lineH,height,_indent,fh,sz,pos,x,y,view==0);
					if(view==0){
						if(_drawHeight==0) _drawHeight += h;
						else _drawHeight += h + lineH - fh;
					}
				}
			}
			Param.Font.Select(*dc);
			Param.Font.Delete();
		}
	public:
		TextDrawParam Param;
		Layout* Layout;
		TextDraw():_w0(0),_w1(0),_view(0),_fontHeight(0),Layout(0){
			Param.Font.SetSize(16);
		}
		void SetContainer(View* view,bool detach = 0){
			if(detach){
				_view = 0;
				view->OnDraw.Delete(this,&TextDraw::_onDraw);
			}else{
				_view = view;
				view->OnDraw.Add(this,&TextDraw::_onDraw);
			}
		}
		int AddLine(LPCWSTR line,int index = -1){
			return _lines.Add(line,index);
		}
		int SetLine(LPCWSTR line,int index){
			if((uint)index>=_lines.Count()){
				return _lines.Add(line);
			}else{
				_lines[index] = line;
				return index;
			}
		}
		//获取文本绘制所需的高度
		int GetDrawHeight(){
			DC dc;
			dc.CreateCompatible();
			_onDraw(&dc,0);
			dc.DeleteDC();
			return _drawHeight;
		}
		int GetLineCount(){
			return _lines.Count();
		}
		int DelLine(int index,int count = 1){
			return _lines.Delete(index,count);
		}
		void GetLine(String& line,int index){
			line = _lines.Element(index);
		}
		void GetText(String& text,LPCWSTR split = L"\r\n"){
			text = L"";
			for(uint i=0;i<_lines.Count();i++){
				String& line = _lines[i];
				if(!text.IsNull()) text += split;
				text += line;
			}
		}
		void SetText(LPCWSTR text,LPCWSTR split = L"\r\n"){
			_lines.Clear();
			_lines.SplitString(text,split);
		}
	};
}