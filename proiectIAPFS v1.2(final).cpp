#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <algorithm> 
#include <chrono>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <stack>
#include <iomanip>
#include <thread>

#ifdef _WIN32
    // Pentru sistemul de operare Windows
    #define CLEAR_SCREEN "cls"
#else
    // Pentru sistemele de operare non-Windows (Unix/Linux/MacOS)
    #define CLEAR_SCREEN "clear"
#endif

#ifdef _WIN32
    // Pentru sistemul de operare Windows
    #define HIDE_CURSOR ""
    #define SHOW_CURSOR ""
#else
    // Pentru sistemele de operare non-Windows (Unix/Linux/MacOS)
    #define HIDE_CURSOR "\e[?25l"
    #define SHOW_CURSOR "\e[?25h"
#endif

#define N 100

using namespace std;

unsigned long long int operatiiAStar = 0;
unsigned long long int operatiiRH = 0;
unsigned long long int operatiiLH = 0;
unsigned long long int operatiiDFS = 0;
unsigned long long int operatiiBFS = 0;



/* A maze generator and solver*/
struct Cell {
	bool visited = false;
	int row = 0;
	int col = 0;

	// only need to manipulate 2 walls for each cell since cells share walls
	bool northWall = true;
	bool eastWall = true;

	//for display result
	bool path = false;
	bool search = false;

	//for RH and LH 
	vector<int> pathIndexes;

	// for A* algorithm
	bool open = false;
	bool closed = false;
	int hCost = 0; // heuristic (estimated) cost of moving from current cell to final cell
	int gCost = 0;  // cost of moving from initial cell to current cell
	int fCost = 0; // gCost + hCost
	Cell* parent = nullptr;

	void setGcost(const int currentgCost) {
		gCost = currentgCost + 1;
		operatiiAStar += 2;
	}

	void setHcost(const int startRow, const int startCol, const int targetRow, const int targetCol) {
		hCost = (targetRow - startRow) + (targetCol - startCol);
		operatiiAStar += 4;
	}

	void setFcost() {
		fCost = gCost + hCost;
		operatiiAStar += 2;
	}

};

class Maze {
private:
	// number of cells described with width and height
	int height;
	int width;
	int rot;

	// 2d maze vector
	vector<vector<Cell>> maze;



public:
	// constructor and methods
	Maze(const int inputHeight = 10, const int inputWidth = 10);
	void displayMazeDimensions() const;
	void initialiseMaze();
	void displayMaze() const;
	void _displayMaze() const;
	void generateMaze();
	void generateMazePrim();
	void solveMazeAStar();
	void determinatePath(int currentRow, int currentCol, int prevIndex);
	bool __moveRH(int *currentRow, int *currentCol, char dir);
	void solveMazeRH();
	bool __moveLH(int *currentRow, int *currentCol, char dir);
	void solveMazeLH();
	bool search(Cell* cell, vector<Cell*> exp);
	void solveMazeBFS();
	void solveMazeDFS();
	void resetMaze();
	int determinatePathLength();
	void modifyMaze();
	void nullmaze();
	int deteminateCellVisited();
};


// initialising dimensions
Maze::Maze(const int inputHeight, const int inputWidth)
	: height{ inputHeight }, width{ inputWidth } {
	rot = 0;
	//cout << "Dimensions have been initialised\n";
}

// display maze dimensions
void Maze::displayMazeDimensions() const {
	cout << "The width of the maze is " << width << endl;
	cout << "The height of the maze is " << height << endl;
}

// create empty maze full of walls
void Maze::initialiseMaze() {
	
	for (int i = 0; i < height; i++) {
		vector<Cell> mazeRow;
		for (int j = 0; j < width; j++) {
			Cell cell;
			cell.row = i;
			cell.col = j;
			mazeRow.push_back(cell);
		}
		maze.push_back(mazeRow);
	}
	//cout << "Maze has been intialised\n";
}

// display maze using raster scan (if solution found it will also display that)
void Maze::displayMaze() const {

	for (auto const& mazeRow : maze) {

		// print top/bottom walls
		for (auto const& cell : mazeRow) {
			cout << "\033[1m+";
			// print if top wall is true
			if (cell.northWall) {
				cout << "\033[1m---";
			}
			else {
				cout << "   ";
			}
		}
		cout << "\033[1m+" << endl;


		// print left/right walls on next line
		cout << "\033[1m|";
		for (auto const& cell : mazeRow) {
			// print if right wall if true ( x = path, o = searched node but not part of path)
        	
			if (cell.eastWall && cell.path && !cell.search) { // 110
				cout << "\033[1;32m x \033[0m\033[1m|";
			}
			else if (cell.eastWall && !cell.path && !cell.search) { // 100
				cout << "   \033[1m|";
			}
			else if (!cell.eastWall && cell.path && !cell.search) { // 010
				cout << "\033[1;32m x \033[0m ";
			}
			else if (cell.eastWall && cell.path && cell.search) { // 111
				cout << "\033[1;32m x \033[0m\033[1m|";
			}
			else if (cell.eastWall && !cell.path && cell.search) { // 101
				cout << "\033[1;31m o \033[0m\033[1m|";
			}
			else if (!cell.eastWall && !cell.path && cell.search) { // 001
				cout << "\033[1;31m o \033[0m ";
			}
			else if (!cell.eastWall && cell.path && cell.search) { // 011
				cout << "\033[1;32m x \033[0m ";
			}
			else {
				cout << "    "; // 000
			}
		}
		cout << endl;

	}

	// print final bottom walls
	for (int i = 0; i < width; i++) {
		cout << "\033[1m+---";
	}
	cout << "\033[1m+" << endl;

}

