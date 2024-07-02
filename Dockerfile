FROM ubuntu:latest as builder
RUN apt-get update && \
    apt-get install -y build-essential git cmake make g++ gcc libpq-dev libcurl4-openssl-dev libxml2-dev
WORKDIR /app
COPY . /app
RUN mkdir /app/build
WORKDIR /app/build
RUN cmake ..
RUN make

FROM ubuntu:latest
WORKDIR /app
COPY --from=builder /app/build/app .
CMD ["./SampleApp"]