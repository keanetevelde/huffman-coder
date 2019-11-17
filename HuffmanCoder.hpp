#pragma once
#include <vector>
#include <bitset>
#include <string>
#include <fstream>

/* README
 * This is a text compression program that uses Huffman Coding which is a really
 * efficent (apparently the most efficent) text compression method. I used the
 * wikipedia article for info on the algorithm and attempted to make the program
 * on my own. Currently it works with any 8 bit data type, (might get it to work
 * with larger), and will output a vector with binary representing the origional
 * data, and then turn that binary back into the origional data.
 */


/*  TODO:
  - make the createFile functions, and dont make vectors unless you have to
  - limit max size of vectors -> total memory used at once by program
  - fill out given vector? also fill existing section of file? create an iterator??
*/

template <typename T>
class HuffmanCoder {

  public:
    /* to use:
    vector<unsigned char>* compressedBinary =
      HuffmanCoder<char>::newEncodedVector(charVectorContainingText);
    vector<char>* decompressedText =
      HuffmanCoder<char>::newDecodedVector(unsignedCharVectorContainingBinary);
    */

    /* TODO
    static void createEncodedFile(std::string dataInFileName, std::string binOutFileName);
    static void createEncodedFile(std::string dataInFileName, std::istream& binOutFile);
    static void createEncodedFile(std::istream& dataInFile, std::string binOutFileName);
    static void createEncodedFile(std::istream& dataInFile, std::istream& binOutFile);
    static void createEncodedFile(std::vector<T>& dataIn, std::string binOutFileName);
    static void createEncodedFile(std::vector<T>& dataIn, std::istream& binOutFile);
    */
    static std::vector<unsigned char>* newEncodedVector(std::vector<T>& dataIn);
    static std::vector<unsigned char>* newEncodedVector(std::istream& dataInFile);
    static std::vector<unsigned char>* newEncodedVector(std::string dataInFileName);
    /* TODO
    static void createDecodedFile(std::string binInFileName, std::string dataOutFileName);
    static void createDecodedFile(std::string binInFileName, std::istream& dataOutFile);
    static void createDecodedFile(std::istream& binInFile, std::string dataOutFileName);
    static void createDecodedFile(std::istream& binInFile, std::istream& dataOutFile);
    static void createDecodedFile(std::vector<unsigned char>& binIn, std::string dataOutFileName);
    static void createDecodedFile(std::vector<unsigned char>& binIn, std::istream& dataOutFile);
    */
    static std::vector<T>* newDecodedVector(std::vector<unsigned char>& binIn);
    static std::vector<T>* newDecodedVector(std::istream& binInFile);
    static std::vector<T>* newDecodedVector(std::string binInFileName);

  private:

    struct Node {
      Node* left;               // indicates left or right down the
      Node* right;              //huffman tree
      Node* searchLeft;         // indicates left or right down the
      Node* searchRight;        //lexographically sorted searchTree
      unsigned int count;       // the count of instances of this data
      std::bitset<16> key;      // the binary combo that represents this data
      unsigned char keyLength;  // the number of bits in the key
      T data;                   // the data

      Node();
      Node(T data);
      Node(Node* left, Node* right);
      ~Node();
      void insert(T data);      // makes node for new data, otherwise increments
      void setUniqueTag();      // sets key[15] to 1 for all searchTree nodes
      Node* uniqueNode();       // grabs a node w/ key[15]=1 and sets it to 0
    };

    struct NodeQueue {
      NodeQueue* next;
      Node* node;

      NodeQueue();
      NodeQueue(Node* node);
      NodeQueue(Node* node, NodeQueue* next);
      ~NodeQueue();
      void add(Node* node);
      Node* pop();
    };

    static Node* readDataIntoSearchTree(std::vector<T>& dataIn);
    static NodeQueue* buildPriorityQueue(Node* searchTree);
    static Node* buildHuffmanTree(NodeQueue* priorityQueue);
    static void encodeHuffmanTree(std::vector<unsigned char>* binOut, Node* node);
    static void encodeData(std::vector<T>& dataIn, std::vector<unsigned char>* binOut, Node* searchTree);

    static Node* decodeHuffmanTree(std::vector<unsigned char>& binIn, int& dataIndex);
    static std::vector<T>* decodeBin(std::vector<unsigned char>& binIn, int dataIndex, Node* huffmanTree);

