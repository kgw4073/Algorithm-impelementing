//==========================================================================//
// huffman.cpp																//
// Huffman Coding for File Compression										//
// version : 3.0, 11/17/2019												//
// Keonwoo Kim																//
// usage: ./mp3_20150514 flag("-c" or "-d") (inputFileName)					//
//==========================================================================//
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>

using namespace std;

//==================================================================//
// ������ Ʈ���� ����� ���� ����ü�� ����.								//
// 1. character, 2. ���� Ƚ��, 3. leftpointer, 4. rightpointer		//
//																	//
//==================================================================//
struct Node {
	char c;
	int count;
	struct Node *left;
	struct Node *right;
};

// �� Node ����ü�� priority_queue(min heap)�� ��� ���� Compare ����ü�� �����ϰ� operator�� ������ ���־���.

struct Compare {
	bool operator() (const Node* first, const Node* second) {
		return first->count > second->count;
	}
};
// ������ Ʈ�� ������ ���� ��� ������
struct Node* u, * v, * w;
// ������ character�鿡 ����, ���Ͽ� ���� ������ �����ϱ� ���� �迭
int FrequencyCount[256] = { 0, };
// ASCII CODE�� �ش��ϴ� ���ڿ� ���� ������ �ڵ带 �����ϱ� ���� string �迭
string BitString[256];
// Ʈ�� ��� �����͸� vector�� �����ϸ� �� �����ڷ� Compare�� �����ڸ� ����ϴ� �ּ� ���� ����
priority_queue<struct Node*, vector<struct Node*>, Compare> pq;

//dfs�� Ʈ���� ���� ��带 Ž���ϸ� ������ ������ BitString �迭�� ���� �ε����� �ش��ϴ� ������ �ڵ� ���ڿ��� �����Ѵ�.
void dfs(struct Node* w, string s) {
	// ���� ���� ��忡 �������� �ʾ��� �� ����ؼ� ��ȸ
	if (w->left != NULL || w->right != NULL) {
		// left�� "0"�� �߰��ؼ� ���� ������ ���� right�� "1"�� �߰��ؼ� ���� ������ ����.
		dfs(w->left, s + "0");
		dfs(w->right, s + "1");
	}
	// ���� ��忡 �������� ���� �ش��ϴ� ������ ASCII CODE���� �ش��ϴ� BitString �ε����� ���ڷ� �Ѿ�� ���ڿ��� ����.
	else {
		BitString[w->c] = s;
	}
}

// ������ Ʈ���� ���� �Ҵ�� ��带 �����ϱ� ����
void DeallocationTree(struct Node* w) {
	// ���� ���� Ʈ���� ����
	if (w->left) {
		DeallocationTree(w->left);
	}
	// ������ ���� Ʈ���� ����
	if (w->right) {
		DeallocationTree(w->right);
	}
	delete w;
}

// �� ���α׷��� ���� �Ҵ�� �޸𸮸� �����ϱ� ���� �Լ�.
// ���⼭�� ������ Ʈ�����̹Ƿ� Tree�� ������.
void DeallocationMemory(struct Node* w) {
	DeallocationTree(w);
}

//==============================================================================================//
// Huffman Tree Making Function																	//						
// parameter : LeafNodeCount(���� ����� ����, ��, ���Ͽ� ��Ÿ�� ������ ����)						//
// 																								//
// priority_queue�� �̿��Ͽ� ������ �˰��� ���� ������ Ʈ���� �����Ѵ�.							//
//																								//
// execution result : w�� ������ Ʈ���� root node�� �ȴ�.											//
//																								//																		//
//==============================================================================================//
void MakeTree(int LeafNodeCount) {

	// LeafNodeCount-1�� ��ŭ�� ������ ����.
	// �� ������ Ʈ���� ����� ������ heap�� ����� �Ϳ��� �� ��带 �Ѳ����� pop�ϰ� �� ���� ���ο� ��� w�� �ڽ����� �α� �����̴�.
	// ���� LeafNodeCount - 1������ ������ ���� Ż���� �Ŀ� pq�� �����ִ� �� ���� ��忡�� ��� ���ڰ� ������ Ƚ���� ������ ����Ǿ� ���� ���̴�.
	for (int i = 0; i < LeafNodeCount - 1; i++) {
		// min heap���� ��� �� ���� ������.
		u = pq.top();
		pq.pop();
		v = pq.top();
		pq.pop();

		// w�� ���ο� ��带 ���� �Ҵ��Ѵ�.
		w = new struct Node();
		// w�� left���� u, right���� v�� �����ϰ� count�� �� ����� count�� ���� �Ҵ��Ѵ�.
		w->left = u;
		w->right = v;
		w->count = u->count + v->count;
		// �̰��� �ٽ� pq�� �����Ѵ�.
		pq.push(w);
	}
	w = pq.top();
	pq.pop();
	// ������� ������ Ʈ���� ��Ʈ w�� dfs�Լ��� ȣ���Ͽ� �� ���� ��忡 ����� ���ڿ� �ش��ϴ� ������ �ڵ� ���ڿ��� BitString�� �����Ѵ�.
	dfs(w, "");
}

