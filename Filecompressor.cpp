#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
using namespace std;

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char character, int frequency) {
        ch = character;
        freq = frequency;
        left = nullptr;
        right = nullptr;
    }
};

struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

class HuffmanCoder {
private:
    Node* root;
    unordered_map<char, string> huffmanCodes;

    void generateCodes(Node* root, string str) {
        if (!root) return;

        if (!root->left && !root->right) {
            huffmanCodes[root->ch] = str.empty() ? "0" : str;
        }

        generateCodes(root->left, str + "0");
        generateCodes(root->right, str + "1");
    }

    void deleteTree(Node* node) {
        if (!node) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

    void buildTree(const unordered_map<char, int>& freqMap) {
        priority_queue<Node*, vector<Node*>, Compare> minHeap;

        for (auto pair : freqMap) {
            minHeap.push(new Node(pair.first, pair.second));
        }

        while (minHeap.size() > 1) {
            Node* left = minHeap.top(); minHeap.pop();
            Node* right = minHeap.top(); minHeap.pop();

            Node* parent = new Node('\0', left->freq + right->freq);
            parent->left = left;
            parent->right = right;
            minHeap.push(parent);
        }

        root = minHeap.top();
    }

public:
    HuffmanCoder() : root(nullptr) {}
    ~HuffmanCoder() {
        deleteTree(root);
    }

    void compress(const string& inputFile, const string& outputFile) {
        ifstream inFile(inputFile, ios::binary);
        if (!inFile) {
            cerr << "Error: Cannot open input file." << endl;
            return;
        }

        unordered_map<char, int> freqMap;
        char ch;
        string text = "";
        while (inFile.get(ch)) {
            freqMap[ch]++;
            text += ch;
        }
        inFile.close();

        if (text.empty()) {
            cout << "File is empty, nothing to compress." << endl;
            return;
        }

        buildTree(freqMap);
        generateCodes(root, "");

        string encodedString = "";
        for (char c : text) {
            encodedString += huffmanCodes[c];
        }

        int padding = (8 - (encodedString.length() % 8)) % 8;

        ofstream outFile(outputFile, ios::binary);

        size_t mapSize = freqMap.size();
        outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

        for (auto const& pair : freqMap) {
            outFile.write(&pair.first, sizeof(pair.first));
            outFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
        }

        outFile.write(reinterpret_cast<const char*>(&padding), sizeof(padding));

        unsigned char bitBuffer = 0;
        int bitCount = 0;

        for (char bit : encodedString) {
            bitBuffer = (bitBuffer << 1) | (bit - '0'); 
            bitCount++;

            if (bitCount == 8) { 
                outFile.write(reinterpret_cast<const char*>(&bitBuffer), sizeof(bitBuffer));
                bitBuffer = 0;
                bitCount = 0;
            }
        }

        if (bitCount > 0) {
            bitBuffer = bitBuffer << (8 - bitCount);
            outFile.write(reinterpret_cast<const char*>(&bitBuffer), sizeof(bitBuffer));
        }

        outFile.close();
        cout << "Compression complete. File saved to: " << outputFile << endl;
    }

    void decompress(const string& inputFile, const string& outputFile) {
        ifstream inFile(inputFile, ios::binary);
        if (!inFile) {
            cerr << "Error: Cannot open compressed file." << endl;
            return;
        }

        size_t mapSize;
        inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

        unordered_map<char, int> freqMap;
        for (size_t i = 0; i < mapSize; i++) {
            char ch;
            int freq;
            inFile.read(&ch, sizeof(ch));
            inFile.read(reinterpret_cast<char*>(&freq), sizeof(freq));
            freqMap[ch] = freq;
        }

        int padding;
        inFile.read(reinterpret_cast<char*>(&padding), sizeof(padding));

        deleteTree(root); 
        root = nullptr;
        buildTree(freqMap);

        vector<unsigned char> compressedBytes((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
        inFile.close();

        string bitString = "";
        for (size_t i = 0; i < compressedBytes.size(); i++) {
            unsigned char byte = compressedBytes[i];
            
            int bitsToRead = (i == compressedBytes.size() - 1) ? (8 - padding) : 8;

            for (int j = 7; j >= 8 - bitsToRead; j--) {
                bitString += ((byte >> j) & 1) ? '1' : '0';
            }
        }

        ofstream outFile(outputFile, ios::binary);
        Node* curr = root;

        for (char bit : bitString) {
            if (bit == '0') curr = curr->left;
            else curr = curr->right;

            if (!curr->left && !curr->right) {
                outFile.put(curr->ch);
                curr = root; 
            }
        }
        
        outFile.close();
        cout << "Decompression complete. File saved to: " << outputFile << endl;
    }
};

int main() {
    string inFile;
    
    cout << "Enter the name of the file to compress (e.g., Filename.txt): ";
    cin >> inFile;

    string compressedFile = "compressed_" + inFile + ".bin";
    string decompressedFile = "unzipped_" + inFile;

    HuffmanCoder coder;

    cout << "\nStarting compression for: " << inFile << "...\n" << endl;
    
    coder.compress(inFile, compressedFile);
    coder.decompress(compressedFile, decompressedFile);

    ifstream f1(inFile, ios::binary | ios::ate);
    ifstream f2(compressedFile, ios::binary | ios::ate);
    
    if (f1.is_open() && f2.is_open()) {
        long long originalSize = f1.tellg();
        long long compressedSize = f2.tellg();
        double ratio = (1.0 - ((double)compressedSize / originalSize)) * 100;

        cout << "-----------------------------------" << endl;
        cout << "Original File Size:   " << originalSize << " bytes" << endl;
        cout << "Compressed File Size: " << compressedSize << " bytes" << endl;
        cout << "Space Saved:          " << ratio << "%" << endl;
    } else {
        cout << "Could not calculate sizes. Please ensure the file exists." << endl;
    }

    return 0;
}