    static std::string getKey(T data, Node* node);
    static void updateKeys(Node* node);
    static void appendKey(Node* node, bool i);

};


/* HuffmanCoder functions implementation */

//public

template <typename T>
std::vector<unsigned char>* HuffmanCoder<T>::newEncodedVector(std::vector<T>& dataIn) {
  std::vector<unsigned char>* binOut = new std::vector<unsigned char>();
  Node* searchTree;
  Node* huffmanTree = nullptr;
  NodeQueue* priorityQueue = nullptr;

  searchTree = readDataIntoSearchTree(dataIn);
  priorityQueue = buildPriorityQueue(searchTree);
  huffmanTree = buildHuffmanTree(priorityQueue);

  binOut->reserve(dataIn.size() * sizeof(T));
  encodeHuffmanTree(binOut, searchTree);
  encodeData(dataIn, binOut, searchTree);

  binOut->shrink_to_fit();
  delete huffmanTree;
  delete priorityQueue;
  return binOut;
}

template <typename T>
std::vector<unsigned char>* HuffmanCoder<T>::newEncodedVector(std::istream& dataInFile) {
  std::vector<T> dataIn ((std::istreambuf_iterator<T>(dataInFile)), std::istreambuf_iterator<T>());
  return newEncodedVector(dataIn);
}

template <typename T>
std::vector<unsigned char>* HuffmanCoder<T>::newEncodedVector(std::string dataInFileName) {
  std::ifstream dataInFile (dataInFileName);
  std::vector<unsigned char>* binOut = newEncodedVector(dataInFile);
  dataInFile.close();
  return binOut;
}

template <typename T>
std::vector<T>* HuffmanCoder<T>::newDecodedVector(std::vector<unsigned char>& binIn) {
  std::vector<T>* dataOut;
  Node* huffmanTree;
  int dataIndex = 0;

  huffmanTree = decodeHuffmanTree(binIn, dataIndex);

  dataOut = decodeBin(binIn, dataIndex, huffmanTree);

  delete huffmanTree;
  return dataOut;
}

template <typename T>
std::vector<T>* HuffmanCoder<T>::newDecodedVector(std::istream& binInFile) {
  std::vector<unsigned char> binIn ((std::istreambuf_iterator<char>(binInFile)), std::istreambuf_iterator<char>());
  return newDecodedVector(binIn);
}

template <typename T>
std::vector<T>* HuffmanCoder<T>::newDecodedVector(std::string binInFileName) {
  std::ifstream binInFile (binInFileName);
  std::vector<T>* dataOut = newDecodedVector(binInFile);
  binInFile.close();
  return dataOut;
}


//private

template <typename T>
typename HuffmanCoder<T>::Node* HuffmanCoder<T>::readDataIntoSearchTree(std::vector<T>& dataIn) {
  Node* searchTree = nullptr;
  if (dataIn.size() == 0) {
    return searchTree;
  }
  searchTree = new Node(*(dataIn.begin()));
  for (auto i = (dataIn.begin()+1); i != dataIn.end(); ++i) {
    searchTree->insert(*i);
  }
  return searchTree;
}

template <typename T>
typename HuffmanCoder<T>::NodeQueue* HuffmanCoder<T>::buildPriorityQueue(Node* searchTree) {
  NodeQueue* priorityQueue = new NodeQueue();
  searchTree->setUniqueTag();
  Node* node = searchTree->uniqueNode();
  while (node != nullptr) {
    priorityQueue->add(node);
    node = searchTree->uniqueNode();
  }
  return priorityQueue;
}

template <typename T>
typename HuffmanCoder<T>::Node* HuffmanCoder<T>::buildHuffmanTree(NodeQueue* priorityQueue) {
  Node* huffmanTree;
  Node* node1 = priorityQueue->pop(); //get next 2 nodes
  Node* node2 = priorityQueue->pop();
  while (node2 != nullptr) {
    huffmanTree = new Node(node2, node1); //create node that points to them
    updateKeys(huffmanTree); //update keys
    priorityQueue->add(huffmanTree); //add node to nodeQueue
    node1 = priorityQueue->pop();
    node2 = priorityQueue->pop();
  }
  return huffmanTree;
}

