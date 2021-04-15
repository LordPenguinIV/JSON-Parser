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
	lookUpTable["O"]["{"] = "{WP";

	lookUpTable["P"]["}"] = "}";
	lookUpTable["P"]["s"] = "sW:VQ}";

	lookUpTable["Q"]["}"] = "E";
	lookUpTable["Q"][","] = ",WsW:VQ";

	lookUpTable["A"]["["] = "[WB";

	lookUpTable["B"]["{"] = "OWC]";
	lookUpTable["B"]["["] = "AWC]";
	lookUpTable["B"]["]"] = "]";
	lookUpTable["B"]["s"] = "sWC]";
	lookUpTable["B"]["z"] = "zWC]";
	lookUpTable["B"]["t"] = "trueWC]";
	lookUpTable["B"]["f"] = "falseWC]";
	lookUpTable["B"]["n"] = "nullWC]";
	lookUpTable["B"]["w"] = "VC]";

	lookUpTable["C"]["]"] = "E";
	lookUpTable["C"][","] = ",VC";

	lookUpTable["V"]["{"] = "OW";
	lookUpTable["V"]["["] = "AW";
	lookUpTable["V"]["s"] = "sW";
	lookUpTable["V"]["z"] = "zW";
	lookUpTable["V"]["t"] = "trueW";
	lookUpTable["V"]["f"] = "falseW";
	lookUpTable["V"]["n"] = "nullW";
	lookUpTable["V"]["w"] = "WVW";

	lookUpTable["W"]["{"] = "E";
	lookUpTable["W"]["}"] = "E";
	lookUpTable["W"]["["] = "E";
	lookUpTable["W"]["]"] = "E";
	lookUpTable["W"][","] = "E";
	lookUpTable["W"]["s"] = "E";
	lookUpTable["W"]["z"] = "E";
	lookUpTable["W"]["t"] = "E";
	lookUpTable["W"]["f"] = "E";
	lookUpTable["W"]["n"] = "E";
	lookUpTable["W"]["w"] = "w";
	lookUpTable["W"][":"] = "E";

	std::regex stringRegex = std::regex(R"("(([^"\\])|\\["\/bfnrt\\]|\\u[0-9a-fA-F])*")");
	std::regex numberRegex = std::regex(R"(-?([1-9][0-9]*|0)(\.[0-9]+)?([eE][-+]?[0-9]+)?)");
	std::regex whitespaceRegex = std::regex(R"(\s+)");

	std::vector<std::string> files;
	std::string path = "D:\\JSON Files";
	for (auto& entry : std::filesystem::directory_iterator(path))
		files.push_back(entry.path().string());

	for (unsigned int fileNum = 0; fileNum < files.size(); fileNum++) {
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

		try {
			auto start = std::chrono::high_resolution_clock::now();
			fileContent = regex_replace(fileContent, stringRegex, "s");
			fileContent = regex_replace(fileContent, numberRegex, "z");
			fileContent = regex_replace(fileContent, whitespaceRegex, "w");
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
		stackMachine.push('W'); //Incase there is whitespace before or after the object in the file read
		stackMachine.push('O');
		stackMachine.push('W');

		for (unsigned int i = 0; i < fileContent.length(); i++) {
			//Stack machine should not be empty yet
			if (stackMachine.empty()) {
				std::cout << "Stack is empty" << std::endl;
				std::cout << fileContent.substr(0, i) << " ^ " << fileContent.substr(i) << std::endl;
				return 1;
			}

			std::string pushString = "";
			char currentChar = fileContent.at(i);
			char stackTop = stackMachine.top();
			stackMachine.pop();

			while (currentChar != stackTop) {
				//std::cout << "START CHECK: stackTop: " << stackTop << std::endl;
				//std::cout << "START CHECK: currentChar: " << currentChar << std::endl;

				if (lookUpTable[std::string(1, stackTop)].size() == 0) {
					std::cout << "stackTop is not a valid non terminal" << std::endl;
					std::cout << "stackTop: " << stackTop << std::endl;
					std::cout << "currentChar: " << currentChar << std::endl;
					std::cout << fileContent.substr(0, i) << " ^ " << fileContent.substr(i) << std::endl;
					return 1;
				}

				pushString = lookUpTable[std::string(1, stackTop)][std::string(1, currentChar)];
				//std::cout << std::string(1, stackTop) << " -> " << pushString << std::endl;			//HERE IT IS SO YOU CAN SEE IT EVERY TIME YOU GO LOOKING FOR IT

				//The character being read should be apart of the character in the look up table
				if (pushString.length() == 0) {
					std::cout << "currentChar does not exist for stackTop" << std::endl;
					std::cout << "stackTop: " << stackTop << std::endl;
					std::cout << "currentChar: " << currentChar << std::endl;
					std::cout << fileContent.substr(0, i) << " ^ " << fileContent.substr(i) << std::endl;
					return 1;
				}

				//If it's not epsilon, push the string
				if (pushString != "E") {
					for (int j = pushString.length() - 1; j >= 0; j--) {
						stackMachine.push(pushString.at(j));
					}
				}

				//Stack machine still should not be empty yet
				if (stackMachine.empty()) {
					std::cout << "Stack is empty" << std::endl;
					std::cout << fileContent.substr(0, i) << " ^ " << fileContent.substr(i) << std::endl;
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