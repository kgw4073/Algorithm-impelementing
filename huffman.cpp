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
// 허프만 트리를 만들기 위한 구조체를 선언.								//
// 1. character, 2. 나온 횟수, 3. leftpointer, 4. rightpointer		//
//																	//
//==================================================================//
struct Node {
	char c;
	int count;
	struct Node *left;
	struct Node *right;
};

// 위 Node 구조체를 priority_queue(min heap)에 담기 위해 Compare 구조체를 선언하고 operator를 재정의 해주었다.

struct Compare {
	bool operator() (const Node* first, const Node* second) {
		return first->count > second->count;
	}
};
// 허프만 트리 생성을 위한 노드 포인터
struct Node* u, * v, * w;
// 각각의 character들에 대해, 파일에 나온 개수를 저장하기 위한 배열
int FrequencyCount[256] = { 0, };
// ASCII CODE에 해당하는 문자에 대한 허프만 코드를 저장하기 위한 string 배열
string BitString[256];
// 트리 노드 포인터를 vector로 저장하며 비교 연산자로 Compare의 연산자를 사용하는 최소 힙을 선언
priority_queue<struct Node*, vector<struct Node*>, Compare> pq;

//dfs로 트리의 리프 노드를 탐색하며 위에서 선언한 BitString 배열의 문자 인덱스에 해당하는 허프만 코드 문자열을 저장한다.
void dfs(struct Node* w, string s) {
	// 아직 리프 노드에 도달하지 않았을 때 계속해서 순회
	if (w->left != NULL || w->right != NULL) {
		// left는 "0"을 추가해서 다음 레벨로 들어가고 right는 "1"을 추가해서 다음 레벨로 들어간다.
		dfs(w->left, s + "0");
		dfs(w->right, s + "1");
	}
	// 리프 노드에 도달했을 때는 해당하는 문자의 ASCII CODE값에 해당하는 BitString 인덱스에 인자로 넘어온 문자열을 저장.
	else {
		BitString[w->c] = s;
	}
}

// 허프만 트리에 동적 할당된 노드를 해제하기 위함
void DeallocationTree(struct Node* w) {
	// 왼쪽 서브 트리를 해제
	if (w->left) {
		DeallocationTree(w->left);
	}
	// 오른쪽 서브 트리를 해제
	if (w->right) {
		DeallocationTree(w->right);
	}
	delete w;
}

// 이 프로그램에 동적 할당된 메모리를 해제하기 위한 함수.
// 여기서는 허프만 트리뿐이므로 Tree만 해제함.
void DeallocationMemory(struct Node* w) {
	DeallocationTree(w);
}

//==============================================================================================//
// Huffman Tree Making Function																	//						
// parameter : LeafNodeCount(리프 노드의 개수, 즉, 파일에 나타난 문자의 개수)						//
// 																								//
// priority_queue를 이용하여 정해진 알고리즘에 따라 허프만 트리를 생성한다.							//
//																								//
// execution result : w가 허프만 트리의 root node가 된다.											//
//																								//																		//
//==============================================================================================//
void MakeTree(int LeafNodeCount) {

	// LeafNodeCount-1번 만큼의 루프를 돈다.
	// 그 이유는 트리를 만드는 과정이 heap에 저장된 것에서 두 노드를 한꺼번에 pop하고 이 둘을 새로운 노드 w의 자식으로 두기 때문이다.
	// 따라서 LeafNodeCount - 1번까지 루프를 돌고 탈출한 후에 pq에 남아있는 한 개의 노드에는 모든 문자가 등장한 횟수의 총합이 저장되어 있을 것이다.
	for (int i = 0; i < LeafNodeCount - 1; i++) {
		// min heap에서 노드 두 개를 꺼낸다.
		u = pq.top();
		pq.pop();
		v = pq.top();
		pq.pop();

		// w에 새로운 노드를 동적 할당한다.
		w = new struct Node();
		// w의 left에는 u, right에는 v로 지정하고 count는 두 노드의 count의 합을 할당한다.
		w->left = u;
		w->right = v;
		w->count = u->count + v->count;
		// 이것을 다시 pq에 삽입한다.
		pq.push(w);
	}
	w = pq.top();
	pq.pop();
	// 만들어진 허프만 트리의 루트 w를 dfs함수를 호출하여 각 리프 노드에 저장된 문자에 해당하는 허프만 코드 문자열을 BitString에 저장한다.
	dfs(w, "");
}

