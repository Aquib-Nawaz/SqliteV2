# Key Value Store (Database) Project

This project is a Key Value Store implemented in C++ that can be extended to store data in a relational table format. It includes features such as secondary indexing and is currently in progress to support conversion from command and concurrent actions.

## Features

- **Key-Value Storage**: Basic key-value storage functionality.
- **Relational Table Format**: Extendable to store data in a relational table format.
- **Secondary Indexing**: Support for secondary indexes to improve query performance.
- **Concurrent Actions**: Work in progress to support concurrent actions.
- **Command Conversion**: Work in progress to support conversion from command.



## Getting Started

### Prerequisites

- C++ compiler
- CMake

### Building the Project

1. Clone the repository:
    ```sh
    git clone <repository-url>
    cd <repository-directory>
    ```

2. Create a build directory and navigate into it:
    ```sh
    mkdir build
    cd build
    ```

3. Run CMake to configure the project:
    ```sh
    cmake ..
    ```

4. Build the project:
    ```sh
    cmake --build .
    ```

### Running Tests

To run the tests, use the following command in the build directory:
   ```sh
   ctest