// beauty display maze
void Maze::_displayMaze() const {

	for (auto const& mazeRow : maze) {

		// print top/bottom walls
		for (auto const& cell : mazeRow) {
			cout << "\033[1m+";
			// print if top wall is true
			if (cell.northWall) {
				cout << "\x1B[9m   \x1B[0m";
			}
			else if(cell.path && !maze[cell.row-1][cell.col].path)
			{
				cout << "\033[1;31m\033[41m   \033[0m\033[1m";
			}
			else if(cell.path)
			{
				cout << "\033[1;32m\033[42m   \033[0m\033[1m";
			}
			else if(!cell.path && cell.search)
			{
				cout<<"\033[1;31m\033[41m   \033[0m\033[1m";
			}
			else if(!cell.path && maze[cell.row-1][cell.col].search)
			{
				cout << "   ";
			}
			else {
				cout << "   ";
			}
			
		}
		cout << "\033[1m+" << endl;


		// print left/right walls on next line
		cout << "\033[1m|";
		for (auto const& cell : mazeRow) {
			// print if right wall if true ( x = path, o = searched node but not part of path)
			if (!cell.eastWall && cell.path && maze[cell.row][cell.col+1].search && !maze[cell.row][cell.col+1].path)
			{
				cout << "\033[1;32m\033[42m   \033[0m\033[41m \033[0m";
				
			}
			else if (cell.eastWall && cell.path && !cell.search) { // 110
				cout << "\033[1;32m\033[42m   \033[0m\033[1m|";
			}
			else if (cell.eastWall && !cell.path && !cell.search) { // 100
				cout << "   \033[1m|";
			}
			else if (!cell.eastWall && cell.path && !cell.search) { // 010
				cout << "\033[1;32m\033[42m     \033[0m";
			}
			else if (cell.eastWall && cell.path && cell.search) { // 111
				cout << "\033[1;32m\033[42m   \033[0m\033[1m|";
			}
			else if (cell.eastWall && !cell.path && cell.search) { // 101
				cout << "\033[1;31m\033[41m   \033[0m\033[1m|";
			}
			else if (!cell.eastWall && !cell.path && cell.search) { // 001
				cout << "\033[1;31m\033[41m    \033[0m\033[1m";
			}
			else if (!cell.eastWall && cell.path && cell.search) { // 011
				cout << "\033[1;32m\033[42m    \033[0m";
			}
			else {
				cout << "    "; // 000
			}
		}
		cout << endl;

	}

	// print final bottom walls
	for (int i = 0; i < width; i++) {
		cout << "+\x1B[9m   \x1B[0m";
	}
	cout << "\033[1m+" << endl;

}

// depth-first search with backtracker algorithm
void Maze::generateMaze() {
	// set random seed using time

	// create a vector of pointers to Cells
	vector<Cell*> mazeStack;

	// create a vector of pointers to neighbours of cells
	vector<Cell*> neighbours;

	// choose the initial cell, mark it as visited and push it to stack
	int row = 0;
	int col = 0;
	Cell* currentCell = &maze[row][col];
	currentCell->visited = true;
	mazeStack.push_back(&maze[row][col]);

	// initialise neighbouring cell pointers to something
	Cell* northCell = currentCell;
	Cell* eastCell = currentCell;
	Cell* southCell = currentCell;
	Cell* westCell = currentCell;
	Cell* randomNeighbour = currentCell;

	// while stack is not empty
	while (!mazeStack.empty()) {

		// pop a cell from the stack and make it the current cell
		row = mazeStack.back()->row;
		col = mazeStack.back()->col;
		currentCell = &maze[row][col];
		mazeStack.pop_back();

		// define and check if any neighbours have not been visited
		// also check for out of bound indexes
		if (row > 0 && !maze[row - 1][col].visited) {
			northCell = &maze[row - 1][col];
			neighbours.push_back(northCell);
		}
		if (col < width - 1 && !maze[row][col + 1].visited) {
			eastCell = &maze[row][col + 1];
			neighbours.push_back(eastCell);
		}
		if (row < height - 1 && !maze[row + 1][col].visited) {
			southCell = &maze[row + 1][col];
			neighbours.push_back(southCell);
		}
		if (col > 0 && !maze[row][col - 1].visited) {
			westCell = &maze[row][col - 1];
			neighbours.push_back(westCell);
		}

		// check if any neighbours have not been visited
		if (!neighbours.empty()) {
			// push the current cell to the stack
			mazeStack.push_back(currentCell);
			// choose random unvisited neighbour
			randomNeighbour = neighbours[rand() % neighbours.size()];
			// push chosen cell as visited and push to stack
			randomNeighbour->visited = true;
			mazeStack.push_back(randomNeighbour);
			// remove wall between current cell and chosen cell
			if (randomNeighbour == northCell) {
				currentCell->northWall = false;
			}
			else if (randomNeighbour == eastCell) {
				currentCell->eastWall = false;
			}
			else if (randomNeighbour == southCell) {
				southCell->northWall = false;
			}
			else {
				westCell->eastWall = false;
			}
			neighbours.clear();

		}
	}
	//cout << "Maze has been generated!\n";
}

