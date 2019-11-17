# huffman-coder

This is a text compression program that uses Huffman Coding which is a really
efficent (apparently the most efficent) text compression method. I used the
wikipedia article for info on the algorithm and attempted to make the program
on my own. Currently it works with any 8 bit data type, (might get it to work
with larger), and will output a vector with binary representing the origional
data, and then turn that binary back into the origional data.

to use:
vector<unsigned char>* compressedBinary =
  HuffmanCoder<char>::newEncodedVector(charVectorContainingText);
  
vector<char>* decompressedText =
  HuffmanCoder<char>::newDecodedVector(unsignedCharVectorContainingBinary);
  
 TODO:
 - make the createFile functions, and dont make vectors unless you have to
 - limit max size of vectors -> total memory used at once by program
 - fill out given vector? also fill existing section of file? create an iterator??


