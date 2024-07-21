
#ifndef P2G_DRWA_VU_H
#define P2G_DRWA_VU_H

// TODO: move to some other folder
#define VIF_CODE_NOP            0x0
#define VIF_CODE_DIRECT         0x50
#define VIF_CODE_MPG            0b1001010
#define VIF_CODE_MSCAL          0b0010100

#define VIF_CODE_NO_STALL 0x0
#define VIF_CODE_STALL 0x1

#define TARGET_VU0 1
#define TARGET_VU1 2

int draw_vu_upload_program(void *buf, size_t buf_size, int vu_uprog_addr, int vu_target);
int draw_vu_call_program(int vu_uprog_addr);

#endif