void Maze::generateMazePrim() {
    // Initialize the maze by setting all cells as unvisited
    for (auto& mazeRow : maze) {
        for (auto& cell : mazeRow) {
            cell.visited = false;
        }
    }

    // Choose starting cell
    int startRow = 0;
    int startCol = 0;
    maze[startRow][startCol].visited = true;

    // Priority queue for selecting cells based on their weights
    priority_queue<pair<int, pair<int, int>>> pq;

    // Add neighboring cells of the starting cell to the priority queue
    vector<pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
    for (const auto& dir : directions) {
        int newRow = startRow + dir.first;
        int newCol = startCol + dir.second;
        if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width) {
            pq.push({ -rand(), {newRow, newCol} }); // Using a random weight for simplicity
        }
    }

    // Generate the maze using Prim's algorithm
    while (!pq.empty()) {
        auto current = pq.top();
        pq.pop();
        int currentRow = current.second.first;
        int currentCol = current.second.second;

        if (!maze[currentRow][currentCol].visited) {
            maze[currentRow][currentCol].visited = true;

            // Connect the current cell to a visited neighboring cell
            // (remove wall between them)
            int prevRow = currentRow - 1;
            int nextRow = currentRow + 1;
            int prevCol = currentCol - 1;
            int nextCol = currentCol + 1;

            if (prevRow >= 0 && maze[prevRow][currentCol].visited) {
                maze[currentRow][currentCol].northWall = false;
            } else if (nextRow < height && maze[nextRow][currentCol].visited) {
                maze[nextRow][currentCol].northWall = false;
            } else if (prevCol >= 0 && maze[currentRow][prevCol].visited) {
                maze[currentRow][prevCol].eastWall = false;
            } else if (nextCol < width && maze[currentRow][nextCol].visited) {
                maze[currentRow][currentCol].eastWall = false;
            }

            // Add neighboring unvisited cells to the priority queue
            for (const auto& dir : directions) {
                int newRow = currentRow + dir.first;
                int newCol = currentCol + dir.second;
                if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width && !maze[newRow][newCol].visited) {
                    pq.push({ -rand(), {newRow, newCol} }); // Using a random weight for simplicity
                }
            }
        }
    }
}



