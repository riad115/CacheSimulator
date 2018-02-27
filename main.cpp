#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <bitset>

using namespace std;
static int cacheCapacity;
static int associativity;
static int blocksize;
static int nblks;
static int nsets;
static int offsetbits;
static int setbits; //index bits
static int tagbits;
static long nAccess=0;
static long nMiss=0;
static long nHit=0;
static long cache[1024][64];
static long lru[1024][64];

bool searchTag (long tagNumber, long setNumber){
    bool found = false;
        for ( int j = 0; j < associativity; ++j ){
                if(cache[setNumber][j]==tagNumber){
                    found = true;
                }
        }

    return found;
}

long searchemptyBlock(long tagNumber, long setNumber){
    long found = -1;
        for ( int j = 0; j < associativity; ++j ){
                if(cache[setNumber][j] < 0){
                    found = j;
                }
        }


    return found;
}

long searchMaxRef(long tagNumber, long setNumber){
    long found = 0;
        for ( int j = 0; j < associativity; ++j ){
                if(lru[setNumber][j] > found){
                    found = lru[setNumber][j];
                }
        }

    return found;
}

long searchLRUIndex(long tagNumber, long setNumber){
    long found = lru[setNumber][0];
    long index = 0;

        for ( int j = 0; j < associativity; ++j ){
                if(lru[setNumber][j] < found){
                    index = j;
                }
        }

    return index;
}


void cacheInitialization(long tagNumber, long setNumber, long blocksize){
        long blockIndex;
        long LRUIndex;
        long tagIndex;



        if(cache[setNumber][0]==-1){
            cache[setNumber][0] = tagNumber;
            lru[setNumber][0]= 1;
            nMiss++;
        }
        else{
        if(!searchTag(tagNumber, setNumber)){
            if(searchemptyBlock(tagNumber, setNumber) < 0){
                LRUIndex = searchLRUIndex(tagNumber, setNumber);
                cache[setNumber][LRUIndex] = tagNumber;
                lru[setNumber][LRUIndex] = searchMaxRef(tagNumber, setNumber)+1;
            }

            else{
                blockIndex = searchemptyBlock(tagNumber, setNumber);
                cache[setNumber][blockIndex] = tagNumber;
                lru[setNumber][blockIndex] = searchMaxRef(tagNumber, setNumber)+1;
            }
            nMiss++;
        }

        else{

                    for ( int j = 0; j < associativity; ++j ){
                        if(cache[setNumber][j]==tagNumber){
                            tagIndex = j;
                        }
                    }
                lru[setNumber][tagIndex] = searchMaxRef(tagNumber, setNumber)+1;
                nHit++;

        }
        }


}

void readInputfile(string filename){

  ifstream fin;
  string line;
  fin.open(filename.c_str()); // open a file

  if (fin.is_open())
  {
    while ( getline (fin ,line) )
    {
      //cout << line << '\n';
      if(line.find("Cache")!=-1){
        cacheCapacity =atoi(line.substr(11,5).c_str());
      }

      if(line.find("Block")!=-1){
        blocksize =atoi(line.substr(11,2).c_str());
      }

      if(line.find("Associativity")!=-1){
        associativity =atoi(line.substr(14,1).c_str());
      }
    }
    fin.close();
  }

  //cout << cacheCapacity << '\n';
  //cout << blocksize << '\n';
  //cout << associativity << '\n';

  nblks = cacheCapacity / blocksize ;
		//System.out.println(nblks);
		nsets = cacheCapacity / (associativity * blocksize );
		offsetbits = (int) log2((double) blocksize);
		setbits = (int) log2((double) nsets);
		tagbits = 32 - (offsetbits + setbits);



}
string hextobin(string s){
    stringstream ss;
    ss << hex << s;
    unsigned n;
    ss >> n;
    bitset<32> b(n);
    // outputs "00000000000000000000000000001010"
    //cout << b.to_string() << endl;
    return b.to_string();
}

long binConv(string binNum)
{
	long decimal = 0;
	int asize = binNum.size();

	for (int counter = asize-1; counter >=0; counter--)
	{
		if (binNum[counter] == '1')
			decimal = (decimal + pow(2.0,asize-(counter+1)));
	}
	//cout << decimal << '\n';
	return decimal;
}

void readAddressfile(string filename){
    ifstream fin;
  string line;
  string address;
  string binaryAddress;
  long setNumber;
  long tagNumber;
  long blockNumber;

  fin.open(filename.c_str());

  if (fin.is_open())
  {
    while ( getline (fin ,line) )
    {
        //cout << line << '\n';
        nAccess++;
        address = line.substr(2,8).c_str();
        binaryAddress = hextobin(address);
        tagNumber = binConv(binaryAddress.substr(0,tagbits));
        setNumber = binConv(binaryAddress.substr(tagbits, setbits));
        blockNumber = binConv(binaryAddress.substr((tagbits+setbits), offsetbits));
        //cout << binaryAddress << '\n';
        //cout << tagNumber << '\n';
        //cout << setNumber << '\n';
        //cout << blockNumber << '\n';
        cacheInitialization(tagNumber, setNumber, blockNumber);
    }
    fin.close();
  }

}

int main(int argc, char *argv[])
{
    string fileName = argv[1];
    readInputfile(fileName);
    for ( int i = 0; i < nsets; ++i ){
                    for ( int j = 0; j < associativity; ++j ){
                        cache[i][j]=-1;
                        lru[i][j]=-1;
                    }
                }
    fileName = argv[2];
    readAddressfile(fileName);


    double hitRatio = (double) nHit / (double) nAccess;
    cout << "Number of blocks: " << nblks << '\n';
    cout << "Number of sets: " << nsets << '\n';
    cout << "Offset length: " << offsetbits << '\n';
    cout << "Index length: " << setbits << '\n';
    cout << "Tag length: " << tagbits << '\n';
    cout << "Number of addresses accessed: " << nAccess << '\n';
    cout << "Number of hits: " << nHit << '\n';
    cout << "Number of misses: " << nMiss << '\n';
    cout << "The cache hit rate: " << hitRatio << '\n';

    return 0;
}



