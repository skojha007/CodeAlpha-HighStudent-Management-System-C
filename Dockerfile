FROM gcc:11-bullseye

# Set working directory
WORKDIR /app

# Copy source files
COPY main.c .
COPY README.md .
COPY Makefile .

# Build the application
RUN gcc -Wall -std=c11 -O2 -o student_mgmt main.c

# Create data directory
RUN mkdir -p /app/data

# Set the entry point
ENTRYPOINT ["./student_mgmt"]

# Labels
LABEL maintainer="skojha007"
LABEL version="1.0"
LABEL description="Student Management System in C"