//==============================================================================================//
// Do Huffman Coding Function																	//
// parameter : File Name 프로그램 실행시 넘겨받은 인자인 input File Name을 인자로 받는다.			//
// 																								//
// 위에서 정의한 MakeTree() function이나 dfs() function을 사용하여 허프만 코딩을 진행한다.			//
//																								//
// execution result : FileName.zz에 허프만 코딩된 문자열이 저장된다. 다만, Decoding의 편의성을 위해 //
//					  input file에 등장한 문자와, 등장 횟수를 저장하는 부분을 추가하였다.			//
//																								//
//==============================================================================================//
void HuffmanCoding(const char* FileName) {

	// FullBitCount는 input file을 비트로 변환했을 때 비트 문자열의 총 길이를 나타낸다.
	int FullBitCount = 0, LeafNodeCount = 0;
	;
	// c는 문자를 받을 임시 변수
	char c;
	// InputFullString은 압축할 파일에 저장된 문자열을 저장하며, InputBitString은 저장된 InputFullString을 허프만 코드로 변환시키고 저장할 문자열 객체이다.
	string InputFullString = "";
	// Huffman Coding 규칙에 따라 변환된 문자열을 결과 파일에 저장할 문자열 객체를 선언.
	string StringForEncoding;
	StringForEncoding.resize(1, 0);
	// FileName으로 파일 입력 스트림을 생성.
	ifstream TextFileIn(FileName, ios::binary);

	// Input file이 존재하지 않거나 파일 스트림이 열리지 않으면 오류 메시지를 출력하고 프로그램을 종료한다.
	if (!TextFileIn.is_open()) {
		cerr << "Input file does not exist!" << endl;
		exit(-1);
	}

	// OutputFileName은 FileName에 ".zz"를 붙여야 한다.
	string OutputFileName = string(FileName) + ".zz";

	//OutputFileName을 가지고 binary모드 파일 출력 스트림을 생성한다.
	ofstream EncodingOut(OutputFileName.c_str(), ios::binary);

	// 파일 입력 스트림의 get 메소드를 사용하여 한 문자씩 읽어들임.
	while (TextFileIn.get(c)) {
		// InputFullString에 읽어들인 문자를 string 객체로 변환하여 더한다.
		InputFullString += c;
		// 읽어들인 문자의 등장 횟수를 증가시킨다.
		FrequencyCount[c]++;
	}
	
	// 파일 입력 스트림을 닫는다.
	TextFileIn.close();

	// LeafNodeCount는 이 파일에 문자의 종류가 얼마나 되는지 저장하는 변수이다.
	for (int i = 0; i < 256; i++) {
		// 한 번도 등장하지 않았으면 continue
		if (!FrequencyCount[i]) continue;
		LeafNodeCount++;
	}
	// LeafNodeCount도 압축된 파일에 저장
	EncodingOut << LeafNodeCount << '\n';

	// 압축된 파일에, input file에 등장한 문자와 그 문자가 등장한 개수를 저장하기 위한 루프이다.
	for (int i = 0; i < 256; i++) {
		if (!FrequencyCount[i]) continue;

		struct Node* pushed = new struct Node();
		pushed->c = char(i);
		pushed->count = FrequencyCount[i];
		pushed->left = NULL;
		pushed->right = NULL;

		EncodingOut << pushed->c << ' ' << pushed->count << '\n';

		// Tree를 만들기 위해 전역 변수로 선언한 priority_queue에 저장한다.
		pq.push(pushed);
	}

	// pq에 저장된 정보를 바탕으로 허프만 트리를 만든다. 내부에서 dfs를 수행하며 각 문자에 해당하는 문자열도 BitString에 저장한다.
	MakeTree(LeafNodeCount);

	// InputBitString에 InputFullString의 한 문자에 해당하는 비트 문자열을 추가한다.
	for (int i = 0, j = 7; i < InputFullString.size(); ) {

		// 현재 탐색하고 있는 InputFullString의 i번째 인덱스 문자를 c에 두고
		// 이 문자에 해당하는 BitString을 가지고 StringForEncoding에 바로 Encoding한다.
		char c = InputFullString[i];
		for (int k = 0; k < BitString[c].size(); k++) {
			// j가 0보다 작아지면 다시 7로 업데이트 해준다. char는 8bit이기 때문
			if (j < 0) j = 7;

			// c의 BitString 중 현재 값이 '1'이면 LSB로부터 j번째 비트를 set한다.
			if (BitString[c][k] == '1') {
				StringForEncoding[FullBitCount / 8] |= (1 << j);
			}
			
			// FullBitCount는 총 Bit의 길이를 세는 변수로 8로 나눈 나머지가 7이면 다음 공간이 필요하므로 '\0' 삽입
			if (FullBitCount % 8 == 7) {
				StringForEncoding += string(1, 0);
			}

			// j는 LSB로부터 떨어진 정도이므로 점점 줄여나가야 한다.
			j--;
			FullBitCount++;
		}
		i++;
	}

	// InputBitString의 길이를 저장
	EncodingOut << FullBitCount << '\n';
	// 압축이 완료된 문자열을 저장.
	EncodingOut << StringForEncoding;
	
	// 출력 파일 스트림을 해제한다.
	EncodingOut.close();
	// 트리를 해제한다.
	DeallocationMemory(w);
}

