
#include <gif_tags.h>
#include <gs_gp.h>
#include <tamtypes.h>

void draw_wait_finish() {}

qword_t *draw_finish(qword_t *q) {
  PACK_GIFTAG(q, GIF_SET_TAG(1, 1, 0, 0, GIF_FLG_PACKED, 1), GIF_REG_AD);
  q++;
  PACK_GIFTAG(q, 1, GS_REG_FINISH);
  q++;

  return q;
}
