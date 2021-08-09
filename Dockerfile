# syntax=docker/dockerfile:1

FROM alpine:3.14 AS build

WORKDIR /app

RUN apk add build-base

COPY . .

RUN make clean release example

FROM alpine:3.14

WORKDIR /app

COPY --from=build /app/bin/ /app/bin/

ENTRYPOINT [ "bin/example.out", "0.0.0.0" ]