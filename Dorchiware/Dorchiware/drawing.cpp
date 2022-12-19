#include "pch.h"
#include "Calcs.h"
extern LPDIRECT3DDEVICE9 pDevice;
ID3DXFont* FontF;

#define w2s calcs->WorldToScreen

struct SD3DVertex
{
	float x, y, z, rhw;
	DWORD color;
};

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color) {
	D3DRECT rect = { x,y,x + w,y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}


void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color, ID3DXLine* LineL) 
{
	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	LineL->SetWidth(thickness);
	LineL->Draw(Line, 2, color);
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

	newPointOffsetFromOrigin.y = pointOffsetFromOrigin.y * cos(radians) - pointOffsetFromOrigin.x * sin(radians);
	newPointOffsetFromOrigin.x = pointOffsetFromOrigin.y * sin(radians) + pointOffsetFromOrigin.x * cos(radians);

	return newPointOffsetFromOrigin + origin;
}


void DrawEspBox3D(playerEnt* player, int thickness, D3DCOLOR color, ID3DXLine* LineL)
{
	Vector3 top = player->getBonePosition(8);
	top.z += 7;

	Vector3 bot = player->getOrigin();
	
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
	bfl.y = bot.y + 15;
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


void DrawFilledBox(int x, int y, int width, int height, D3DCOLOR color)
{
	SD3DVertex pVertex[4] = { { x, y + height, 0.0f, 1.0f, color }, { x, y, 0.0f, 1.0f, color }, { x + width, y + height, 0.0f, 1.0f, color }, { x + width, y, 0.0f, 1.0f, color } };
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(SD3DVertex));
}




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
	font->DrawTextA(x, y, color, text, 0);
}

void slowDrawText(const char* text, float x, float y, int justification, D3DCOLOR color, const char* fontName, int fontSize)
{
	CD3DFont* font = new CD3DFont(fontName, fontSize, 0);

	if (font->InitializeDeviceObjects(pDevice) == S_OK)
		font->RestoreDeviceObjects();

	font->DrawTextA(x, y, color, text, 0);

	font->~CD3DFont();
}