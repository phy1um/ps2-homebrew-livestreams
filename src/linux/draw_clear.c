#include <draw.h>

qword_t *draw_clear(qword_t *q, int context, float x, float y, float width, float height, int r, int g, int b)
{

	rect_t rect;

	union{
		float	fvalue;
		u32	ivalue;
	} q0 = {
		1.0f
	};

	rect.v0.x = x;
	rect.v0.y = y;
	rect.v0.z = 0x00000000;

	rect.color.rgbaq = GS_SET_RGBAQ(r,g,b,0x80,q0.ivalue);

	rect.v1.x = x + width - 0.9375f;
	rect.v1.y = y + height - 0.9375f;
	rect.v1.z = 0x00000000;

	PACK_GIFTAG(q, GIF_SET_TAG(2,0,0,0,0,1), GIF_REG_AD);
	q++;
	PACK_GIFTAG(q, GS_SET_PRMODECONT(PRIM_OVERRIDE_ENABLE),GS_REG_PRMODECONT);
	q++;
	PACK_GIFTAG(q, GS_SET_PRMODE(0,0,0,0,0,0,context,1), GS_REG_PRMODE);
	q++;

	q = draw_rect_filled_strips(q, context, &rect);

	PACK_GIFTAG(q, GIF_SET_TAG(1,0,0,0,0,1), GIF_REG_AD);
	q++;
	PACK_GIFTAG(q, GS_SET_PRMODECONT(PRIM_OVERRIDE_DISABLE),GS_REG_PRMODECONT);
	q++;

	return q;

}