//==============================================================================================//
// Do Huffman Coding Function																	//
// parameter : File Name ���α׷� ����� �Ѱܹ��� ������ input File Name�� ���ڷ� �޴´�.			//
// 																								//
// ������ ������ MakeTree() function�̳� dfs() function�� ����Ͽ� ������ �ڵ��� �����Ѵ�.			//
//																								//
// execution result : FileName.zz�� ������ �ڵ��� ���ڿ��� ����ȴ�. �ٸ�, Decoding�� ���Ǽ��� ���� //
//					  input file�� ������ ���ڿ�, ���� Ƚ���� �����ϴ� �κ��� �߰��Ͽ���.			//
//																								//
//==============================================================================================//
void HuffmanCoding(const char* FileName) {

	// FullBitCount�� input file�� ��Ʈ�� ��ȯ���� �� ��Ʈ ���ڿ��� �� ���̸� ��Ÿ����.
	int FullBitCount = 0, LeafNodeCount = 0;
	;
	// c�� ���ڸ� ���� �ӽ� ����
	char c;
	// InputFullString�� ������ ���Ͽ� ����� ���ڿ��� �����ϸ�, InputBitString�� ����� InputFullString�� ������ �ڵ�� ��ȯ��Ű�� ������ ���ڿ� ��ü�̴�.
	string InputFullString = "";
	// Huffman Coding ��Ģ�� ���� ��ȯ�� ���ڿ��� ��� ���Ͽ� ������ ���ڿ� ��ü�� ����.
	string StringForEncoding;
	StringForEncoding.resize(1, 0);
	// FileName���� ���� �Է� ��Ʈ���� ����.
	ifstream TextFileIn(FileName, ios::binary);

	// Input file�� �������� �ʰų� ���� ��Ʈ���� ������ ������ ���� �޽����� ����ϰ� ���α׷��� �����Ѵ�.
	if (!TextFileIn.is_open()) {
		cerr << "Input file does not exist!" << endl;
		exit(-1);
	}

	// OutputFileName�� FileName�� ".zz"�� �ٿ��� �Ѵ�.
	string OutputFileName = string(FileName) + ".zz";

	//OutputFileName�� ������ binary��� ���� ��� ��Ʈ���� �����Ѵ�.
	ofstream EncodingOut(OutputFileName.c_str(), ios::binary);

	// ���� �Է� ��Ʈ���� get �޼ҵ带 ����Ͽ� �� ���ھ� �о����.
	while (TextFileIn.get(c)) {
		// InputFullString�� �о���� ���ڸ� string ��ü�� ��ȯ�Ͽ� ���Ѵ�.
		InputFullString += c;
		// �о���� ������ ���� Ƚ���� ������Ų��.
		FrequencyCount[c]++;
	}
	
	// ���� �Է� ��Ʈ���� �ݴ´�.
	TextFileIn.close();

	// LeafNodeCount�� �� ���Ͽ� ������ ������ �󸶳� �Ǵ��� �����ϴ� �����̴�.
	for (int i = 0; i < 256; i++) {
		// �� ���� �������� �ʾ����� continue
		if (!FrequencyCount[i]) continue;
		LeafNodeCount++;
	}
	// LeafNodeCount�� ����� ���Ͽ� ����
	EncodingOut << LeafNodeCount << '\n';

	// ����� ���Ͽ�, input file�� ������ ���ڿ� �� ���ڰ� ������ ������ �����ϱ� ���� �����̴�.
	for (int i = 0; i < 256; i++) {
		if (!FrequencyCount[i]) continue;

		struct Node* pushed = new struct Node();
		pushed->c = char(i);
		pushed->count = FrequencyCount[i];
		pushed->left = NULL;
		pushed->right = NULL;

		EncodingOut << pushed->c << ' ' << pushed->count << '\n';

		// Tree�� ����� ���� ���� ������ ������ priority_queue�� �����Ѵ�.
		pq.push(pushed);
	}

	// pq�� ����� ������ �������� ������ Ʈ���� �����. ���ο��� dfs�� �����ϸ� �� ���ڿ� �ش��ϴ� ���ڿ��� BitString�� �����Ѵ�.
	MakeTree(LeafNodeCount);

	// InputBitString�� InputFullString�� �� ���ڿ� �ش��ϴ� ��Ʈ ���ڿ��� �߰��Ѵ�.
	for (int i = 0, j = 7; i < InputFullString.size(); ) {

		// ���� Ž���ϰ� �ִ� InputFullString�� i��° �ε��� ���ڸ� c�� �ΰ�
		// �� ���ڿ� �ش��ϴ� BitString�� ������ StringForEncoding�� �ٷ� Encoding�Ѵ�.
		char c = InputFullString[i];
		for (int k = 0; k < BitString[c].size(); k++) {
			// j�� 0���� �۾����� �ٽ� 7�� ������Ʈ ���ش�. char�� 8bit�̱� ����
			if (j < 0) j = 7;

			// c�� BitString �� ���� ���� '1'�̸� LSB�κ��� j��° ��Ʈ�� set�Ѵ�.
			if (BitString[c][k] == '1') {
				StringForEncoding[FullBitCount / 8] |= (1 << j);
			}
			
			// FullBitCount�� �� Bit�� ���̸� ���� ������ 8�� ���� �������� 7�̸� ���� ������ �ʿ��ϹǷ� '\0' ����
			if (FullBitCount % 8 == 7) {
				StringForEncoding += string(1, 0);
			}

			// j�� LSB�κ��� ������ �����̹Ƿ� ���� �ٿ������� �Ѵ�.
			j--;
			FullBitCount++;
		}
		i++;
	}

	// InputBitString�� ���̸� ����
	EncodingOut << FullBitCount << '\n';
	// ������ �Ϸ�� ���ڿ��� ����.
	EncodingOut << StringForEncoding;
	
	// ��� ���� ��Ʈ���� �����Ѵ�.
	EncodingOut.close();
	// Ʈ���� �����Ѵ�.
	DeallocationMemory(w);
}

