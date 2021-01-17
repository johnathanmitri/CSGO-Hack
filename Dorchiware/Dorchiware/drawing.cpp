#include "pch.h"
#include "Calcs.h"
extern LPDIRECT3DDEVICE9 pDevice;
ID3DXFont* FontF;

#define w2s calcs->WorldToScreen

//#include "hack.h"
// filled rectangle

struct SD3DVertex
{
	float x, y, z, rhw;
	DWORD color;
};

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color) {
	D3DRECT rect = { x,y,x + w,y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

/*void DrawFilledRect2()
{
	pDevice->DrawPrimitive(D)

}*/

void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color, ID3DXLine* LineL) 
{
	//ID3DXLine* shit;
	//D3DXCreateLine(pDevice, &shit);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	LineL->SetWidth(thickness);
	LineL->Draw(Line, 2, color);
	//shit->Release();
}

void DrawLine(Vector2 first, Vector2 second, int thickness, D3DCOLOR color, ID3DXLine* LineL)
{
	DrawLine(first.x, first.y, second.x, second.y, thickness, color, LineL);
}


void DrawEspBox(Vector2 bottom, Vector2 top, int thickness, D3DCOLOR color, ID3DXLine* LineL) 
{
	int height = top.y - bottom.y;

	int ratio = height/5;

	Vector2 topLeft = { top.x - ratio, top.y };
	Vector2 bottomRight = { bottom.x + ratio, bottom.y };
	Vector2 topRight = { top.x + ratio, top.y };
	Vector2 bottomLeft = { bottom.x - ratio, bottom.y };

	DrawLine(topLeft, topRight, thickness, color, LineL);
	DrawLine(topRight, bottomRight, thickness, color, LineL);
	DrawLine(bottomRight, bottomLeft, thickness, color, LineL);
	DrawLine(bottomLeft, topLeft, thickness, color, LineL);

}

Vector3 rotateCWAboutPoint(Vector3 origin, Vector3 toRotate, float radians)
{
	Vector3 pointOffsetFromOrigin = toRotate - origin;

	Vector3 newPointOffsetFromOrigin;

	newPointOffsetFromOrigin.z = 0;

	//y' = y*cos(a) - x*sin(a);
	//x' = y*sin(a) + x*cos(a);
	newPointOffsetFromOrigin.y = pointOffsetFromOrigin.y * cos(radians) - pointOffsetFromOrigin.x * sin(radians);
	newPointOffsetFromOrigin.x = pointOffsetFromOrigin.y * sin(radians) + pointOffsetFromOrigin.x * cos(radians);

	return newPointOffsetFromOrigin + origin;
}


void DrawEspBox3D(playerEnt* player, int thickness, D3DCOLOR color, ID3DXLine* LineL)
{
	Vector3 top = player->getBonePosition(8);
	top.z += 7;

	Vector3 bot = player->getOrigin();
	//Vector3 mid = bot;
	//mid.z += 50;

	
	//Vector2AIM aimAngle = player->getViewAngles();
	float aimAngle = player->getViewAngles().x;   //aim angle in radians

	float angleOfRotationCWRads = toRad((-90) - aimAngle); 
	/*Vector3 arrowOffset;
	arrowOffset.y = sin(aimAngleRads) * 30;
	arrowOffset.x = cos(aimAngleRads) * 30;
	arrowOffset.z = 0;
	*/

	/*Vector3 arrowTip = mid;
	arrowTip.y -= 30;       
	arrowTip = rotateCWAboutPoint(mid, arrowTip, angleOfRotationCWRads);

	/*Vector3 boxCornerOffset;
	boxCornerOffset.y = sin(aimAngleRads) * 15;
	boxCornerOffset.x = cos(aimAngleRads) * 15;
	*/

	//Vector3 arrowTip = mid + arrowOffset;

	Vector3 tfl, tfr, tbr, tbl;  //top coordinates
	Vector3 bfl, bfr, bbr, bbl;  //bottom coordinates
	
	//top points
	tfl.x = top.x - 15;
	tfl.y = top.y + 15;	
	tfl.z = top.z;

	tfr.x = top.x + 15;
	tfr.y = top.y + 15;
	tfr.z = top.z;

	tbr.x = top.x + 15;
	tbr.y = top.y - 15;
	tbr.z = top.z;

	tbl.x = top.x - 15;
	tbl.y = top.y - 15;
	tbl.z = top.z;

	//bottom ones now
	bfl.x = bot.x - 15;
	bfl.y = bot.y + 10;
	bfl.z = bot.z;

	bfr.x = bot.x + 15;
	bfr.y = bot.y + 15;
	bfr.z = bot.z;

	bbr.x = bot.x + 15;
	bbr.y = bot.y - 15;
	bbr.z = bot.z;

	bbl.x = bot.x - 15;
	bbl.y = bot.y - 15;
	bbl.z = bot.z;

	tfl = rotateCWAboutPoint(top, tfl, angleOfRotationCWRads);
	tfr = rotateCWAboutPoint(top, tfr, angleOfRotationCWRads);
	tbl = rotateCWAboutPoint(top, tbl, angleOfRotationCWRads);
	tbr = rotateCWAboutPoint(top, tbr, angleOfRotationCWRads);

	bfl = rotateCWAboutPoint(bot, bfl, angleOfRotationCWRads);
	bfr = rotateCWAboutPoint(bot, bfr, angleOfRotationCWRads);
	bbl = rotateCWAboutPoint(bot, bbl, angleOfRotationCWRads);
	bbr = rotateCWAboutPoint(bot, bbr, angleOfRotationCWRads);


	//world to screen
	Vector2 tflS, tfrS, tbrS, tblS;  //top SCREEN coordinates
	Vector2 bflS, bfrS, bbrS, bblS;  //bottom SCREEN coordinates

	//Vector2 midS, arrowTipS;

	if (w2s(tfl, tflS) &&
		w2s(tfr, tfrS) &&
		w2s(tbr, tbrS) &&
		w2s(tbl, tblS) &&
		w2s(bfl, bflS) &&
		w2s(bfr, bfrS) &&
		w2s(bbr, bbrS) &&
		w2s(bbl, bblS))  //IF THEY ARE ALL ON SCREEN
	{

		//w2s(mid, midS);           //direction arrow - doesnt really matter if its on screen
		//w2s(arrowTip, arrowTipS);


		D3DCOLOR frontColor = D3DCOLOR_ARGB(255, 0, 0, 0);

		DrawLine(tflS, tfrS, thickness, color, LineL);   //top square
		DrawLine(tfrS, tbrS, thickness, color, LineL);
		DrawLine(tbrS, tblS, thickness, frontColor, LineL);
		DrawLine(tblS, tflS, thickness, color, LineL);

		DrawLine(bflS, bfrS, thickness, color, LineL);   //bottom square
		DrawLine(bfrS, bbrS, thickness, color, LineL);
		DrawLine(bbrS, bblS, thickness, frontColor, LineL);
		DrawLine(bblS, bflS, thickness, color, LineL);

		DrawLine(tflS, bflS, thickness, color, LineL);   //connecting vertical lines
		DrawLine(tfrS, bfrS, thickness, color, LineL);
		DrawLine(tbrS, bbrS, thickness, frontColor, LineL);
		DrawLine(tblS, bblS, thickness, frontColor, LineL);

		//draw arrow from head
		//DrawLine(midS, arrowTipS, thickness, color, LineL);
	}
}


/*
void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color)
{
	D3DPRIMITIVETYPE type = D3DPT_POINTLIST;
	

}*/

void DrawFilledBox(int x, int y, int width, int height, D3DCOLOR color)
{
	SD3DVertex pVertex[4] = { { x, y + height, 0.0f, 1.0f, color }, { x, y, 0.0f, 1.0f, color }, { x + width, y + height, 0.0f, 1.0f, color }, { x + width, y, 0.0f, 1.0f, color } };
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(SD3DVertex));
	
}



