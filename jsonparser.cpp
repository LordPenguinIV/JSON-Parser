#include <iostream>
#include <regex>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <chrono>

#define EPSILON "E"


int main() {
	//All the transistions in the look up table
	std::map<char, std::map<char, std::string>> lookUpTable;

	//Object start
	lookUpTable['O']['{'] = "{P";

	//Object end
	lookUpTable['P']['}'] = "}";
	lookUpTable['P']['\"'] = "\"S\":VQ}";

	//Object continue
	lookUpTable['Q']['}'] = EPSILON;
	lookUpTable['Q'][','] = ",\"S\":VQ";

	//Array start
	lookUpTable['A']['['] = "[B";

	//Array end
	lookUpTable['B']['{'] = "OC]";
	lookUpTable['B']['['] = "AC]";
	lookUpTable['B'][']'] = "]";
	lookUpTable['B']['\"'] = "\"S\"C]";
	lookUpTable['B']['t'] = "trueC]";
	lookUpTable['B']['f'] = "falseC]";
	lookUpTable['B']['n'] = "nullC]";
	for (char i = '0'; i <= '9'; i++) {
		lookUpTable['B'][i] = "ZC]";
	}
	lookUpTable['B']['-'] = "ZC]";

	//Array continue
	lookUpTable['C'][']'] = EPSILON;
	lookUpTable['C'][','] = ",VC";

	//Value
	lookUpTable['V']['{'] = "O";
	lookUpTable['V']['['] = "A";
	lookUpTable['V']['\"'] = "\"S\"";
	lookUpTable['V']['t'] = "true";
	lookUpTable['V']['f'] = "false";
	lookUpTable['V']['n'] = "null";
	for (char i = '0'; i <= '9'; i++) {
		lookUpTable['V'][i] = "Z";
	}
	lookUpTable['V']['-'] = "Z";

	//String
	lookUpTable['S']['\\'] = "\\HS";
	lookUpTable['S']['\"'] = EPSILON;
	//S -> <everything else>S

	//Backslash in string
	lookUpTable['H']['\"'] = "\"";
	lookUpTable['H']['\\'] = "\\";
	lookUpTable['H']['/'] = "/";
	lookUpTable['H']['b'] = "b";
	lookUpTable['H']['f'] = "f";
	lookUpTable['H']['n'] = "n";
	lookUpTable['H']['r'] = "r";
	lookUpTable['H']['t'] = "t";
	lookUpTable['H']['u'] = "uNNNN";

	//Hexadecimal in string
	for (char i = '0'; i <= '9'; i++) {
		lookUpTable['N'][i] = std::string(1, i);
	}
	for (char i = 'a'; i <= 'f'; i++) {
		lookUpTable['N'][i] = std::string(1, i);
	}
	for (char i = 'A'; i <= 'F'; i++) {
		lookUpTable['N'][i] = std::string(1, i);
	}

	//Number start (Negative)
	lookUpTable['Z']['-'] = "-Y";
	lookUpTable['Z']['0'] = "0IJ";
	for (char i = '1'; i <= '9'; i++) {
		lookUpTable['Z'][i] = std::string(1, i) + "XIJ";
	}

	//Number start
	lookUpTable['Y']['0'] = "0IJ";
	for (char i = '1'; i <= '9'; i++) {
		lookUpTable['Y'][i] = std::string(1, i) + "XIJ";
	}

	//(0-9)+
	for (char i = '0'; i <= '9'; i++) {
		lookUpTable['X'][i] = std::string(1, i) + "X";
	}
	lookUpTable['X'][','] = EPSILON;
	lookUpTable['X'][']'] = EPSILON;
	lookUpTable['X']['}'] = EPSILON;
	lookUpTable['X']['.'] = EPSILON;
	lookUpTable['X']['e'] = EPSILON;
	lookUpTable['X']['E'] = EPSILON;

	//Number decimal
	lookUpTable['I'][','] = EPSILON;
	lookUpTable['I'][']'] = EPSILON;
	lookUpTable['I']['}'] = EPSILON;
	lookUpTable['I']['.'] = ".KX";
	lookUpTable['I']['e'] = EPSILON;
	lookUpTable['I']['E'] = EPSILON;

	//Number Scientific notation
	lookUpTable['J'][','] = EPSILON;
	lookUpTable['J'][']'] = EPSILON;
	lookUpTable['J']['}'] = EPSILON;
	lookUpTable['J']['.'] = ".KX";
	lookUpTable['J']['e'] = "eLKX";
	lookUpTable['J']['E'] = "ELKX";

	//(0-9)*
	for (char i = '0'; i <= '9'; i++) {
		lookUpTable['K'][i] = std::string(1, i);
	}

	//Number Scientific notation sign
	lookUpTable['L']['+'] = "+";
	lookUpTable['L']['-'] = "-";
	for (char i = '0'; i <= '9'; i++) {
		lookUpTable['K'][i] = EPSILON;
	}

	//Finds the path to all files in given directory
	std::vector<std::string> files;
	std::string path = "D:\\JSON Files";
	for (auto& entry : std::filesystem::directory_iterator(path))
		files.push_back(entry.path().string());

	//Processes every single file the was found
	for (unsigned int fileNum = 0; fileNum < files.size(); fileNum++) {
		std::ifstream file;
		std::stack<char> stackMachine;
		char currentChar;
		std::string pushString;
		char stackTop;
		unsigned int column = 1;
		unsigned int line = 1;

		std::cout << fileNum << " / " << files.size() - 1 << " : " << files[fileNum] << std::endl;
		auto start = std::chrono::high_resolution_clock::now();

		//Initializing the stack
		stackMachine.push('V');

		file.open(files[fileNum]);
		while (file.get(currentChar)) {
			pushString = "";

			if (currentChar == '\n') {
				column = 1;
				line++;
			}
			else {
				column++;
			}
			
			//Skip whitespace since it doesn't matter
			if (currentChar == ' ' || currentChar == '\n' || currentChar == '\t' || currentChar == '\r') {
				continue;
			}

			//Stack machine should not be empty until the whole file has been processed
			if (stackMachine.empty()) {
				std::cout << "Stack is empty pt 1" << std::endl;
				std::cout << "Line: " << line << "  Column: " << column << std::endl;
				return 1;
			}

			stackTop = stackMachine.top();
			stackMachine.pop();

			//If stackTop and currentChar are equal, it just gets popped (Special case for 'S' as it can be a nonterminal and terminal char)
			while ((currentChar != stackTop) || (currentChar == 'S' && currentChar == stackTop && stackMachine.top() == '\"')) {

				if (lookUpTable[stackTop].size() == 0) {
					std::cout << "stackTop is not a valid non terminal" << std::endl;
					std::cout << "stackTop: " << stackTop << std::endl;
					std::cout << "currentChar: " << currentChar << std::endl;
					std::cout << "Line: " << line << "  Column: " << column << std::endl;
					return 1;
				}

				pushString = lookUpTable[stackTop][currentChar];
				if (stackTop == 'S' && pushString.length() == 0) {	//S -> <currentChar>S
					pushString = std::string(1, currentChar) + "S";
				}

				//The character being read should be apart of the characters in the look up table
				if (pushString.length() == 0) {
					std::cout << "currentChar does not exist for stackTop" << std::endl;
					std::cout << "stackTop: " << stackTop << std::endl;
					std::cout << "currentChar: " << currentChar << std::endl;
					std::cout << "Line: " << line << "  Column: " << column << std::endl;
					return 1;
				}

				//If it's not EPSILON, push the string
				if (pushString != EPSILON) {
					for (int j = pushString.length() - 1; j >= 0; j--) {
						stackMachine.push(pushString.at(j));
					}
				}

				//Stack machine still should not be empty yet. still processing file
				if (stackMachine.empty()) {
					std::cout << "Stack is empty pt 2" << std::endl;
					std::cout << "Line: " << line << "  Column: " << column << std::endl;
					return 1;
				}

				stackTop = stackMachine.top();
				stackMachine.pop();
			}
		}

		if (stackMachine.empty()) {
			std::cout << "Valid JSON File\n";
		}
		else {
			std::cout << "Invalid JSON File\n";
			return 1;
		}

		auto finish = std::chrono::high_resolution_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms to validate\n";
		std::cout << "\n\n\n\n\n";
		file.close();
	}
}