// A* algorithm
void Maze::solveMazeAStar() {

	// set of nodes to be evaluated
	vector<Cell*> openList;

	// set of nodes already evaluated
	vector<Cell*> closedList;

	// set target node
	int targetRow = height - 1;
	int targetCol = width - 1;
	Cell* targetNode = &maze[targetRow][targetCol];

	// set start and current node
	int startRow = 0;
	int startCol = 0;
	Cell* startNode = &maze[startRow][startCol];
	Cell* currentNode = startNode;

	// print out start and end coordinates
	//cout << "\nStart coordinates are: " << startRow << ", " << startCol << endl;
	//cout << "Target coordinates are: " << targetRow << ", " << targetCol << endl;

	// add current node to open list
	openList.push_back(currentNode);
	currentNode->open = true;


	// initialise neighbouring cell pointers to nullptr
	/*Cell* northNode{ nullptr };
	Cell* eastNode{ nullptr };
	Cell* southNode{ nullptr };
	Cell* westNode{ nullptr };*/
	Cell* neighbourNode{ nullptr };

	// set some variables
	int currentLowestfCost = 0;
	int currentLowestfCostIndex = 0;

	operatiiAStar += 23;

	// loop
	while (1) {
		
		// set current node to node with lowest fCost in open list
		currentLowestfCost = openList[0]->fCost;
		currentLowestfCostIndex = 0;
		operatiiAStar += 5;
		for (unsigned int i = 1; i < openList.size(); i++ ) {
			if (openList[i]->fCost < currentLowestfCost) {
				currentLowestfCost = openList[i]->fCost;
				currentLowestfCostIndex = i;
				operatiiAStar += 3;
			}
			operatiiAStar += 3;
		}
		currentNode = openList[currentLowestfCostIndex];

		// remove current node from open list
		operatiiAStar += openList.size() - currentLowestfCostIndex;
		openList.erase(openList.begin() + currentLowestfCostIndex);
		currentNode->open = false;

		// add current node to closed list
		closedList.push_back(currentNode);
		currentNode->closed = true;
		operatiiAStar += 4;

		// escape loop if current node is equal to target node
		if (currentNode == targetNode) {
			break;
		}
		
		// for each neighbour (4 adjacent) of the current [row][col]
		operatiiAStar += 4;
		for (int j = -1; j < 2; j++) {
			for (int i = -1; i < 2; i++) {
				operatiiAStar += 11;

				// skip if diagonal neighbour or current node
				if (i == j || i == -j || (i == 0 && j == 0)) {
					continue;
				}
				// skip if neighbour is out of bounds
				operatiiAStar += 11;
				if (currentNode->row + i < 0 || currentNode->col + j < 0 || currentNode->row + i >= height || currentNode->col + j >= width) {
					continue;
				}
				// skip if neighbour is in closed list
				operatiiAStar += 2;
				if (maze[currentNode->row + i][currentNode->col + j].closed) {
					continue;
				}
				// skip if current neighbour is north and north wall exists
				operatiiAStar += 4;
				if (i == -1 && j == 0 && maze[currentNode->row][currentNode->col].northWall ) {
					continue;
				}
				// skip if current neighbour is east and east wall exists
				operatiiAStar += 4;
				if (i == 0 && j == 1 && maze[currentNode->row][currentNode->col].eastWall) {
					continue;
				}
				// skip if current neighbour is south and south wall exists
				operatiiAStar += 5;
				if (i == 1 && j == 0 && maze[currentNode->row + 1][currentNode->col].northWall) {
					continue;
				}
				// skip if current neighbour is west and west wall exists
				operatiiAStar += 5;
				if (i == 0 && j == -1 && maze[currentNode->row][currentNode->col - 1].eastWall) {
					continue;
				}

				// set gCost of neighbour
				neighbourNode = &maze[currentNode->row + i][currentNode->col + j];
				neighbourNode->setGcost(currentNode->gCost);
				operatiiAStar += 7;
				// if new path to neighbour is shorter or neighbour is not in open list
				if (neighbourNode->gCost < currentNode->gCost || !neighbourNode->open) {
					// set fCost of neighbour
					neighbourNode->setHcost(startRow, startCol, targetRow, targetCol);
					neighbourNode->setFcost();
					// set parent of neighbour to current
					neighbourNode->parent = currentNode;
					neighbourNode->search = true;
					operatiiAStar += 5;
					// if neighbour is not in open list
					if (!neighbourNode->open) {
						neighbourNode->open = true;
						openList.push_back(neighbourNode);
						operatiiAStar += 2;
					}
				}
			}
		}
	}

	// maze has been solved
	// now backtrack from target node to find path and store solution
	currentNode = targetNode;
	operatiiAStar += 2;
	while (currentNode != startNode) {
		currentNode->path = true;
		currentNode = currentNode->parent;
		operatiiAStar += 3;
	}
	startNode->path = true;
	//cout << "Maze has been solved!\n";
	//cout << "x = path, o = search area\n";

}



int pathIndex = 0;

void Maze::determinatePath(int currentRow, int currentCol, int prevIndex) {
	int maxIndex = 0;
	bool check = 0;
	operatiiRH += 6;
	operatiiLH += 6;
	for (size_t i = 0; i < maze[currentRow][currentCol].pathIndexes.size(); i++)
	{
		maxIndex = (maxIndex < maze[currentRow][currentCol].pathIndexes[i])? maze[currentRow][currentCol].pathIndexes[i] : maxIndex;
		operatiiRH += 4;
		operatiiLH += 4;
		if (maze[currentRow][currentCol].pathIndexes[i] - 1 == prevIndex) {
			check = 1;
			operatiiRH += 1;
			operatiiLH += 1;
		}
	}
	if (check)
	{
		maze[currentRow][currentCol].path = 1;
		operatiiRH += 3;
		operatiiLH += 3;
		if (currentRow - 1 >= 0)
		{
			determinatePath(currentRow - 1, currentCol, maxIndex);
			operatiiRH += 2;
			operatiiLH += 2;
		}
		operatiiRH += 2;
		operatiiLH += 2;
		if (currentRow + 1 < height)
		{
			determinatePath(currentRow + 1, currentCol, maxIndex);
			operatiiRH += 2;
			operatiiLH += 2;
		}
		operatiiRH += 2;
		operatiiLH += 2;
		if (currentCol - 1 >= 0)
		{
			determinatePath(currentRow, currentCol - 1, maxIndex);
			operatiiRH += 2;
			operatiiLH += 2;
		}
		operatiiRH += 2;
		operatiiLH += 2;
		if (currentCol + 1 < width)
		{
			determinatePath(currentRow, currentCol + 1, maxIndex);
			operatiiRH += 2;
			operatiiLH += 2;
		}
	}
	operatiiRH += 1;
	operatiiLH += 1;
}

