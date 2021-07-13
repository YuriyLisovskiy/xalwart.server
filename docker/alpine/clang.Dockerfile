ARG alpine_version=3.13

FROM alpine:$alpine_version

ARG GH_ACCESS_TOKEN

ENV APP_HOME /app
ENV APP_USER root
USER $APP_USER

WORKDIR $APP_HOME

RUN echo "ipv6" >> /etc/modules

RUN apk update && apk upgrade

RUN apk add --update --no-cache \
    clang \
    clang-dev \
    alpine-sdk \
    dpkg \
    cmake \
    ccache \
    git \
    valgrind

RUN git clone -q https://$GH_ACCESS_TOKEN@github.com/YuriyLisovskiy/xalwart.base.git /xalwart.base && \
    mkdir -p /xalwart.base/build && \
    cd /xalwart.base && \
    cd /xalwart.base/build && \
    cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Release .. && \
    make && make install && \
    cd / && rm -rf /xalwart.base

RUN ldconfig /etc/ld.so.conf.d

COPY . .

RUN mkdir $APP_HOME/build && \
    cd $APP_HOME/build && \
    cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug ..

CMD cd $APP_HOME/build/ && make