//===============================================================================================//
// Decoding Huffman Code																		 //
// parameter : File Name 프로그램 실행시 넘겨받은 인자인 input File Name을 인자로 받는다.			 //
// 																								 //
// 위에서 정의한 MakeTree() function이나 dfs() function을 이용하여 허프만 트리를 재구성하고 이를 통해 //											//
// 만들어진 허프만 트리의 탐색을 통해 문자를 재구성한다.												 //
//																								 //
// execution result : FileName.yy에 압축이 해제된 문자열이 저장된다.								 //
//																								 //
//===============================================================================================//
void DecodingHuffmanCode(const char* FileName) {

	// NodeCount는 압축 파일의 처음에 저장된 숫자로, 등장한 문자의 종류의 개수이다.
	// BitStringLength은 압축을 풀어야 할 BitString의 길이를 저장한다.
	int NodeCount, BitStringLength;

	// start와 end는 파일 스트림에서 압축을 해제할 문자열이 시작되는 지점과 끝을 담기 위한 변수
	int start, end;
	// FileNameString은 FileName을 string 객체로 바꾼 것이다.
	string FileNameString = string(FileName);
	// StringForDecoding는 압축을 풀기 위해 input file에 저장된 허프만 코드 문자열을 담는 string 변수이다.
	string StringForDecoding = "";
	// FileName을 binary모드로 여는 입력 스트림을 생성한다.
	ifstream DecoderIn(FileName, ios::binary);

	// 입력 스트림이 생성되지 않으면 에러를 출력하고 프로그램을 종료한다.
	if (!DecoderIn.is_open()) {
		cerr << "Huffman Coded File does not exist!" << endl;
		exit(-1);
	}

	DecoderIn >> NodeCount;

	// NodeChar와 CharFrequency는 입력을 받기 위해 임시로 선언한 변수이다.
	char NodeChar;
	int CharFrequency;

	// '\n' 제거
	DecoderIn.get(NodeChar);

	// min heap을 비워준다.
	pq.empty();

	// min heap에다가, 등장한 문자와 해당 문자의 개수를 저장하기 위한 loop이다.
	for (int i = 0; i < NodeCount; i++) {
		// 파일 입력 스트림으로 문자 하나를 읽는다.
		DecoderIn.get(NodeChar);
		// 읽은 문자의 등장 횟수를 읽는다.
		DecoderIn >> CharFrequency;

		// 이 정보를 가지고 노드를 생성하여 min heap에 저장한다. (추후 허프만 트리를 다시 만들기 위함이다.)
		struct Node* pushed = new struct Node;
		pushed->c = NodeChar;
		pushed->count = CharFrequency;
		pushed->left = NULL;
		pushed->right = NULL;
		pq.push(pushed);
		
		// '\n' 제거
		DecoderIn.get(NodeChar);
	}

	// 풀어야 할 BitString의 길이를 입력받는다.
	DecoderIn >> BitStringLength;
	// '\n' 제거
	DecoderIn.get(NodeChar);

	// Huffman Coding function에서의 MakeTree와 완전히 동일하게 허프만 트리를 재구성한다.
	MakeTree(NodeCount);

	// C++의 파일 스트림 메소드를 활용한다. 
	// tellg()는 현재 파일 스트림의 위치를 반환한다.
	start = DecoderIn.tellg();
	// seekg(OFFSET, point)이다. point로부터 OFFSET만큼의 위치로 이동한다.
	DecoderIn.seekg(0, ios::end);
	
	// 입력 파일의 가장 끝에 파일 스트림의 위치를 end에 저장한다.
	end = DecoderIn.tellg();

	// StringForDecoding은 압축 파일에 저장된 문자를 저장하기 위함이다. end-start만큼의 길이를 갖는다.
	StringForDecoding.resize(end - start);

	// 다시 파일의 처음으로부터 start만큼 떨어진 곳으로 스트림의 위치를 변경한다.
	DecoderIn.seekg(start, ios::beg);
	// StringForDecoding[0]의 위치에 end-start길이만큼 읽어들인다.
	DecoderIn.read(&StringForDecoding[0], end - start);
	// 파일 입력 스트림을 해제한다.
	DecoderIn.close();

	// DecodedFullString, 즉, 압축 해제 후 문자열을 저장하기 위한 string 객체이다.
	string DecodedFullString = "";

	// DecodedBitString의 한 문자 한 문자 검색하며 진행한다. 다만 for문에서 증감식은 없고 while문 안에 추가하였다.
	for (int i = 0, j = 7; i < BitStringLength; ) {
		// w에는 허프만 트리가 저장되어 있다. 문자를 한 번 제대로 찾아낼 때마다 ptr을 w로 다시 바꾸어 주어야 한다.
		struct Node* ptr = w;
		// ptr과 DecodedBitString의 문자를 가지고 어떤 문자인지 탐색해나간다.
		while (true) {
			if (j < 0) j = 7;
			// 허프만 트리는 리프 노드가 아니면 반드시 두 개의 자식 노드가 존재한다.
			if (ptr->left && ptr->right) {
				// 현재 DecodedBitString의 i / 8번째 인덱스에 LSB로부터 j번째 비트가 0이면 left로, 1이면 right로 들어간다. (이는 허프만 트리를 생성할 때 그렇게 만들었기 때문이다.)
				if (StringForDecoding[i / 8] & (1 << j)) {
					ptr = ptr->right;
				}
				else ptr = ptr->left;
				// i를 하나 증가시킨다.
				i++;
				j--;
			}
			// 리프 노드이므로 while문을 탈출한다.
			else break;
		}
		// 현재 ptr이 가리키고 있는 문자를 DecodedFullString에 더한다.
		DecodedFullString += ptr->c;
	}
	
	 
	// 압축 해제시 DecodedBitString의 사이즈가 0이면, 즉, 원본에 한 글자만 있어서
	// 이 문자에 해당하는 비트가 NULL일 경우 위 for loop를 돌지 못한다. 따라서
	// 굳이 트리를 탐색할 필요가 없을 경우 예외처리를 해주었다. 이렇게 하면
	// 한 글자만 있을 때 압축 파일의 크기가 매우 작아진다.
	if (BitStringLength == 0) {
		for (int i = 0; i < w->count; i++) {
			DecodedFullString += string(1, w->c);
		}
	}

	// HuffmanDecodedFile은 FileNameString에 ".yy"를 추가한 것이다.
	string HuffmanDecodedFile = FileNameString + ".yy";

	// HuffmanDecodedFile으로 파일 출력 스트림을 생성하고 압축 해제가 완료된 문자열을 출력한다.
	ofstream DecoderOut(HuffmanDecodedFile.c_str(), ios::binary);
	DecoderOut << DecodedFullString;
	DecoderOut.close();
	

	//===================================================================================================
	// inputfile과 Decoded string의 차이를 보기 위해 삽입한 임시 코드
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
	
	
	// 허프만 트리를 해제한다.
	DeallocationMemory(w);
}


