# Vigenère Encryption Project

## Introduction

This project was a collaborative effort for the Computer Systems course (COSC 171) at Amherst College. Our goal was to implement Vigenère encryption, a method of encrypting alphabetic text by using a simple form of polyalphabetic substitution. This project aimed to deepen our understanding of cryptographic algorithms and their implementation in C.

## Project Description

The Vigenère encryption algorithm uses a keyword to shift the letters of the plaintext. Each letter in the keyword specifies how many positions each corresponding letter in the plaintext is shifted. This process continues cyclically through the keyword. The project involved creating functions for encrypting and decrypting text using this method, providing a practical experience with string manipulation and encryption concepts.

## Key Features

- **Encryption and Decryption**: The program includes functions to both encrypt plaintext messages and decrypt ciphertext back into readable text.
- **Keyword-Based Shifts**: The encryption and decryption processes rely on a keyword provided by the user, allowing for flexible and varied shifts.
- **User Interface**: A simple command-line interface allows users to input plaintext and keywords to see the resulting ciphertext, and vice versa.

## Technologies Used

- **Programming Language**: C
- **Development Environment**: Unix-based systems for compiling and running the program

## Contributors

- Feisal Kiiru(myself)
- Lillian Pentecost(proffesor)

## How to Run

1. Clone the repository: `git clone [repository_url]`
2. Navigate to the project directory: `cd VigenereEncryption`
3. Compile the project: `gcc -o vigenere vigenere.c`
4. Run the program: `./vigenere`

## Usage

To encrypt a message:
```
./vigenere -e "YOURKEYWORD" "Your plaintext message here"
```

To decrypt a message:
```
./vigenere -d "YOURKEYWORD" "Your ciphertext message here"
```
