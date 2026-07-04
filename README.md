# File-Compressor-using-Huffman
A lossless file compression utility built in C++ utilizing Huffman Coding, Min-Heaps, and bitwise operations

Technical Architecture
1.Language: C++ (Standard Template Library)
2.Core Algorithms: Huffman Coding (Variable-Length Prefix Codes), Greedy Algorithm, Depth-First Search (DFS)
3.Data Structures: Custom Min-Heap, Binary Trees, Hash Maps
4.System Concepts: Bitwise Manipulation, Binary File I/O, Memory Management

How It Works
1. Frequency Analysis: Scans the input file to calculate the exact frequency of every character.
2. Min-Heap Construction: Builds a custom priority queue to isolate the least frequent characters.
3. Tree Generation: Merges nodes bottom-up to construct a Huffman Binary Tree.
4. Bitwise Packing: Generates unique, optimal binary codes for each character and uses bitwise shifting to tightly pack the new bits into raw bytes.
5. Standalone Decompression: Injects the frequency map into the binary file header, allowing the decompression algorithm to perfectly reconstruct the original file even if the original tree is wiped from memory.
