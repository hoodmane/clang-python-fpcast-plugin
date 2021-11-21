FROM debian:bullseye-slim
RUN apt-get update \
  && apt-get install -y --no-install-recommends  lsb-release wget software-properties-common gnupg2 cmake \
  && wget https://apt.llvm.org/llvm.sh \
  && chmod +x llvm.sh \
  && ./llvm.sh 13 \
  apt-get install -y --no-install-recommends libclang-13-dev \
  && rm -rf /var/lib/apts/lists/*

WORKDIR /src