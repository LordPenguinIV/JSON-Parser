#include <iostream>
#include <regex>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <chrono>

constexpr auto EPSILON = "E";


int main() {
	std::map<char, std::map<char, std::string>> lookUpTable;
	lookUpTable['O']['{'] = "{P";

	lookUpTable['P']['}'] = "}";
	lookUpTable['P']['\"'] = "\"S\":VQ}";

	lookUpTable['Q']['}'] = EPSILON;
	lookUpTable['Q'][','] = ",\"S\":VQ";

	lookUpTable['A']['['] = "[B";

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

	lookUpTable['C'][']'] = EPSILON;
	lookUpTable['C'][','] = ",VC";

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

	lookUpTable['S']['\\'] = "\\HS";
	lookUpTable['S']['\"'] = EPSILON;
	//EVERYTHING else goes to <whatever char I found>S   

	lookUpTable['H']['\"'] = "\"";
	lookUpTable['H']['\\'] = "\\";
	lookUpTable['H']['/'] = "/";
	lookUpTable['H']['b'] = "b";
	lookUpTable['H']['f'] = "f";
	lookUpTable['H']['n'] = "n";
	lookUpTable['H']['r'] = "r";
	lookUpTable['H']['t'] = "t";
	lookUpTable['H']['u'] = "uNNNN";

	for (char i = '0'; i <= '9'; i++) {
		lookUpTable['N'][i] = std::string(1, i);
	}
	for (char i = 'a'; i <= 'f'; i++) {
		lookUpTable['N'][i] = std::string(1, i);
	}
	for (char i = 'A'; i <= 'F'; i++) {
		lookUpTable['N'][i] = std::string(1, i);
	}

	lookUpTable['Z']['-'] = "-Y";
	lookUpTable['Z']['0'] = "0IJ";
	for (char i = '1'; i <= '9'; i++) {
		lookUpTable['Z'][i] = std::string(1, i) + "XIJ";
	}

	lookUpTable['Y']['0'] = "0IJ";
	for (char i = '1'; i <= '9'; i++) {
		lookUpTable['Y'][i] = std::string(1, i) + "XIJ";
	}

	for (char i = '0'; i <= '9'; i++) {
		lookUpTable['X'][i] = std::string(1, i) + "X";
	}
	lookUpTable['X'][','] = EPSILON;
	lookUpTable['X'][']'] = EPSILON;
	lookUpTable['X']['}'] = EPSILON;
	lookUpTable['X']['.'] = EPSILON;
	lookUpTable['X']['e'] = EPSILON;
	lookUpTable['X']['E'] = EPSILON;

	lookUpTable['I'][','] = EPSILON;
	lookUpTable['I'][']'] = EPSILON;
	lookUpTable['I']['}'] = EPSILON;
	lookUpTable['I']['.'] = ".KX";
	lookUpTable['I']['e'] = EPSILON;
	lookUpTable['I']['E'] = EPSILON;

	lookUpTable['J'][','] = EPSILON;
	lookUpTable['J'][']'] = EPSILON;
	lookUpTable['J']['}'] = EPSILON;
	lookUpTable['J']['.'] = ".KX";
	lookUpTable['J']['e'] = "eLKX";
	lookUpTable['J']['E'] = "ELKX";

	for (char i = '0'; i <= '9'; i++) {
		lookUpTable['K'][i] = std::string(1, i);
	}

	lookUpTable['L']['+'] = "+";
	lookUpTable['L']['-'] = "-";
	for (char i = '0'; i <= '9'; i++) {
		lookUpTable['K'][i] = EPSILON;
	}

	std::vector<std::string> files;
	std::string path = "D:\\JSON Files";
	for (auto& entry : std::filesystem::directory_iterator(path))
		files.push_back(entry.path().string());

	for (unsigned int fileNum = 0; fileNum < files.size(); fileNum++) {											//HERE IS WHERE YOU CHANGE WHICH FILES YOU ITERATE THROUGHT		files.size()
		std::ifstream file;
		std::string fileContent = R"()";
		std::string temp;
		std::cout << fileNum << " / " << files.size() - 1 << " : " << files[fileNum] << std::endl;
		file.open(files[fileNum]);
		while (!file.eof()) {
			getline(file, temp);
			fileContent += temp + "\n";
		}
		file.close();

		auto start = std::chrono::high_resolution_clock::now();
		std::stack<char> stackMachine;
		stackMachine.push('V');

		for (unsigned int i = 0; i < fileContent.length(); i++) {

			std::string pushString = "";
			char currentChar = fileContent.at(i);
			
			//Skip whitespace unless I'm in the middle of a string
			//Hmmm, or I can actually just always skip it...
			if ((currentChar == ' ' || currentChar == '\n' || currentChar == '\t' || currentChar == '\r') ) {
				continue;
			}
			char stackTop = stackMachine.top();

			//Stack machine should not be empty yet
			if (stackMachine.empty()) {
				std::cout << "Stack is empty pt 1" << std::endl;
				std::cout << fileContent.substr(0, i) << "  ^  " << fileContent.substr(i) << std::endl;
				return 1;
			}

			stackMachine.pop();

			while ((currentChar != stackTop) || (currentChar == 'S' && currentChar == stackTop && stackMachine.top() == '\"')) {

				if (lookUpTable[stackTop].size() == 0) {
					std::cout << "stackTop is not a valid non terminal" << std::endl;
					std::cout << "stackTop: " << stackTop << std::endl;
					std::cout << "currentChar: " << currentChar << std::endl;
					std::cout << fileContent.substr(0, i) << "  ^  " << fileContent.substr(i) << std::endl;
					return 1;
				}

				pushString = lookUpTable[stackTop][currentChar];
				if (stackTop == 'S' && pushString.length() == 0) {	//S -> (currentChar)S
					pushString = std::string(1, currentChar) + "S";
				}
				//std::cout << std::string(1, stackTop) << " -> " << pushString << std::endl;						//HERE IT IS SO YOU CAN SEE IT EVERY TIME YOU GO LOOKING FOR IT

				//The character being read should be apart of the character in the look up table, and if pushString is empty, then it isn't
				if (pushString.length() == 0) {
					std::cout << "currentChar does not exist for stackTop" << std::endl;
					std::cout << "stackTop: " << stackTop << std::endl;
					std::cout << "currentChar: " << currentChar << std::endl;
					std::cout << fileContent.substr(0, i) << "  ^  " << fileContent.substr(i) << std::endl;
					return 1;
				}

				//If it's not epsilon, push the string
				if (pushString != EPSILON) {
					for (int j = pushString.length() - 1; j >= 0; j--) {
						stackMachine.push(pushString.at(j));
					}
				}

				//Stack machine still should not be empty yet
				if (stackMachine.empty()) {
					std::cout << "Stack is empty pt 2" << std::endl;
					std::cout << fileContent.substr(0, i) << "  ^  " << fileContent.substr(i) << std::endl;
					return 1;
				}

				stackTop = stackMachine.top();
				stackMachine.pop();
			}
		}

		if (stackMachine.empty()) {
			std::cout << "Congrats, this one works\n";
		}
		else {
			std::cout << "This one failed :(\n";
			return 1;
		}
		auto finish = std::chrono::high_resolution_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms to complete stack machine\n";
		std::cout << "\n\n\n\n\n";
	}
}