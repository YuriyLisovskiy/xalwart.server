ARG ALPINE_VERSION=3.13

FROM alpine:$ALPINE_VERSION

ARG GH_ACCESS_TOKEN

ENV APP_HOME /app
ENV APP_USER root
USER $APP_USER

WORKDIR $APP_HOME

RUN echo "ipv6" >> /etc/modules

RUN apk update && apk upgrade

RUN apk add --update --no-cache \
    gcc \
    alpine-sdk \
    dpkg \
    cmake \
    ccache \
    git \
    valgrind

RUN git clone -q https://github.com/google/googletest.git /googletest && \
    mkdir -p /googletest/build && \
    cd /googletest/build && \
    cmake -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc .. && \
    make && make install && \
    cd / && rm -rf /googletest

RUN git clone -q https://$GH_ACCESS_TOKEN@github.com/YuriyLisovskiy/xalwart.core.git /xalwart.core && \
    mkdir -p /xalwart.core/build && \
    cd /xalwart.core && \
    git checkout dev && \
    cd /xalwart.core/build && \
    cmake -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE=Release .. && \
    make && make install && \
    cd / && rm -rf /xalwart.core

RUN ldconfig /etc/ld.so.conf.d

COPY . .

RUN mkdir $APP_HOME/build && \
    cd $APP_HOME/build && \
    cmake -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE=Debug ..

CMD cd $APP_HOME/build/ && make