template <typename T>
void HuffmanCoder<T>::encodeHuffmanTree(std::vector<unsigned char>* binOut, Node* searchTree) {
  binOut->push_back(0);
  std::bitset<16> key;
  unsigned char numData = 0;
  Node* node;
  searchTree->setUniqueTag();
  node = searchTree->uniqueNode();
  while (node != nullptr) {
    key = node->key;
    key[node->keyLength] = 1;
    binOut->push_back(static_cast<unsigned char>(node->data));
    binOut->push_back(static_cast<unsigned char>((std::bitset<8>((key >> 8).to_ulong())).to_ulong()));
    binOut->push_back(static_cast<unsigned char>((std::bitset<8>((key.to_ulong()))).to_ulong()));
    numData++;
    node = searchTree->uniqueNode();
  }
  binOut->at(0) = numData;
}

template <typename T>
void HuffmanCoder<T>::encodeData(std::vector<T>& dataIn, std::vector<unsigned char>* binOut, Node* searchTree) {
  std::string keys = "";
  auto data = dataIn.begin();
  unsigned char underflow = 0;
  while (data != dataIn.end()) {
    if (keys.size() < 8) {
      keys += getKey(*data,searchTree);
      data++;
    }
    if (keys.size() >= 8) {
      binOut->push_back(static_cast<unsigned char>((std::bitset<8>(keys.substr(0,8))).to_ulong()));
      keys.erase(0,8);
    }
  }
  while (keys.size() < 8 && keys.size() != 0) {
    keys += "0";
    underflow++;
  }
  if (keys.size() == 8) {
    binOut->push_back(static_cast<unsigned char>((std::bitset<8>(keys.substr(0,8))).to_ulong()));
  }

  binOut->push_back(underflow);
  return;
}


template <typename T>
typename HuffmanCoder<T>::Node* HuffmanCoder<T>::decodeHuffmanTree(std::vector<unsigned char>& binIn, int& dataIndex) {
  Node* huffmanTree = new Node();
  Node* node = huffmanTree;
  auto bin = binIn.begin();
  std::bitset<8> bits;
  unsigned char index = 0;
  unsigned char huffmanLength = (*bin);
  bool ignore = true;
  bin++;
  while (index < huffmanLength) {
    T data = static_cast<T>(*bin);
    bin++;
    for (int repeat = 0; repeat < 2; repeat++) {
      bits = (std::bitset<8>(*bin));
      bin++;
      for (int i = 7; i >= 0; i--) {
        if (ignore) {
          if (bits[i]) {
            ignore = false;
          }
        } else {
          if (bits[i]) {
            if (node->right == nullptr) {
              node->right = new Node();
            }
            node = node->right;
          } else {
            if (node->left == nullptr) {
              node->left = new Node();
            }
            node = node->left;
          }
        }
      }
    }
    node->data = data;
    node->count = 1;
    node = huffmanTree;
    ignore = true;
    index++;
  }
  dataIndex = 1 + (index * 3);
  return huffmanTree;
}

template <typename T>
std::vector<T>* HuffmanCoder<T>::decodeBin(std::vector<unsigned char>& binIn, int dataIndex, Node* huffmanTree) {
  std::vector<T>* dataOut = new std::vector<T>();
  std::bitset<8> bits;
  Node* indexNode = huffmanTree;

  dataOut->reserve(binIn.size() * sizeof(T) * 2);

  auto bin = binIn.begin() + dataIndex;
  auto binEnd = (binIn.end()-1);
  unsigned char underflow = (*binEnd);
  binEnd--;

  while (bin != binEnd) {
    bits = (std::bitset<8>(*bin));
    bin++;
    for (int i = 7; i >= 0; i--) {
      if (bits[i]) {
        indexNode = indexNode->right;
      } else {
        indexNode = indexNode->left;
      }
      if (indexNode->count == 1) {
        dataOut->push_back(indexNode->data);
        indexNode = huffmanTree;
      }
    }
  }
  bits = (std::bitset<8>(*bin));
  for (int i = 7; i >= underflow; i--) {
    if (bits[i]) {
      indexNode = indexNode->right;
    } else {
      indexNode = indexNode->left;
    }
    if (indexNode->count == 1) {
      dataOut->push_back(indexNode->data);
      indexNode = huffmanTree;
    }
  }

  dataOut->shrink_to_fit();
  return dataOut;
}

template <typename T>
std::string HuffmanCoder<T>::getKey(T data, Node* node) {
  if (node->data == data) {
    return node->key.to_string().erase(0,(16 - node->keyLength));
  }
  if (node->data < data) {
    return getKey(data, node->searchRight);
  }
  return getKey(data, node->searchLeft);
}

