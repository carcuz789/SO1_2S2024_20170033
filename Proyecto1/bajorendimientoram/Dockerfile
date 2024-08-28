FROM ubuntu:latest
RUN apt-get update && apt-get install -y stress
CMD ["stress", "--vm", "1", "--vm-bytes", "128M", "--timeout", "60"]
