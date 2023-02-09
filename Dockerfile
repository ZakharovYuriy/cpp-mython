FROM gcc

WORKDIR /application

COPY . .

RUN apt-get update && \
    apt-get install -y \
      cmake \
    && \
    mkdir Release \
    && \
    cmake -E chdir Release/ cmake -G "Unix Makefiles" ../ -DCMAKE_BUILD_TYPE:STRING=Release \
    && \
    cmake --build Release/.

WORKDIR /application/Release

CMD ./mython
