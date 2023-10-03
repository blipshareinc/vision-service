FROM --platform=linux/amd64 blipshare/vcpkg:latest

RUN apk update

WORKDIR /workspace

RUN rm -rf ./build