/*void DrawLine(int x, int y, int x2, int y2, int unusedThickness, D3DCOLOR color) {
	SD3DVertex pVertex[2] = { { x, y, 0.0f, 1.0f, color }, { x2, y2, 0.0f, 1.0f, color } };
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &pVertex, sizeof(SD3DVertex));
}*/

/*void DrawLine(Vector2 src, Vector2 dst, int thickness, D3DCOLOR color)
{
	DrawLine(src.x, src.y, dst.x, dst.y, thickness, color);
}*/
/*
void DrawEspBox2D(Vector2 top, Vector2 bot, int thickness, D3DCOLOR color) {
	int height = abs(top.y - bot.y);
	Vector2 tl, tr;
	tl.x = top.x - height / 4;
	tr.x = top.x + height / 4;
	tl.y = tr.y = top.y;

	Vector2 bl, br;
	bl.x = bot.x - height / 4;
	br.x = bot.x + height / 4;
	bl.y = br.y = bot.y;

	DrawLineFAST(tl, tr, thickness, color);
	DrawLineFAST(bl, br, thickness, color);
	DrawLineFAST(tl, bl, thickness, color);
	DrawLineFAST(tr, br, thickness, color);
}

void DrawEspBox3D(Vector3 top, Vector3 bot, float a, int width, int thickness, D3DCOLOR color) 
{
	int height3D = abs(top.z - bot.z);
	Vector3 b1, b2, b3, b4, t1, t2, t3, t4;
	b1.z = b2.z = b3.z = b4.z = bot.z;

	b1.x = bot.x + (cos(toRad(a + 45)) * width);
	b1.y = bot.y + (sin(toRad(a + 45)) * width);

	b2.x = bot.x + (cos(toRad(a + 135)) * width);
	b2.y = bot.y + (sin(toRad(a + 135)) * width);

	b3.x = bot.x + (cos(toRad(a + 225)) * width);
	b3.y = bot.y + (sin(toRad(a + 225)) * width);

	b4.x = bot.x + (cos(toRad(a + 315)) * width);
	b4.y = bot.y + (sin(toRad(a + 315)) * width);

	t1.x = b1.x;
	t1.y = b1.y;
	t1.z = b1.z + height3D;

	t2.x = b2.x;
	t2.y = b2.y;
	t2.z = b2.z + height3D;

	t3.x = b3.x;
	t3.y = b3.y;
	t3.z = b3.z + height3D;

	t4.x = b4.x;
	t4.y = b4.y;
	t4.z = b4.z + height3D;

	Vector2 b1_2, b2_2, b3_2, b4_2, t1_2, t2_2, t3_2, t4_2;
	if (w2s(b1, b1_2) && w2s(b2, b2_2) && w2s(b3, b3_2) && w2s(b4, b4_2) && w2s(t1, t1_2) && w2s(t2, t2_2) && w2s(t3, t3_2) && w2s(t4, t4_2)) {

		// col
		DrawLine(t1_2, b1_2, thickness, color);
		DrawLine(t2_2, b2_2, thickness, color);
		DrawLine(t3_2, b3_2, thickness, color);
		DrawLine(t4_2, b4_2, thickness, color);

		// top base
		DrawLine(t1_2, t2_2, thickness, color);
		DrawLine(t2_2, t3_2, thickness, color);
		DrawLine(t3_2, t4_2, thickness, color);
		DrawLine(t4_2, t1_2, thickness, color);

		// bottom base
		DrawLine(b1_2, b2_2, thickness, color);
		DrawLine(b2_2, b3_2, thickness, color);
		DrawLine(b3_2, b4_2, thickness, color);
		DrawLine(b4_2, b1_2, thickness, color);
	}
}*/

void LDrawText(const char* text, float x, float y, int justification, D3DCOLOR color, ID3DXFont* font) {
	RECT rect;
	
	if (!font)
		D3DXCreateFont(pDevice, 12, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &font);

	SetRect(&rect, x + 1, y + 1, x + 1, y + 1);
	font->DrawTextA(NULL, text, -1, &rect, justification | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));		
	
	SetRect(&rect, x, y, x, y);
	font->DrawTextA(NULL, text, -1, &rect, justification | DT_NOCLIP, color);

	
}

void fastDrawText(const char* text, float x, float y, int justification, D3DCOLOR color, CD3DFont* font)
{
	//CD3DFont* aa = new CD3DFont("Arial", 14, 0);

	//aa->InitDeviceObjects(pDevice);
	

	font->DrawTextA(x, y, color, text, 0);

	//aa->~CD3DFont();

}

void slowDrawText(const char* text, float x, float y, int justification, D3DCOLOR color, const char* fontName, int fontSize)
{
	CD3DFont* font = new CD3DFont(fontName, fontSize, 0);

	if (font->InitializeDeviceObjects(pDevice) == S_OK)
		font->RestoreDeviceObjects();

	font->DrawTextA(x, y, color, text, 0);

	font->~CD3DFont();
}