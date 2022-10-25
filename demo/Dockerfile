FROM debian AS builder

RUN apt-get update && \
    apt-get install -y libgnutls28-dev libcurl4-gnutls-dev cmake gcc git curl

RUN git clone https://github.com/risoflora/libsagui.git && cd libsagui && \
    cmake -DBUILD_TESTING=ON && make all test

RUN git clone https://github.com/hanhsuan/taipower.git && \
    cd taipower/demo && \
    curl -L https://gitlab.com/Meithal/cisson/-/raw/master/cisson.h?inline=false -o cisson.h && \
    cc -I./ -I/libsagui/include -I./inc/ -I../inc/ -L/libsagui/src ../src/taipower.c ./src/comm.c apiserver.c -o apiserver -lsagui

FROM busybox:glibc

COPY --from=builder /taipower/demo/apiserver /apiserver

COPY --from=builder /libsagui/src/libsagui.so.3.4.0 /lib/libsagui.so.3

RUN echo '#!/bin/sh' > /start.sh && \
    echo -e '_term() {\n    kill -TERM "${child}" 2>/dev/null\n} \ntrap _term SIGTERM' >> /start.sh && \
    echo -e '/apiserver $1 & \n child=$! \n wait "${child}"' >> /start.sh && \
    chmod -R 777 /start.sh

ENTRYPOINT ["/start.sh"]
CMD ["/start.sh"]