template <typename T>
void HuffmanCoder<T>::updateKeys(Node* node) {
  appendKey(node->left, 0);
  appendKey(node->right, 1);
}

template <typename T>
void HuffmanCoder<T>::appendKey(Node* node, bool i) {
  node->key[node->keyLength] = i;
  node->keyLength++;
  if (node->left != nullptr) {
    appendKey(node->left, i);
  }
  if (node->right != nullptr) {
    appendKey(node->right, i);
  }
}


/*Node functions implementation*/

template <typename T>
HuffmanCoder<T>::Node::Node()
{
  this->left = nullptr;
  this->right = nullptr;
  this->count = 0;
}

template <typename T>
HuffmanCoder<T>::Node::Node(T data)
{
  this->data = data;
  this->count = 1;
  this->keyLength = 0;
  this->left = nullptr;
  this->right = nullptr;
  this->searchLeft = nullptr;
  this->searchRight = nullptr;
}

template <typename T>
HuffmanCoder<T>::Node::Node(Node* left, Node* right)
{
  this->count = left->count + right->count;
  this->keyLength = 0;
  this->left = left;
  this->right = right;
  this->searchLeft = nullptr;
  this->searchRight = nullptr;
}

template <typename T>
HuffmanCoder<T>::Node::~Node()
{
  delete left;
  delete right;
}

template <typename T>
void HuffmanCoder<T>::Node::insert(T data) {
  if (this->data > data) {
    if (this->searchLeft == nullptr) {
      this->searchLeft = new Node(data);
      return;
    }
    this->searchLeft->insert(data);
    return;
  }
  if (this->data < data) {
    if (this->searchRight == nullptr) {
      this->searchRight = new Node(data);
      return;
    }
    this->searchRight->insert(data);
    return;
  }
  if (this->data == data) {
    (this->count)++;
  }
}

template <typename T>
void HuffmanCoder<T>::Node::setUniqueTag() {
  this->key[15] = 1;
  if (this->searchLeft != nullptr) {
    this->searchLeft->setUniqueTag();
  }
  if (this->searchRight != nullptr) {
    this->searchRight->setUniqueTag();
  }
}

template <typename T>
typename HuffmanCoder<T>::Node* HuffmanCoder<T>::Node::uniqueNode() {
  if (this->searchLeft != nullptr) {
    if (this->searchLeft->key[15] == 1) {
      return this->searchLeft->uniqueNode();
    }
  }
  if (this->searchRight != nullptr) {
    if (this->searchRight->key[15] == 1) {
      return this->searchRight->uniqueNode();
    }
  }
  if (this->key[15] == 0) {
    return nullptr;
  }
  this->key[15] = 0;
  return this;
}


/* NodeQueue functions implementation */

template <typename T>
HuffmanCoder<T>::NodeQueue::NodeQueue() {
  node = nullptr;
  next = nullptr;
}

template <typename T>
HuffmanCoder<T>::NodeQueue::NodeQueue(Node* node) {
  this->node = node;
  next = nullptr;
}

template <typename T>
HuffmanCoder<T>::NodeQueue::NodeQueue(Node* node, NodeQueue* next) {
  this->node = node;
  this->next = next;
}

template <typename T>
HuffmanCoder<T>::NodeQueue::~NodeQueue() {
  delete next;
}

template <typename T>
void HuffmanCoder<T>::NodeQueue::add(Node* node) {
  if (this->node == nullptr) {
    if (this->next == nullptr) {
      this->next = new NodeQueue(node);
      return;
    }
    return this->next->add(node);
  }
  if (node->count <= this->node->count) {
    NodeQueue* tempQueue = new NodeQueue(this->node, this->next);
    this->node = node;
    this->next = tempQueue;
    return;
  }
  if (this->next == nullptr) {
    this->next = new NodeQueue(node);
    return;
  }
  return this->next->add(node);
}

template <typename T>
typename HuffmanCoder<T>::Node* HuffmanCoder<T>::NodeQueue::pop() {
  if (this->next == nullptr) {
    return nullptr;
  }
  Node* tempNode = this->next->node;
  NodeQueue* oldQueue = this->next;
  this->next = this->next->next;
  oldQueue->next = nullptr;
  delete oldQueue;
  return tempNode;
}