bool Maze::__moveRH(int *currentRow, int *currentCol, char dir) {
    switch (dir) {
        case 'N':
			operatiiRH += 3;
            if (*currentCol + 1 < width) {
				operatiiRH += 1;
                if (!maze[*currentRow][*currentCol].eastWall) {
                    *currentCol += 1;
                    maze[*currentRow][*currentCol].search = 1;
					pathIndex++;
					maze[*currentRow][*currentCol].pathIndexes.push_back(pathIndex);
                    //maze[*currentRow][*currentCol].pathTest = pathIndex++;
					operatiiRH += 5;
                    return true;
                }
            }
            break;
        case 'E':
			operatiiRH += 3;
            if (*currentRow - 1 >= 0) {
				operatiiRH += 1;
                if (!maze[*currentRow][*currentCol].northWall) {
                    *currentRow -= 1;
                    maze[*currentRow][*currentCol].search = 1;
                    pathIndex++;
					maze[*currentRow][*currentCol].pathIndexes.push_back(pathIndex);
					//maze[*currentRow][*currentCol].pathTest = pathIndex++;
					operatiiRH += 5;
                    return true;
                }
            }
            break;
        case 'S':
			operatiiRH += 3;
            if (*currentCol - 1 >= 0) {
				operatiiRH += 2;
                if (!maze[*currentRow][*currentCol - 1].eastWall) {
                    *currentCol -= 1;
                    maze[*currentRow][*currentCol].search = 1;
                    pathIndex++;
					maze[*currentRow][*currentCol].pathIndexes.push_back(pathIndex);
					//maze[*currentRow][*currentCol].pathTest = pathIndex++;
					operatiiRH += 5;
                    return true;
                }
            }
            break;
        case 'W':
			operatiiRH += 3;
            if (*currentRow + 1 < height) {
				operatiiRH += 2;
                if (!maze[*currentRow + 1][*currentCol].northWall) {
                    *currentRow += 1;
                    maze[*currentRow][*currentCol].search = 1;
                    pathIndex++;
					maze[*currentRow][*currentCol].pathIndexes.push_back(pathIndex);
					//maze[*currentRow][*currentCol].pathTest = pathIndex++;
					operatiiRH += 5;
                    return true;
                }
            }
            break;
        default:
            break;
    }
	operatiiRH += 2;
    return false;
}

void Maze::solveMazeRH() {
    int currentRow = 0;
    int currentCol = 0;
    int targetRow = height - 1;
    int targetCol = width - 1;
    maze[currentRow][currentCol].search = 1;
	pathIndex = 1;
    maze[currentRow][currentCol].pathIndexes.push_back(pathIndex);
    char dir = 'N'; // Orientare inițială către nord
	operatiiRH += 15;
    while (!(currentRow == targetRow && currentCol == targetCol)) {
        bool moved = __moveRH(&currentRow, &currentCol, dir);
		operatiiRH += 7;
        if (!moved) {
            // Roatează spre dreapta
            switch (dir) {
                case 'N': dir = 'E'; break;
                case 'E': dir = 'S'; break;
                case 'S': dir = 'W'; break;
                case 'W': dir = 'N'; break;
                default: break;
            }
        } else {
            // Roatează spre stânga
            switch (dir) {
                case 'N': dir = 'W'; break;
                case 'E': dir = 'N'; break;
                case 'S': dir = 'E'; break;
                case 'W': dir = 'S'; break;
                default: break;
            }
        }
		operatiiRH += 5;

        // Dacă robotul ajunge în colțul din dreapta jos, se oprește
        if (currentRow == targetRow && currentCol == targetCol) {
            break;
        }
    }
	determinatePath(0, 0, 0);
}

