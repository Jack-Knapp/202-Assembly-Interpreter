/* 
    Name: Jack Knapp, 202.1
    Description: A VM that converts and runs simple assembly programs
    Input: files to be converted and run
    Output: Values and output based on the given assembly files
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using std::string;

// Global constant variables
const int LABEL_COUNT = 100; 	// default number of labels
const int OPCODE_COUNT = 18; 	// default number of opcodes
const int MAX_CHARS = 5;		// maximum number of characters
const int STACK_SIZE = 1000;	// maximum capacity of array
const string OPCODE_LIST = "const get put ld st add sub mul div\
 cmp jpos jz j jl jle jg jge halt";     //list of all valid assembly cmds

// STRUCTS
struct Symbol {
    std::string name;
    int mem;
};

struct Opcode {
    std::string name;
    int code;
};

/// splitOpcodes: Splits Opcode Const into codes and values, 
/// 	then loads them into struct array for further use
/// @param inst the string of opcodes
/// @param opcodes the array of opcode struct to load 
/// @return n/a is void
void splitOpcodes(std::string inst, Opcode* opcodes) {
    int breakPoint = 0;
    std::string code = "";
    for (int i = 0; i< OPCODE_COUNT; i++) {
        if(inst.find(" ") != std::string::npos) { 
        breakPoint = inst.find(" ");
        code =inst.substr(0,breakPoint);
        inst = inst.substr(breakPoint+1, inst.size());

        opcodes[i].name = code;
        opcodes[i].code = i;
        }
        else{
            opcodes[i].name = inst;
            opcodes[i].code = i;
        } 
    }
}

/// isOpcode: Takes a given string and checks whether it is in 
/// 	the opcode struct to see if it is a valid code
/// @param word the string to check
/// @param opcodes the array of opcode struct to check against
/// @return a bool reflecting validity
bool isOpcode(std::string& word, Opcode* opcodes) {
    for (int i=0; i < OPCODE_COUNT; i++) {
        if (opcodes[i].name == word) {
            return true;
        }
    }
    return false;
}

/// getOpcode: Takes a given opcode name, then searches opcode array 
/// 	to find its corresponding value
/// @param word the opcode to search for
/// @param opcodes the array of opcode struct to search through
/// @return an opcode value or -1 in case of failure
int getOpcode(std::string& word, Opcode* opcodes) {
   for (int i=0; i < OPCODE_COUNT; i++) {
        if (opcodes[i].name == word) {
            return opcodes[i].code;
        }
    }
    return -1;
}

/// getLocation: Takes a given label, then searches symbol array 
/// 	to find its location value
/// @param name the label to search for
/// @param labels the array of symbol struct to search through
/// @param nextLabel range of the array to search through
/// @return a label address or -1 in case of failure
int getLocation(std::string& name, Symbol* labels, int nextLabel) {
    for (int i = 0; i < nextLabel; i++) {
        if (labels[i].name == name) {
            return labels[i].mem;
        }
    }
    return -1;
}

/// isNumber: Takes a given string, checks whether it can be converted 
/// 	to a number or not
/// @param str the string to be checked
/// @return a bool showing whether the string is a num
bool isNumber(std::string& str) {
    bool isNum = true;
    for (int i=0; i < str.size(); i++) {
        if (str[i] <= 57 && str[i] >=48) {
            continue;
        }
        else {
            isNum = false;
            break;
        }
    }
    return isNum;
}

//Function descriptions for prototypes can be found 
//	with their definitions, after main
int firstPass(Symbol*, std::string);
void secondPass(Symbol*, Opcode*, int*, int);
void thirdPass(Symbol*, Opcode*, int*);


// Helper functions

/// Returns the number of spaced needed for padding.
/// @param str the string that needs padding.
/// @param len the maximum number of characters.
/// @return the number of spaces needed.
//int padding(std::string str, int len) {
//	int n = static_cast<int>(str.length());
//	return len + abs(len - n);
//}

/// Dumps the name and numerical value that each instrcution represents
/// @param op the array of opcodes.
/// @param count the number of opcodes.
/// @return void.
//void dumpOpcodes(Opcode *opcodes, int count) {
//	std::cout << "Opcodes" << std::endl;
//	int pad = 0;
//	for (int i = 0; i < count; ++i) {
//		pad = padding(opcodes[i].name, MAX_CHARS);
//		std::cout << "> " << opcodes[i].name 
//		<< std::string(pad, ' ') << opcodes[i].code << std::endl;
//	}
//}

/// Dumps the name of a label and its corresponding location in memory.
/// @param sym an array of symbols.
/// @param count the number of symbols in the array.
/// @return void.
//void dumpSymbols(Symbol* labels, int count) {
//	std::cout << "Symbols" << std::endl;
//	int pad = 0;
//	for (int i = 0; i < count; ++i) {
//		pad = padding(labels[i].name, MAX_CHARS);
//		std::cout << "> " << "[" << labels[i].name 
//		<< "] = " << labels[i].mem << std::endl; 
//	}
//}

/// Dumps the address of each instruction and label.
/// @param memory the array of addresses.
/// @param count the number of addresses.
/// @return void.
//void dumpMemory(int* memory, int count) {
//	std::cout << "Memory" << std::endl;
//	std::cout.fill('0');
//	for (int i = 0; i < count; ++i) {
//		std::cout << std::setw(8) << memory[i] << std::endl;
//	}
//}

/// Entry point of the main program.
/// @param argc the number of command-line arguments.
/// @param argv the vector of command-line arguments.
/// @return 0 if the program runs to completion.
int main(int argc, char *argv[]) {

	//Initializing arrays
	Symbol symbol[LABEL_COUNT];
    Opcode opcodes[OPCODE_COUNT];
    int memory[STACK_SIZE];

    //Creating the Table of Opcodes 
    splitOpcodes(OPCODE_LIST, opcodes);

	// check command-line arguments
	for (int i = 1; i < argc; i++) {
        std::string filename = argv[i];

		//--------------------------------------------
        //Performing the first pass
		// (1) Removing comments
		// (2) Remember labels
		// (3) Save instruction/opcode and address
		// (4) Write generated code to an output file
        int lineNum = firstPass(symbol, filename);

		// --------------------------------------------
        //Performing the second pass
		// (1) Read output file
		// (2) Replace numeric values and pack them into
        secondPass(symbol, opcodes, memory, lineNum);

		// --------------------------------------------
        //Performing the third pass
		// (1) Execute instructions in sequential order
        thirdPass(symbol, opcodes, memory);
    }

	//return value	
	return EXIT_SUCCESS;
}

/// firstPass: Performs all tasks needed for the first pass.
/// 	It strips col 1 and stores it in the label array 
///		as well as removing ws, comments, and empty lines
/// @param symbol the array of symbol struct to search through
/// @param filename the file to open and parse
/// @return an int recording the number of lines in the revised file
int firstPass(Symbol* symbol, std::string filename) {
	//Initializing some vars
    std::string line = "", lineStart = "";
    std::string fileContents[100];
	int breakp = 0, lineNum = 0;

    std::ifstream ifile;
    std::ofstream ofile;
    std::stringstream stringstre;

	//Program Running Notice
	std::cout << "Running program..." << std::endl;

    //open test file
    ifile.open(filename);
    while (getline(ifile, line)) {
        
        //stripping comment lines and blank lines
        if (line[0] == ';' || line.size() == 0) {
            continue;
        }

        else {
            //stripping midline comments
            while (line.find(";") != std::string::npos) {
            int breakp = line.find(";");   
            line = line.substr(0, breakp);
            } 

            //removing ws if col 1 has no label
            if (line[0] == ' ' || line[0] == '\t'){
                while(line[0] == ' ' || line[0] == '\t'){
                    line = line.substr(1, line.size());
                }
            }
            //stripping and recording labels for col1
            else {
                for(int i=0; i <= line.size(); i++){
                    //finding first whitespace after label
                    if (line[i] == ' ' || line[i] == '\t') {
                        breakp = i;
                        break;
                    }
                }
                //recording label name and location to array
                symbol[lineNum].name = line.substr(0, breakp);
                symbol[lineNum].mem = lineNum;

                //removing remaining col1 whitespace
                line = line.substr(breakp+1, line.size());
                    while(line[0] == ' ' || line[0] == '\t'){
                        line = line.substr(1, line.size());
                    }
            }

            breakp = -1;
            //stripping whitespace from col2 and col3
            for(int i=0; i <= line.size(); i++){
                //finding first whitespace after col2
                if (line[i] == ' ' || line[i] == '\t') {
                    breakp = i;
                    break;
                }
            }

            //If there is no col2 ws, save line to array
            if (breakp == -1) {
                fileContents[lineNum] = line;
                lineNum++;
                continue;
            }

            //Checking whether col3 is empty
            for(int i=breakp; i < line.size(); i++){
                //finding first whitespace after col2
                if (line[i] == ' ' || line[i] == '\t') {
                    continue;
                }
                else {
                    breakp = -1;
                    break;
                }
            }
            
            //removing remaining col2 + col3 whitespace
            stringstre.str(line);
            stringstre >> lineStart >> line;
            stringstre.clear();

			//setting the line value based on if col3 has a label or not 
            if (breakp == -1) {
                line = lineStart + ' ' + line;
            }
            else {
                line = lineStart;
            }
        
        }
            //storing formatted lines in array
            fileContents[lineNum] = line; 

		//iterate line counter
        lineNum++;
    }
    ifile.close(); 

    //Saving stripped data to output file
    ofile.open("output.txt");
    for (unsigned int i=0; i< 100; i++){
        if (fileContents[i].length() != 0) {
            ofile << fileContents[i] << "\n";
        }
    }
    ofile.close();

	//return line counter for use in second pass
    return lineNum;
}

/// secondPass: Performs all tasks needed for the second pass.
/// 	It initializes the memory array and converts opcodes
///		and symbols to memory codes and stores them in memory
/// @param symbol the array of symbol struct to search through
/// @param opcodes the array of opcode strcut to search through
/// @param memory the array of memory to store data in
/// @param lineNum the count of lines in the file to convert
/// @return N/A void
void secondPass(Symbol* symbol, Opcode* opcodes,
int* memory, int lineNum) {
	//Initializing some vars
    std::string line = "", sym = "", opc = "";
    int breakp = 0, symInt = 0, opcInt = 0;

    std::ifstream ifile;
    std::stringstream stringstre;
	
	//initializing the memory array
    for (int i= 0; i < STACK_SIZE; i++) {
        memory[i] = 6174;
    }
	
	//beginning of conversion loop
    ifile.open("output.txt");
    int loopc = 0;
    while (getline(ifile, line)) {
		//store words from line in strings
        stringstre.str(line);
        stringstre >> opc >> sym;
        stringstre.clear();

		//check whether first string is an opcode and 
		//what that opcode's value is 
        if (isOpcode(opc, opcodes)) {
            opcInt = getOpcode(opc, opcodes);
        }
        else {
            std::cout << "\nError: Opcode not found!\n";
        }

		//if there is a label, get its location
        if (sym.size() > 0) {
            symInt = getLocation(sym, symbol, lineNum);
			//if there is no location, check if its a num
            if (symInt == -1 && isNumber(sym)) {
                symInt = stoi(sym);
            }
            else if (symInt == -1 && !isNumber(sym)) { 
                std::cout << "\nError: Symbol not found!\n";
            }
        }
        else {
            symInt = 0;
        }

		//convert and store opcode and label values
        memory[loopc] = STACK_SIZE * opcInt + symInt; 

		//iterate loop and clear strings
        loopc++;
        opc = "", sym = "";
    }
	//close the file
    ifile.close();
}

/// thirdPass: Performs all tasks needed for the third pass. It runs the
/// 	converted code from the memory array, and outputs the results 
/// @param symbol the array of symbol struct to access
/// @param opcodes the array of opcode strcut to access
/// @param memory the array of memory to access and modify
/// @return N/A void
void thirdPass(Symbol* symbol, Opcode* opcodes, int* memory) {
	//initializing some vars
    int reg = 0, ipoint = 0;
    std::string input = ""; 
    bool skipIncrement = false;

	//the main loop
    do {
		//loop specific vars
        int label = 0, opNum = 0;
        skipIncrement = false;

		//converting memory back to opcodes and labels
        opNum = memory[ipoint] / 1000;
        label = memory[ipoint] % 1000;

		//switch statement based on opcode from given line
        switch (opNum) {
            case 1:     //get
				//get input from user and error check
				//if it fails, end program with error
				//otherwise, print the input and store in register
                std::cin >> input;
				if (!isNumber(input)) {
					std::cout << "break\n";
					std::cout << "ERROR: invalid input!\n";
					std::cout << "** Program terminated with an error code **\n";
					exit (0);
				} 
				else {
					std::cout << "read: " << input << "\n";
                	reg = std::stoi(input);
				}
                break;  

            case 2:     //put
				//print the contents of the register
                std::cout << "result: " << reg << std::endl;
                break;

            case 3:     //ld
				//load a given value into the register
                reg = memory[label];
                break;

            case 4:     //st
				//store the register at a given address
                memory[label] = reg;
                break;  

            case 5:     //add
                reg += memory[label];
                break;

            case 6:     //sub
                reg -= memory[label];
                break;
    
            case 7:     //mul
                reg *= memory[label];
                break; 

            case 8:     //div
                reg /= memory[label];
                break;

            case 9:     //cmp
				//subtract a value from register before a jump
                reg -= memory[label];
                break;

			//for jump conditions, the instruction pointer 
			//will not be incremented 
			//and the jump is normally conditional

            case 10:    //jpos
                if (reg > 0) {
                    ipoint = label;
                    skipIncrement = true;
                }
                break;  

            case 11:    //jz
                if (reg == 0) {
                    ipoint = label;
                    skipIncrement = true;
                }
                break;

            case 12:    //j
                ipoint = label;
                skipIncrement = true;
                break; 

            case 13:    //jl
                if (reg < 0) {
                    ipoint = label;
                    skipIncrement = true;
                }
                break; 

            case 14:    //jle
                if (reg <= 0) {
                    ipoint = label;
                    skipIncrement = true;
                }
                break;

            case 15:    //jg
                if (reg > 0) {
                        ipoint = label;
                        skipIncrement = true;
                    }
                break;

            case 16:    //jge
                if (reg >= 0) {
                        ipoint = label;
                        skipIncrement = true;
                    }
                break; 

            case 17:    //halt
				//the code to end the program
                ipoint = -1;
                std::cout << "** Program terminated **\n";
                break;

            default:
				//the default case
                std::cout << "Error: Undefined Case!\n";
                ipoint = -1;       
        }

		//code to increment the instruction pointer
		//under most circumstances
        if (ipoint >= 0 && skipIncrement == false) {
            ipoint++;
        }

    //condtion to break the loop   
    } while (ipoint != -1); 
}
