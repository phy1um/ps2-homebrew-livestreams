
.set GIF_PACKET_OFFSET, 4
.set VERTEX_OFFSET, GIF_PACKET_OFFSET+3
.vu
.align 4
         NOP                                                        LQ VF03, 0(VI00)
         NOP                                                        IADDI VI01, VI00, VERTEX_OFFSET
         NOP                                                        IADDI VI02, VI00, 10
         NOP                                                        LQ VF01, 1(VI01)               
         loop:
         MUL VF01, VF01, VF03                                       NOP
         FTOI4 VF01, VF02                                           NOP
         NOP                                                        SQ VF02, 1(VI01)
         NOP                                                        LQ VF01, 3(VI01)               
         NOP                                                        IADDI VI01, VI01, 2
         NOP                                                        ISUBIU VI02, VI01, 1
         NOP                                                        IBLEZ VI02, loop
         NOP                                                        NOP
         NOP[T]                                                     NOP
         NOP                                                        IADDIU VI01, VI00, GIF_PACKET_OFFSET
         NOP                                                        xgkick VI01
         NOP                                                        NOP
         NOP[E]                                                     NOP
         NOP                                                        NOP