bool Maze::__moveLH(int *currentRow, int *currentCol, char dir) {
    switch (dir) {
        case 'N':
			operatiiLH += 3;
            if (*currentCol - 1 >= 0) {
				operatiiLH += 2;
                if (!maze[*currentRow][*currentCol - 1].eastWall) {
                    *currentCol -= 1;
                    maze[*currentRow][*currentCol].search = 1;
					pathIndex++;
					maze[*currentRow][*currentCol].pathIndexes.push_back(pathIndex);
                    //maze[*currentRow][*currentCol].pathTest = pathIndex++;
					operatiiLH += 5;
                    return true;
                }
            }
            break;
        case 'E':
			operatiiLH += 3;
            if (*currentRow + 1 < height) {
				operatiiLH += 2;
                if (!maze[*currentRow + 1][*currentCol].northWall) {
                    *currentRow += 1;
                    maze[*currentRow][*currentCol].search = 1;
                    pathIndex++;
					maze[*currentRow][*currentCol].pathIndexes.push_back(pathIndex);
					//maze[*currentRow][*currentCol].pathTest = pathIndex++;
					operatiiLH += 5;
                    return true;
                }
            }
            break;
        case 'S':
			operatiiLH += 3;
            if (*currentCol + 1 < width) {
				operatiiLH += 1;
                if (!maze[*currentRow][*currentCol].eastWall) {
                    *currentCol += 1;
                    maze[*currentRow][*currentCol].search = 1;
                    pathIndex++;
					maze[*currentRow][*currentCol].pathIndexes.push_back(pathIndex);
					//maze[*currentRow][*currentCol].pathTest = pathIndex++;
					operatiiLH += 5;
                    return true;
                }
            }
            break;
        case 'W':
			operatiiLH += 3;
            if (*currentRow - 1 >= 0) {
				operatiiLH += 1;
                if (!maze[*currentRow][*currentCol].northWall) {
                    *currentRow -= 1;
                    maze[*currentRow][*currentCol].search = 1;
					pathIndex++;
					maze[*currentRow][*currentCol].pathIndexes.push_back(pathIndex);
                    //maze[*currentRow][*currentCol].pathTest = pathIndex++;
					operatiiLH += 5;
                    return true;
                }
            }
            break;
        default:
            break;
    }
	operatiiLH += 2;
    return false;
}

void Maze::solveMazeLH() {
    int currentRow = 0;
    int currentCol = 0;
    int targetRow = height - 1;
    int targetCol = width - 1;
    maze[currentRow][currentCol].search = 1;
    pathIndex = 1;
    maze[currentRow][currentCol].pathIndexes.push_back(pathIndex);
    char dir = 'N'; // Orientare inițială către nord
	operatiiLH += 15;
    while (currentRow != targetRow || currentCol != targetCol) {
        bool moved = __moveLH(&currentRow, &currentCol, dir);
		operatiiLH += 8;
        if (!moved) {
            // Schimbă direcția spre stânga
            switch (dir) {
                case 'N': dir = 'W'; break;
                case 'E': dir = 'N'; break;
                case 'S': dir = 'E'; break;
                case 'W': dir = 'S'; break;
                default: break;
            }
        } else {
            // Roatează spre dreapta
            switch (dir) {
                case 'N': dir = 'E'; break;
                case 'E': dir = 'S'; break;
                case 'S': dir = 'W'; break;
                case 'W': dir = 'N'; break;
                default: break;
            }
        }
    }
	determinatePath(0, 0, 0);
}

bool Maze::search(Cell* cell, vector<Cell*> exp)
{
	for(Cell* C : exp)
	{
		if(C == cell)
		return true;
	}
	return false;
}

void Maze::solveMazeBFS() {
    // set target node
    int targetRow = height - 1;
    int targetCol = width - 1;
    Cell* targetNode = &maze[targetRow][targetCol];

    // set start node
    int startRow = 0;
    int startCol = 0;
    Cell* startNode = &maze[startRow][startCol];

    // create a queue for BFS
    queue<Cell*> bfsQueue;

    // enqueue start node
    bfsQueue.push(startNode);
    startNode->search = true;
	operatiiBFS += 17;
    // loop until the queue is empty or the target node is reached
    while (!bfsQueue.empty()) {
        // dequeue a node from the front of the queue
        Cell* currentNode = bfsQueue.front();
        bfsQueue.pop();
		operatiiBFS += 7;
        // check if the current node is the target node
        if (currentNode == targetNode) {
            break;
        }

        // neighbor above
		operatiiBFS += 7;
        if (currentNode->row - 1 >= 0 && !maze[currentNode->row][currentNode->col].northWall &&
            !maze[currentNode->row - 1][currentNode->col].search) {
            bfsQueue.push(&maze[currentNode->row - 1][currentNode->col]);
            maze[currentNode->row - 1][currentNode->col].search = true;
            maze[currentNode->row - 1][currentNode->col].parent = currentNode;
			operatiiBFS += 6;
        }

        // neighbor below
		operatiiBFS += 8;
        if (currentNode->row + 1 < height && !maze[currentNode->row + 1][currentNode->col].northWall &&
            !maze[currentNode->row + 1][currentNode->col].search) {
            bfsQueue.push(&maze[currentNode->row + 1][currentNode->col]);
            maze[currentNode->row + 1][currentNode->col].search = true;
            maze[currentNode->row + 1][currentNode->col].parent = currentNode;
			operatiiBFS += 6;
        }

        // neighbor to the left
		operatiiBFS += 8;
        if (currentNode->col - 1 >= 0 && !maze[currentNode->row][currentNode->col - 1].eastWall &&
            !maze[currentNode->row][currentNode->col - 1].search) {
            bfsQueue.push(&maze[currentNode->row][currentNode->col - 1]);
            maze[currentNode->row][currentNode->col - 1].search = true;
            maze[currentNode->row][currentNode->col - 1].parent = currentNode;
			operatiiBFS += 6;
        }

        // neighbor to the right
		operatiiBFS += 7;
        if (currentNode->col + 1 < width && !maze[currentNode->row][currentNode->col].eastWall &&
            !maze[currentNode->row][currentNode->col + 1].search) {
            bfsQueue.push(&maze[currentNode->row][currentNode->col + 1]);
            maze[currentNode->row][currentNode->col + 1].search = true;
            maze[currentNode->row][currentNode->col + 1].parent = currentNode;
			operatiiBFS += 6;
        }
    }

    // reconstruct and mark the path
    Cell* currentNode = targetNode;
	operatiiBFS += 2;
    while (currentNode != nullptr) {
        currentNode->path = true;
        currentNode = currentNode->parent;
		operatiiBFS += 2;
    }

    //cout << "Maze has been solved using BFS!\n";
}

