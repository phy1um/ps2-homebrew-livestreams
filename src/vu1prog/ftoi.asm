.vu
.align 4
         NOP                                                        IADDI VI01, VI00, 3
         loop:
         NOP                                                        LQ VF01, 1(VI01)               
         FTOI4 VF01, VF02                                           NOP
         NOP                                                        SQ VF02, 1(VI01)
         NOP                                                        LQ VF01, 3(VI01)               
         NOP                                                        IADDI VI01, VI01, 2
         NOP                                                        ISUBIU VI02, VI01, 35
         NOP[T]                                                     NOP
         NOP                                                        IBLEZ VI02, loop
         NOP                                                        NOP
         NOP                                                        xgkick        VI00  
         NOP[E]                                                     NOP
         NOP                                                        NOP

