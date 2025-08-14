# **File reversal with different modes**
This is an Assignment done as part of the Advanced Operating Systems(AOS) course.
1. It deals with reversal of file content using different strategies and store the result in a new file in the directory named Assignment1.
2. Verifies whether the written content is reversed according to the required mode or not.

**The different modes of reversal operaions are:**
1. Block-wise reversal
2. Full file reversal
3. Partial range reversal

### Logic behind each mode and command to run the files is as follows:

## Flag 0 : Block-wise reversal
We will be given a block size, and content in each block should be reversed.
1. Take CHUNK_SIZE = 512kb and a buffer of size same as CHUNK_SIZE.
2. Read 512kb of data and reverse the characters in buffer block-wise and keep writing to output file.
3. To verify, open and read both input and output files parallely and verify the reversal of content.

### commands for reversal:
    $ g++ Q1.cpp -o Q1
    $ ./Q1 ./input.txt 0 <block_size>

### commands for verification of content and permissions:
    $ g++ Q2.cpp -o Q2
    $ ./Q2 ./input.txt ./<dir_name>/0_input.txt ./<dir_name> 0 <block_size>

## Flag 1: Full file reversal
Reverse the entire content of the file
1. Read the data chunk-wise from the end and keep writing it to the output file
### commands for reversal:
    $ g++ Q1.cpp -o Q1
    $ ./Q1 ./input.txt 1
### commands for verification of content and permissions:
    $ g++ Q2.cpp -o Q2
    $ ./Q2 ./input.txt ./<dir_name>/1_input.txt ./<dir_name> 1

## Flag 2: Partial range reversal
Except for the content between the given range remaining content should be reversed
1. Reverse the content from beginning of the file to the first_index and then from second_index to end.
### commands for reversal:
    $ g++ Q1.cpp -o Q1
    $ ./Q1 ./input.txt 2 <first_index> <second_index>
### commands for verification of content and permissions:
    $ g++ Q2.cpp -o Q2
    $ ./Q2 ./input.txt ./<dir_name>/2_input.txt ./<dir_name> 2 <first_index> <second_index>