void Maze::solveMazeDFS() {
    // set target node
    int targetRow = height - 1;
    int targetCol = width - 1;
    Cell* targetNode = &maze[targetRow][targetCol];

    // set start node
    int startRow = 0;
    int startCol = 0;
    Cell* startNode = &maze[startRow][startCol];

    // create a stack for DFS
    stack<Cell*> dfsStack;

    // push start node onto stack
    dfsStack.push(startNode);
    startNode->search = true;
	operatiiDFS += 17;
    // loop until stack is empty or target node is reached
    while (!dfsStack.empty()) {
        // get top node from stack
        Cell* currentNode = dfsStack.top();
        dfsStack.pop();
		operatiiDFS += 7;
        // check if current node is the target node
        if (currentNode == targetNode) {
            break;
        }
		operatiiDFS += 4;
        // check each neighbor of the current node
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                // skip diagonal and current node
				operatiiDFS += 10;
                if ((i == 0 && j == 0) || (i != 0 && j != 0)) {
                    continue;
                }

                int newRow = currentNode->row + i;
                int newCol = currentNode->col + j;
				operatiiDFS += 13;
                // check if neighbor is within bounds
                if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width) {
                    Cell* neighbor = &maze[newRow][newCol];
					operatiiDFS += 9;
                    if (i == 0 && j == -1 && !neighbor->eastWall && !neighbor->search) {
						dfsStack.push(neighbor);
						neighbor->search = true;
						neighbor->parent = currentNode;
						operatiiDFS += 3;
					} else if (i == 0 && j == 1 && !currentNode->eastWall && !neighbor->search) {
						dfsStack.push(neighbor);
						neighbor->search = true;
						neighbor->parent = currentNode;
						operatiiDFS += 10;
					} else if (i == -1 && j == 0 && !currentNode->northWall  && !neighbor->search) {
						dfsStack.push(neighbor);
						neighbor->search = true;
						neighbor->parent = currentNode;
						operatiiDFS += 10;
					} else if (i == 1 && j == 0 && !neighbor->northWall && !neighbor->search) {
						dfsStack.push(neighbor);
						neighbor->search = true;
						neighbor->parent = currentNode;
						operatiiDFS += 10;
					}
                }
            }
        }
    }

    // reconstruct and mark the path
    Cell* currentNode = targetNode;
	operatiiDFS += 2;
    while (currentNode != nullptr) {
        currentNode->path = true;
        currentNode = currentNode->parent;
		operatiiDFS += 3;
    }

    //cout << "Maze has been solved using DFS!\n";
}

void Maze::resetMaze() {
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			maze[i][j].search = false;
			maze[i][j].path = false;
			maze[i][j].pathIndexes.clear();
		}
	}
}

int Maze::determinatePathLength() {
	int length = 0;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (maze[i][j].path)
			{
				length++;
			}
			
		}
	}
	return length;
}

int Maze::deteminateCellVisited()
{
	int length = 0;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (maze[i][j].path || maze[i][j].search)
			{
				length++;
			}
			
		}
	}
	return length;
}

void Maze::modifyMaze()
{
	// 0 east 1 north
	int dir;
	for(int i=3; i<height; i++)
	{
		for(int j=3; j<width-1; j+=2)
		{
			dir = rand()%2;
			if(dir)
			{
				maze[i][j].northWall = false;
			}
			else
			{
				maze[i][j].eastWall = false;
			}
			
		}
	}
}

void Maze::nullmaze()
{
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			maze[i][j].northWall = true;
			maze[i][j].eastWall = true;
			maze[i][j].visited = false;
		}
	}
}


