# File Manager ALS

## Introduction

File Manager ALS is a simple file manager simulation designed to facilitate various operations on a virtual disk partition. It provides users with functionalities to create, delete, rename, read, modify files, list files, format partitions, and visualize partition details.

## Getting Started

### Prerequisites

- C Compiler (GCC recommended)
- Standard C Libraries

### Installation

1. Clone the repository or download the source code.
2. Compile the main code using a C compiler.
3. Execute the compiled binary, providing the partition name as a command-line argument.

## Usage

Upon launching the program with the specified partition name, users are presented with a menu offering different options:

1. **Create File**: Create a new file in the partition.
2. **Delete File**: Delete a file from the partition.
3. **Rename File**: Rename an existing file in the partition.
4. **Read File**: Read the contents of a file from the partition.
5. **Modify File**: Modify the contents of a file in the partition.
6. **List Files**: List all files currently stored in the partition.
7. **Format Partition**: Format the partition, erasing all files (caution: irreversible).
8. **Visualize Partition**: View details about the partition, including file structure and disk usage.
9. **Exit**: Terminate the program.

## Documentation

### Implementation Details

File Manager ALS implements a simplified version of the FAT file system, focusing on core functionalities such as file creation, deletion, reading, and modification. While the implementation may not cover all aspects of the FAT specification, it provides a basic understanding of file system operations.

[![OS - Too easy](https://img.shields.io/badge/OS-Too_easy-2ea44f)](https://)
