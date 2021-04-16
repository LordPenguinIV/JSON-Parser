#include <iostream>
#include <regex>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <chrono>


int main() {
	std::map<std::string, std::map<std::string, std::string>> lookUpTable;
	// z : (num)	E : epsilon
	// WAIT A SECOND, WHY ARE THESE MAPS STRINGS WHEN THEY COULD BE CHARS????????
	lookUpTable["O"]["{"] = "{P";

	lookUpTable["P"]["}"] = "}";
	lookUpTable["P"]["\""] = "\"S\":VQ}";

	lookUpTable["Q"]["}"] = "E";
	lookUpTable["Q"][","] = ",\"S\":VQ";

	lookUpTable["A"]["["] = "[B";

	lookUpTable["B"]["{"] = "OC]";
	lookUpTable["B"]["["] = "AC]";
	lookUpTable["B"]["]"] = "]";
	lookUpTable["B"]["\""] = "\"S\"C]";
	lookUpTable["B"]["z"] = "zC]";
	lookUpTable["B"]["t"] = "trueC]";
	lookUpTable["B"]["f"] = "falseC]";
	lookUpTable["B"]["n"] = "nullC]";
	lookUpTable["B"]["w"] = "VC]";

	lookUpTable["C"]["]"] = "E";
	lookUpTable["C"][","] = ",VC";

	lookUpTable["V"]["{"] = "O";
	lookUpTable["V"]["["] = "A";
	lookUpTable["V"]["\""] = "\"S\"";
	lookUpTable["V"]["z"] = "z";
	lookUpTable["V"]["t"] = "true";
	lookUpTable["V"]["f"] = "false";
	lookUpTable["V"]["n"] = "null";
	lookUpTable["V"]["w"] = "V";

	lookUpTable["S"]["\\"] = "\\HS";
	lookUpTable["S"]["\""] = "E";
	//EVERYTHING else goes to E

	lookUpTable["H"]["\""] = "\"";
	lookUpTable["H"]["\\"] = "\\";
	lookUpTable["H"]["/"] = "/";
	lookUpTable["H"]["b"] = "b";
	lookUpTable["H"]["f"] = "f";
	lookUpTable["H"]["n"] = "n";
	lookUpTable["H"]["r"] = "r";
	lookUpTable["H"]["t"] = "t";
	lookUpTable["H"]["u"] = "uNNNN";

	//N goes to 0-9, a-f, and A-F

	//std::regex stringRegex = std::regex(R"("(([^"\\])|\\["\/bfnrt\\]|\\u[0-9a-fA-F])*")");
	std::regex numberRegex = std::regex(R"(-?([1-9][0-9]*|0)(\.[0-9]+)?([eE][-+]?[0-9]+)?)");
	//std::regex whitespaceRegex = std::regex(R"(\s+)");

	std::vector<std::string> files;
	std::string path = "D:\\JSON Files";
	for (auto& entry : std::filesystem::directory_iterator(path))
		files.push_back(entry.path().string());

	for (unsigned int fileNum = 0; fileNum < files.size(); fileNum++) {
		std::ifstream file;
		std::string fileContent = R"()";
		std::string temp;
		std::cout << fileNum << " / " << files.size() - files.size() + 1 << " : " << files[fileNum] << std::endl;
		file.open(files[fileNum]);
		while (!file.eof()) {
			getline(file, temp);
			fileContent += temp + "\n";
		}
		file.close();

		try {
			auto start = std::chrono::high_resolution_clock::now();
			//fileContent = regex_replace(fileContent, stringRegex, "s");
			fileContent = regex_replace(fileContent, numberRegex, "z");
			//fileContent = regex_replace(fileContent, whitespaceRegex, "w");
			auto finish = std::chrono::high_resolution_clock::now();
			std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms to complete regex\n";
		}
		catch (std::regex_error & e) {
			if (e.code() == std::regex_constants::error_stack || e.code() == std::regex_constants::error_complexity) {
				std::cout << "BOOO, QUIT WHINING\n\n\n\n\n";
				continue;
			}
			else {
				std::cout << "Yerr regex be wrong\n";
				return 1;
			}
		}

		auto start = std::chrono::high_resolution_clock::now();
		std::stack<char> stackMachine;
		stackMachine.push('O');
		//stackMachine.push('W');

		for (unsigned int i = 0; i < fileContent.length(); i++) {

			std::string pushString = "";
			char currentChar = fileContent.at(i);	
			char stackTop = stackMachine.top();
			if ((currentChar == ' ' || currentChar == '\n' || currentChar == '\t' || currentChar == '\r') && stackTop != 'S') {
				continue;
			}

			//Stack machine should not be empty yet
			if (stackMachine.empty()) {
				std::cout << "Stack is empty pt 1" << std::endl;
				std::cout << fileContent.substr(0, i) << "  ^  " << fileContent.substr(i) << std::endl;
				return 1;
			}

			stackMachine.pop();

			while (currentChar != stackTop) {

				if (lookUpTable[std::string(1, stackTop)].size() == 0) {
					std::cout << "stackTop is not a valid non terminal" << std::endl;
					std::cout << "stackTop: " << stackTop << std::endl;
					std::cout << "currentChar: " << currentChar << std::endl;
					std::cout << fileContent.substr(0, i) << "  ^  " << fileContent.substr(i) << std::endl;
					return 1;
				}

				pushString = lookUpTable[std::string(1, stackTop)][std::string(1, currentChar)];
				if (stackTop == 'S' && pushString.length() == 0) {
					pushString = std::string(1, currentChar) + "S";
				}
				std::cout << std::string(1, stackTop) << " -> " << pushString << std::endl;			//HERE IT IS SO YOU CAN SEE IT EVERY TIME YOU GO LOOKING FOR IT

				//The character being read should be apart of the character in the look up table
				if (pushString.length() == 0) {
					std::cout << "currentChar does not exist for stackTop" << std::endl;
					std::cout << "stackTop: " << stackTop << std::endl;
					std::cout << "currentChar: " << currentChar << std::endl;
					std::cout << fileContent.substr(0, i) << "  ^  " << fileContent.substr(i) << std::endl;
					return 1;
				}

				//If it's not epsilon, push the string
				if (pushString != "E" && pushString.length() != 0) {
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
			std::cout << "Ya failed :(\n";
			return 1;
		}
		auto finish = std::chrono::high_resolution_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms to complete stack machine\n";
		std::cout << "\n\n\n\n\n";
	}
}