int main() {
	srand(time(NULL));
	vector<unsigned long long int> v_operatiiAStar, v_operatiiRH, v_operatiiLH, v_operatiiDFS, v_operatiiBFS;
	unsigned long long int m_operatiiAStar, m_operatiiRH, m_operatiiLH, m_operatiiDFS, m_operatiiBFS;
	int marimi[] = {32, 45, 55, 64, 71, 78, 84, 90, 95, 100};
	
	for (auto &&i : marimi) {
		m_operatiiAStar = 0;
		m_operatiiRH = 0;
		m_operatiiLH = 0;
		m_operatiiDFS = 0;
		m_operatiiBFS = 0;
		for(int j=0; j<N; j++) {
			operatiiAStar = 0;
			operatiiRH = 0;
			operatiiLH = 0;
			operatiiDFS = 0;
			operatiiBFS = 0;
			int height = i;
			int width = i;
			Maze maze(height, width);
			//maze.displayMazeDimensions();
			//cout << "celule total " << width * height << endl;

			maze.initialiseMaze();
			maze.generateMaze();
			//maze.displayMaze();

			maze.modifyMaze();
			maze.solveMazeAStar();
			m_operatiiAStar += maze.deteminateCellVisited();
			//maze.displayMaze();
			
			maze.resetMaze();
			operatiiLH = 0;
			maze.solveMazeLH();
			m_operatiiLH += maze.deteminateCellVisited();
			//maze.displayMaze();

			maze.resetMaze();
			operatiiRH = 0;
			maze.solveMazeRH();
			m_operatiiRH += maze.deteminateCellVisited();
			//maze.displayMaze();

			maze.resetMaze();
			maze.solveMazeDFS();
			m_operatiiDFS += maze.deteminateCellVisited();
			//maze.displayMaze();

			maze.resetMaze();
			maze.solveMazeBFS();
			m_operatiiBFS += maze.deteminateCellVisited();
			//maze.displayMaze();
		}
		v_operatiiAStar.push_back(m_operatiiAStar/N);
		v_operatiiLH.push_back(m_operatiiLH/N);
		v_operatiiRH.push_back(m_operatiiRH/N);
		v_operatiiDFS.push_back(m_operatiiDFS/N);
		v_operatiiBFS.push_back(m_operatiiBFS/N);
	}
	
	cout << "A*   ";
	for (size_t i = 0; i < 10; i++)
	{
		cout << setw(8) << v_operatiiAStar[i];
	}
	cout << endl << "LH   ";
	for (size_t i = 0; i < 10; i++)
	{
		cout << setw(8) <<  v_operatiiLH[i];
	}
	cout << endl << "RH   ";
	for (size_t i = 0; i < 10; i++)
	{
		cout << setw(8)  << v_operatiiRH[i];
	}
	cout << endl << "DFS  ";
	for (size_t i = 0; i < 10; i++)
	{
		cout << setw(8) << v_operatiiDFS[i];
	}
	cout  << endl << "BFS  ";
	for (size_t i = 0; i < 10; i++)
	{
		cout << setw(8) << v_operatiiBFS[i];
	}
	cout << endl;
	
	/*
	for(int i = 0; i<10; i++)
	{
			operatiiAStar = 0;
			operatiiRH = 0;
			operatiiLH = 0;
			operatiiDFS = 0;
			operatiiBFS = 0;
			int height = 71;
			int width = 71;
			Maze maze(height, width);
			//maze.displayMazeDimensions();
			cout << "\n\ncelule total " << width * height << endl;

			maze.initialiseMaze();
			maze.generateMaze();
			//maze.displayMaze();
			

			//maze.modifyMaze();
			//maze.displayMaze();
			maze.solveMazeAStar();
			//cout<<" "<<operatiiAStar;
			cout<<" "<<maze.determinatePathLength();
			//maze.displayMaze();
			
			maze.resetMaze();
			operatiiLH = 0;
			maze.solveMazeLH();
			//cout<<"\n "<<operatiiLH;
			cout<<"\n "<<maze.determinatePathLength();
			//maze.displayMaze();

			maze.resetMaze();
			operatiiRH = 0;
			maze.solveMazeRH();
			//maze.displayMaze();
			//cout<<"\n "<<operatiiRH;
			cout<<"\n "<<maze.determinatePathLength();
			//maze.displayMaze();

			maze.resetMaze();
			operatiiDFS = 0;
			maze.solveMazeDFS();
			//cout<<"\n "<<operatiiDFS;
			cout<<"\n "<<maze.determinatePathLength();
			//maze.displayMaze();

			maze.resetMaze();
			operatiiBFS = 0;
			maze.solveMazeBFS();
			//cout<<"\n "<<operatiiBFS;
			cout<<"\n "<<maze.determinatePathLength();
			//maze.displayMaze();
	}*/
	
	return 0;

}