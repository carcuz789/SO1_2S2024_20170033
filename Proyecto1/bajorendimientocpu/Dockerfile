FROM ubuntu:latest
RUN apt-get update && apt-get install -y stress
CMD ["stress", "--cpu", "1", "--timeout", "60"]