//===============================================================================================//
// Decoding Huffman Code																		 //
// parameter : File Name ���α׷� ����� �Ѱܹ��� ������ input File Name�� ���ڷ� �޴´�.			 //
// 																								 //
// ������ ������ MakeTree() function�̳� dfs() function�� �̿��Ͽ� ������ Ʈ���� �籸���ϰ� �̸� ���� //											//
// ������� ������ Ʈ���� Ž���� ���� ���ڸ� �籸���Ѵ�.												 //
//																								 //
// execution result : FileName.yy�� ������ ������ ���ڿ��� ����ȴ�.								 //
//																								 //
//===============================================================================================//
void DecodingHuffmanCode(const char* FileName) {

	// NodeCount�� ���� ������ ó���� ����� ���ڷ�, ������ ������ ������ �����̴�.
	// BitStringLength�� ������ Ǯ��� �� BitString�� ���̸� �����Ѵ�.
	int NodeCount, BitStringLength;

	// start�� end�� ���� ��Ʈ������ ������ ������ ���ڿ��� ���۵Ǵ� ������ ���� ��� ���� ����
	int start, end;
	// FileNameString�� FileName�� string ��ü�� �ٲ� ���̴�.
	string FileNameString = string(FileName);
	// StringForDecoding�� ������ Ǯ�� ���� input file�� ����� ������ �ڵ� ���ڿ��� ��� string �����̴�.
	string StringForDecoding = "";
	// FileName�� binary���� ���� �Է� ��Ʈ���� �����Ѵ�.
	ifstream DecoderIn(FileName, ios::binary);

	// �Է� ��Ʈ���� �������� ������ ������ ����ϰ� ���α׷��� �����Ѵ�.
	if (!DecoderIn.is_open()) {
		cerr << "Huffman Coded File does not exist!" << endl;
		exit(-1);
	}

	DecoderIn >> NodeCount;

	// NodeChar�� CharFrequency�� �Է��� �ޱ� ���� �ӽ÷� ������ �����̴�.
	char NodeChar;
	int CharFrequency;

	// '\n' ����
	DecoderIn.get(NodeChar);

	// min heap�� ����ش�.
	pq.empty();

	// min heap���ٰ�, ������ ���ڿ� �ش� ������ ������ �����ϱ� ���� loop�̴�.
	for (int i = 0; i < NodeCount; i++) {
		// ���� �Է� ��Ʈ������ ���� �ϳ��� �д´�.
		DecoderIn.get(NodeChar);
		// ���� ������ ���� Ƚ���� �д´�.
		DecoderIn >> CharFrequency;

		// �� ������ ������ ��带 �����Ͽ� min heap�� �����Ѵ�. (���� ������ Ʈ���� �ٽ� ����� �����̴�.)
		struct Node* pushed = new struct Node;
		pushed->c = NodeChar;
		pushed->count = CharFrequency;
		pushed->left = NULL;
		pushed->right = NULL;
		pq.push(pushed);
		
		// '\n' ����
		DecoderIn.get(NodeChar);
	}

	// Ǯ��� �� BitString�� ���̸� �Է¹޴´�.
	DecoderIn >> BitStringLength;
	// '\n' ����
	DecoderIn.get(NodeChar);

	// Huffman Coding function������ MakeTree�� ������ �����ϰ� ������ Ʈ���� �籸���Ѵ�.
	MakeTree(NodeCount);

	// C++�� ���� ��Ʈ�� �޼ҵ带 Ȱ���Ѵ�. 
	// tellg()�� ���� ���� ��Ʈ���� ��ġ�� ��ȯ�Ѵ�.
	start = DecoderIn.tellg();
	// seekg(OFFSET, point)�̴�. point�κ��� OFFSET��ŭ�� ��ġ�� �̵��Ѵ�.
	DecoderIn.seekg(0, ios::end);
	
	// �Է� ������ ���� ���� ���� ��Ʈ���� ��ġ�� end�� �����Ѵ�.
	end = DecoderIn.tellg();

	// StringForDecoding�� ���� ���Ͽ� ����� ���ڸ� �����ϱ� �����̴�. end-start��ŭ�� ���̸� ���´�.
	StringForDecoding.resize(end - start);

	// �ٽ� ������ ó�����κ��� start��ŭ ������ ������ ��Ʈ���� ��ġ�� �����Ѵ�.
	DecoderIn.seekg(start, ios::beg);
	// StringForDecoding[0]�� ��ġ�� end-start���̸�ŭ �о���δ�.
	DecoderIn.read(&StringForDecoding[0], end - start);
	// ���� �Է� ��Ʈ���� �����Ѵ�.
	DecoderIn.close();

	// DecodedFullString, ��, ���� ���� �� ���ڿ��� �����ϱ� ���� string ��ü�̴�.
	string DecodedFullString = "";

	// DecodedBitString�� �� ���� �� ���� �˻��ϸ� �����Ѵ�. �ٸ� for������ �������� ���� while�� �ȿ� �߰��Ͽ���.
	for (int i = 0, j = 7; i < BitStringLength; ) {
		// w���� ������ Ʈ���� ����Ǿ� �ִ�. ���ڸ� �� �� ����� ã�Ƴ� ������ ptr�� w�� �ٽ� �ٲپ� �־�� �Ѵ�.
		struct Node* ptr = w;
		// ptr�� DecodedBitString�� ���ڸ� ������ � �������� Ž���س�����.
		while (true) {
			if (j < 0) j = 7;
			// ������ Ʈ���� ���� ��尡 �ƴϸ� �ݵ�� �� ���� �ڽ� ��尡 �����Ѵ�.
			if (ptr->left && ptr->right) {
				// ���� DecodedBitString�� i / 8��° �ε����� LSB�κ��� j��° ��Ʈ�� 0�̸� left��, 1�̸� right�� ����. (�̴� ������ Ʈ���� ������ �� �׷��� ������� �����̴�.)
				if (StringForDecoding[i / 8] & (1 << j)) {
					ptr = ptr->right;
				}
				else ptr = ptr->left;
				// i�� �ϳ� ������Ų��.
				i++;
				j--;
			}
			// ���� ����̹Ƿ� while���� Ż���Ѵ�.
			else break;
		}
		// ���� ptr�� ����Ű�� �ִ� ���ڸ� DecodedFullString�� ���Ѵ�.
		DecodedFullString += ptr->c;
	}
	
	 
	// ���� ������ DecodedBitString�� ����� 0�̸�, ��, ������ �� ���ڸ� �־
	// �� ���ڿ� �ش��ϴ� ��Ʈ�� NULL�� ��� �� for loop�� ���� ���Ѵ�. ����
	// ���� Ʈ���� Ž���� �ʿ䰡 ���� ��� ����ó���� ���־���. �̷��� �ϸ�
	// �� ���ڸ� ���� �� ���� ������ ũ�Ⱑ �ſ� �۾�����.
	if (BitStringLength == 0) {
		for (int i = 0; i < w->count; i++) {
			DecodedFullString += string(1, w->c);
		}
	}

	// HuffmanDecodedFile�� FileNameString�� ".yy"�� �߰��� ���̴�.
	string HuffmanDecodedFile = FileNameString + ".yy";

	// HuffmanDecodedFile���� ���� ��� ��Ʈ���� �����ϰ� ���� ������ �Ϸ�� ���ڿ��� ����Ѵ�.
	ofstream DecoderOut(HuffmanDecodedFile.c_str(), ios::binary);
	DecoderOut << DecodedFullString;
	DecoderOut.close();
	

	//===================================================================================================
	// inputfile�� Decoded string�� ���̸� ���� ���� ������ �ӽ� �ڵ�
	//
	//string s = FileNameString.substr(0, FileNameString.size() - 2);
	//ifstream Tin(s.c_str(), ios::binary);
	//char c;
	//int count = 0;
	//string inputstring = "";
	//while (Tin.get(c)) {
	//	inputstring += c;
	//}
	//for (int i = 0; i < inputstring.size(); i++) {
	//	if (inputstring[i] != DecodedFullString[i]) {
	//		cout << i << '\n';
	//	}
	//}
	//Tin.close();
	//================================================================================================ =
	
	
	// ������ Ʈ���� �����Ѵ�.
	DeallocationMemory(w);
}


