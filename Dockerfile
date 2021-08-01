FROM ps2dev/ps2dev:latest as ps2

FROM alpine:latest

ENV PS2DEV /usr/local/ps2dev

COPY --from=ps2 $PS2DEV $PS2DEV

ENV PS2SDK $PS2DEV/ps2sdk
ENV GSKIT $PS2DEV/gsKit
ENV PATH $PATH:${PS2DEV}/bin:${PS2DEV}/ee/bin:${PS2DEV}/iop/bin:${PS2DEV}/dvp/bin:${PS2SDK}/bin
ENV PLATFORM ps2

RUN apk add make bash patch wget gmp-dev mpfr-dev mpc1-dev cdrkit
RUN rm -rf /var/cache/apk/*

WORKDIR /src

