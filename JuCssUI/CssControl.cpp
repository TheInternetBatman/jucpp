#include "../juwnd.h"
#include "CssControl.h"
namespace ju {
	void CssControl::onDraw(ju::DC* dc, ju::View*) {
		dc->SetDCBrushColor(background);
		Arc(*dc, 0, 0, 100, 0, 100, 100, 0, 100);
		//FillRgn(*dc, hrgn, dc->GetDCBrush());
	}
}