int main(int argc, char* argv[]) {
	// CodeFlag�� ������ ���� ������ ���� ��Ÿ���� ���ڿ� "-c" | "-d"�� ��� ���ڿ�
	string CodeFlag = argv[1];
	// InputFileName�� �� ��° ���ڷ� ����
	const char* InputFileName = argv[2];

	// ���ڰ� �� ���� �ƴϸ� �߸� �Է��� ���̹Ƿ� ������ ����ϰ� �����Ѵ�.
	if(argc != 3) {
		cerr << "Wrong Usage!" << endl;
		cerr << "Usage : binaryFile flag(-c or -d) inputFile" <<endl;
		exit(-1);
	}

	// CodeFlag�� "-c"�̸� ������ �ڵ����� �����Ѵ�.
	if (CodeFlag == "-c") {
		HuffmanCoding(InputFileName);
	}

	// CodeFlag�� "-d"�̸� ������ �����Ѵ�.
	else if (CodeFlag == "-d") {
		DecodingHuffmanCode(InputFileName);

	}

	// CodeFlag�� �߸� �������� ������ ����ϰ� �����Ѵ�.
	else {
		cerr << "Wrong Flag!" << endl;
		cerr << "Huffman Coding Usage : binaryFile -c inputFile" << endl;
		cerr << "Decoding Huffman Code Usage : binaryFile -d inputFile" << endl;
		exit(-1);
	}

	return 0;
}
