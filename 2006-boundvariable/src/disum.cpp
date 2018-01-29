#include <exception>
#include <iomanip>
#include <iostream>

#include "um.hpp"

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "Syntax: " << argv[0] << " <scroll>\n";
		return 1;
	}

	try {
		auto scroll = read_scroll(argv[1]);

		for (const auto& platter : scroll) {
			// std::cout << '\t' << platter.sop.op << '\n';

			auto instruction = Instruction::decode(platter);
			std::cout << std::setw(8) << std::setbase(16) << platter << '\t';

			if (instruction.op == 0) {
				std::cout << "cmov r" << instruction.a << ", r" << instruction.b << ", r" << instruction.c << '\n';
			}
			else if (instruction.op == 1) {
				std::cout << "r" << instruction.a << " = r" << instruction.b << "[r" << instruction.c << "]\n";
			}
			else if (instruction.op == 2) {
				std::cout << "r" << instruction.a << "[r" << instruction.b << "] = r" << instruction.c << '\n';
			}
			else if (instruction.op == 3) {
				std::cout << "r" << instruction.a << " = r" << instruction.b << " + r" << instruction.c << '\n';
			}
			else if (instruction.op == 4) {
				std::cout << "r" << instruction.a << " = r" << instruction.b << " * r" << instruction.c << '\n';
			}
			else if (instruction.op == 5) {
				std::cout << "r" << instruction.a << " = r" << instruction.b << " / r" << instruction.c << '\n';
			}
			else if (instruction.op == 6) {
				std::cout << "r" << instruction.a << " = !(r" << instruction.b << " & r" << instruction.c << ")\n";
			}
			else if (instruction.op == 7) {
				std::cout << "halt\n";
			}
			else if (instruction.op == 8) {
				std::cout << "r" << instruction.b << " = ulloc r" << instruction.c << '\n';
			}
			else if (instruction.op == 9) {
				std::cout << "ufree r" << instruction.c << '\n';
			}
			else if (instruction.op == 10) {
				std::cout << "out r" << instruction.c << '\n';
			}
			else if (instruction.op == 11) {
				std::cout << "in r" << instruction.c << '\n';
			}
			else if (instruction.op == 12) {
				std::cout << "load r" << instruction.b << ", r" << instruction.c << '\n';
			}
			else if (instruction.op == 13) {
				std::cout << "r" << instruction.a << " = " << instruction.value << '\n';
			}
			else {
				std::cout << "*** unknown operator " << instruction.op << '\n';
			}
		}
	}
	catch (const std::system_error& se) {
		std::cerr << "error " << se.code() << ": " << se.what() << '\n';
		return 2;
	}
	catch (const std::exception& ex) {
		std::cerr << "exception " << ex.what() << '\n';
		return 2;
	}

	return 0;
}