int main(int argc, char* argv[]) {
	// CodeFlag는 압축을 할지 해제를 할지 나타내는 문자열 "-c" | "-d"을 담는 문자열
	string CodeFlag = argv[1];
	// InputFileName은 세 번째 인자로 들어옴
	const char* InputFileName = argv[2];

	// 인자가 세 개가 아니면 잘못 입력한 것이므로 에러를 출력하고 종료한다.
	if(argc != 3) {
		cerr << "Wrong Usage!" << endl;
		cerr << "Usage : binaryFile flag(-c or -d) inputFile" <<endl;
		exit(-1);
	}

	// CodeFlag가 "-c"이면 허프만 코딩으로 압축한다.
	if (CodeFlag == "-c") {
		HuffmanCoding(InputFileName);
	}

	// CodeFlag가 "-d"이면 압축을 해제한다.
	else if (CodeFlag == "-d") {
		DecodingHuffmanCode(InputFileName);

	}

	// CodeFlag가 잘못 들어왔으면 에러를 출력하고 종료한다.
	else {
		cerr << "Wrong Flag!" << endl;
		cerr << "Huffman Coding Usage : binaryFile -c inputFile" << endl;
		cerr << "Decoding Huffman Code Usage : binaryFile -d inputFile" << endl;
		exit(-1);
	}

	return 0;
}
