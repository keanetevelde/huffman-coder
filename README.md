# huffman-coder

This is a text compression program that uses Huffman Coding which is a really
efficent (apparently the most efficent) text compression method. I used the
[wikipedia article](https://en.wikipedia.org/wiki/Huffman_coding) for info on the algorithm and attempted to make the program
on my own. Currently it works with any 8 bit data type, (might get it to work
with larger), and will output a vector with binary representing the origional
data, and then turn that binary back into the origional data.


So far, you can:

```c++
vector<unsigned char>* compressedBinary = HuffmanCoder<char>::newEncodedVector(charVectorContainingText);
  
vector<char>* decompressedText = HuffmanCoder<char>::newDecodedVector(unsignedCharVectorContainingBinary);
```


I came across this video which is what made me want to try this:
[![How Computers Compress Text: Huffman Coding and Huffman Trees](http://img.youtube.com/vi/JsTptu56GM8/0.jpg)](http://www.youtube.com/watch?v=JsTptu56GM8)
  
  
  
 TODO:
 - make the createFile functions, and dont make vectors unless you have to
 - limit max size of vectors -> total memory used at once by program
 - fill out given vector? also fill existing section of file? create an iterator??


