MIT License

Copyright (c) 2024 Eric Malik Uecker

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int xSize = 8;
const int ySize = 8;
int numOfCars = 0;
long long numOfNodes = 0;
int recordDepth = 0;
char board[8][8 + 1] = {
	"++++++++",
	"+    P +",
	"+    P +",
	"+ XX P #",
	"+  U   +",
	"+  U   +",
	"+      +",
	"++++++++"
};

/* to solve
Nr 1:
	"++++++++",
	"+ ABBCD+",
	"+ A ECD+",
	"+ XXE F#",
	"+  II F+",
	"+   H  +",
	"+   H  +",
	"++++++++"
	
Nr 2:
	"++++++++",
	"+OAAP  +",
	"+O  P  +",
	"+OXXP  #",
	"+  BQQQ+",
	"+  B  E+",
	"+  RRRE+",
	"++++++++"
*/

/* TestBoards
Nr. 1:
	"++++++++",
	"+    P +",
	"+    P +",
	"+ XX P #",
	"+  U   +",
	"+  U   +",
	"+      +",
	"++++++++"
*/

void printBoard()
{
	for (int i = 0; i < ySize; i++) {
		printf("%s\n", board[i]);
	}
}

typedef struct {
	int x;
	int y;
}point;

typedef struct {
	point pos;
	char sign, axis;
	int len;
	int offset;
}car;

struct node;
typedef struct node{
	struct node* parent;
	char color;

	car* gameState;

	int numOfChildren;
	struct node* children;
}node;

int collisionCheck(int checkX, int checkY, car* gameState, int* magicList)
{
	if (checkX == 0 || checkY == 0 || checkX == 7 || checkY == 7) {
		return 0;
	}
	for (int i = 0; i < numOfCars; i++) {
		if (gameState[i].pos.x == checkX && gameState[i].pos.y == checkY) {
			return 0;
		}
		if (gameState[i].pos.x == checkX && gameState[i].axis == 'y' && gameState[i].pos.y < checkY && gameState[i].pos.y + gameState[i].len - 1 >= checkY) {
			return 0;
		}
		if (gameState[i].pos.y == checkY && gameState[i].axis == 'x' && gameState[i].pos.x < checkX && gameState[i].pos.x + gameState[i].len - 1 >= checkX) {
			return 0;
		}
	}
	
	//check if gamestate has already been reached by ancestors 
	/*
	*	Effizienz!
	*/
	
	/*
	if(curr->parent == NULL){
		return 1;
	}
	
	int theSame = 1;
	int count = 0;
	node* tmp = curr->parent->parent;
	while(tmp != NULL && count < 6) {
		
		for(int i = 0; i < numOfCars; i++) {
			if(curr->gameState[i].pos.x != tmp->gameState[i].pos.x || curr->gameState[i].pos.y != tmp->gameState[i].pos.y) {
				theSame = 0;
				break;
			}
		}
		
		if(theSame){
			//printf("DEBUG: Gamestate already existed\n");
			return 0;
		}
		
		count++;
		theSame = 1;
		tmp = tmp->parent;
	}
	*/
}

int nodeCheck(car* gameState, int* magicList) {

	//printf("DEBUG: Checking if Gamestate already exists\n");
	int idx = 0;
	int a = 1;
	for (int i = 0; i < numOfCars; i++) {
		idx += gameState[i].offset * a;
		a *= 5;
	}

	//printf("DEBUG: Index: %d\n", idx);
	//printf("DEBUG: Value at Index: %d\n", magicList[idx]);

	if (magicList[idx] == 1) {
		//printf("DEBUG: Gamestate already exists\n");
		return 0;
	}

	magicList[idx] = 1;
	return 1;
}

node* bfs(node* root) {
	
	numOfNodes++;
	int numOfPossibleMoves;
	node* curr = root;

	long long listSize = 1;

	for (int i = 0; i < numOfCars; i++) {
		listSize *= 5;
	}

	int* magicList = (int*) calloc(listSize, sizeof(int));

	while (curr->color != 'b' || curr->parent != NULL) {		
		
		int d = 0;
		node* tmp = curr;
		while(tmp->parent != NULL){
			d++;
			tmp = tmp->parent;
		}
		//printf("DEBUG: Current depth is %d\n",d);
		//printf("DEBUG: Number of nodes: %d\n",numOfNodes);
		if(recordDepth < d) {
			recordDepth = d;
			printf("DEBUG: Depth is %d (new record)\n", recordDepth);
			printf("DEBUG: Number of nodes: %lld\n", numOfNodes);
		}
		
		//If there are no children, build em
		if (curr->children == NULL) {

			//But first check if the current State has been reached before

			if (nodeCheck(curr->gameState, magicList) == 0) {
				curr->color = 'b';
				curr = curr->parent;
				continue;
			}

			numOfPossibleMoves = 0;
			//printf("DEBUG: There are no children, building them...\n");
			//check for the amount of possible moves
			for (int i = 0; i < numOfCars; i++) {
				if (curr->gameState[i].axis == 'x') {
					if (collisionCheck(curr->gameState[i].pos.x - 1, curr->gameState[i].pos.y, curr->gameState,	magicList)) {
						numOfPossibleMoves++;
					}
					if (collisionCheck(curr->gameState[i].pos.x + curr->gameState[i].len, curr->gameState[i].pos.y, curr->gameState, magicList)) {
						numOfPossibleMoves++;
					}
				}
				else {
					if (collisionCheck(curr->gameState[i].pos.x, curr->gameState[i].pos.y - 1, curr->gameState, magicList)) {
						numOfPossibleMoves++;
					}
					if (collisionCheck(curr->gameState[i].pos.x, curr->gameState[i].pos.y + curr->gameState[i].len, curr->gameState, magicList)) {
						numOfPossibleMoves++;
					}
				}
			}

			//if its 0, paint curr black and go into parent ->
			if (numOfPossibleMoves == 0) {
				curr->color = 'b';
				if (curr->parent != NULL) {
					curr = curr->parent;
				}
				//printf("DEBUG: No Children Possible!");
			}

			//build children out of possible moves
			else {
				curr->children = (node*)malloc(sizeof(node) * numOfPossibleMoves);
				//check Axis, if x: check left and right
				for (int i = 0; i < numOfCars; i++) {
					if (curr->gameState[i].axis == 'x') {
						if (collisionCheck(curr->gameState[i].pos.x - 1, curr->gameState[i].pos.y, curr->gameState, magicList)) {
							//going left is possible:
							numOfNodes++;
							curr->children[curr->numOfChildren].color = 'w';
							curr->children[curr->numOfChildren].parent = curr;
							curr->children[curr->numOfChildren].children = NULL;
							curr->children[curr->numOfChildren].numOfChildren = 0;
							curr->children[curr->numOfChildren].gameState = (car*)malloc(sizeof(car) * numOfCars);
							for (int j = 0; j < numOfCars; j++) {
								curr->children[curr->numOfChildren].gameState[j] = curr->gameState[j];
							}
							curr->children[curr->numOfChildren].gameState[i].pos.x--;
							curr->children[curr->numOfChildren].gameState[i].offset--;

							curr->numOfChildren++;
						}
						if (collisionCheck(curr->gameState[i].pos.x + curr->gameState[i].len, curr->gameState[i].pos.y, curr->gameState, magicList)) {
							//going right is possible:
							numOfNodes++;
							curr->children[curr->numOfChildren].color = 'w';
							curr->children[curr->numOfChildren].parent = curr;
							curr->children[curr->numOfChildren].children = NULL;
							curr->children[curr->numOfChildren].numOfChildren = 0;
							curr->children[curr->numOfChildren].gameState = (car*)malloc(sizeof(car) * numOfCars);
							for (int j = 0; j < numOfCars; j++) {
								curr->children[curr->numOfChildren].gameState[j] = curr->gameState[j];
							}
							curr->children[curr->numOfChildren].gameState[i].pos.x++;
							curr->children[curr->numOfChildren].gameState[i].offset++;

							//check if solution is found
							if(curr->children[curr->numOfChildren].gameState[i].sign == 'X' && board[3][curr->children[curr->numOfChildren].gameState[i].pos.x + 2] == '#') {
								printf("\nSolution Found!!11!1!!\n\n");
								return curr;
							}
							
							curr->numOfChildren++;
						}
					}
					
					// if y: check up n down
					else {
						if (collisionCheck(curr->gameState[i].pos.x, curr->gameState[i].pos.y - 1, curr->gameState, magicList)) {
							//going up is possible:
							numOfNodes++;
							curr->children[curr->numOfChildren].color = 'w';
							curr->children[curr->numOfChildren].parent = curr;
							curr->children[curr->numOfChildren].children = NULL;
							curr->children[curr->numOfChildren].numOfChildren = 0;
							curr->children[curr->numOfChildren].gameState = (car*)malloc(sizeof(car) * numOfCars);
							for (int j = 0; j < numOfCars; j++) {
								curr->children[curr->numOfChildren].gameState[j] = curr->gameState[j];
							}
							curr->children[curr->numOfChildren].gameState[i].pos.y--;
							curr->children[curr->numOfChildren].gameState[i].offset--;

							curr->numOfChildren++;

						}
						if (collisionCheck(curr->gameState[i].pos.x, curr->gameState[i].pos.y + curr->gameState[i].len, curr->gameState, magicList)) {
							//going down is possible:
							numOfNodes++;
							curr->children[curr->numOfChildren].color = 'w';
							curr->children[curr->numOfChildren].parent = curr;
							curr->children[curr->numOfChildren].children = NULL;
							curr->children[curr->numOfChildren].numOfChildren = 0;
							curr->children[curr->numOfChildren].gameState = (car*)malloc(sizeof(car) * numOfCars);
							for (int j = 0; j < numOfCars; j++) {
								curr->children[curr->numOfChildren].gameState[j] = curr->gameState[j];
							}
							curr->children[curr->numOfChildren].gameState[i].pos.y++;
							curr->children[curr->numOfChildren].gameState[i].offset++;

							curr->numOfChildren++;

						}
					}
				}

			}
			//after building children paint curr grey and go into parent

			if (curr->parent != NULL) {
				curr->color = 'g';
				curr = curr->parent;
			}

		}
		
		//If there are children check their color	
		else {
			//printf("DEBUG: There are Children!\n");
			//If there are white ones, go into them ->
			char c = 'b';
			for (int i = 0; i < curr->numOfChildren; i++) {
				if (curr->children[i].color == 'w') {
					//printf("DEBUG: There are white Children\n");
					c = 'w';
					curr = &curr->children[i];
					break;
				}
				if (c != 'g' && curr->children[i].color == 'g') {
					c = 'g';
				}
			}
			//If there are greys and NO whites, paint grey children white, curr grey and go into parent ->	
			if (c == 'g') {
				//printf("DEBUG: There are no white but grey Children\n");
				for (int i = 0; i < curr->numOfChildren; i++) {
					if (curr->children[i].color == 'g') {
						curr->children[i].color = 'w';
					}
				}
				if (curr->parent != NULL) {
					curr->color = 'g';
					curr = curr->parent;
				}
			}
			//If all children are black, delete children, paint curr black and go into parent ->		
			if (c == 'b') {
				//printf("DEBUG: All Children are Black!\n");
				for (int i = 0; i < curr->numOfChildren; i++) {
					free(curr->children[i].gameState);
				}
				free(curr->children);
				curr->color = 'b';
				if (curr->parent != NULL) {
					curr = curr->parent;
				}
			}
		}
		//printf("DEBUG: Repeating the loop!\n");
	}
	
	printf("\nNo solution possible ;(\n");
	return curr;
}

//void

int main()
{
	/*Car parser*/
	car* initGameState = (car*)malloc(sizeof(car) * 16);
	int carIsKnown = 0;

	for (int y = 0; y < ySize; y++) {
		for (int x = 0; x < xSize; x++) {
			if (board[y][x] != '+' && board[y][x] != ' ' && board[y][x] != '#') {
				for (int i = 0; i < numOfCars; i++) {
					if (board[y][x] == initGameState[i].sign) {
						carIsKnown = 1;
						if (initGameState[i].axis == 'y') {
							initGameState[i].len++;
						}
						break;
					}
				}
				if (carIsKnown) {
					carIsKnown = 0;
				}
				else {
					initGameState[numOfCars].sign = board[y][x];
					initGameState[numOfCars].pos.x = x;
					initGameState[numOfCars].pos.y = y;
					initGameState[numOfCars].len = 1;
					if (initGameState[numOfCars].sign == board[y][x + 1]) {
						initGameState[numOfCars].axis = 'x';
						initGameState[numOfCars].offset = initGameState[numOfCars].pos.x - 1;
						initGameState[numOfCars].len++;
						x++;
						if (initGameState[numOfCars].sign == board[y][x + 1]) {
							initGameState[numOfCars].len++;
							x++;
						}
					}
					else {
						initGameState[numOfCars].axis = 'y';
						initGameState[numOfCars].offset = y - 1;
					}
					numOfCars++;
				}
			}
		}
	}

	for (int i = 0; i < numOfCars; i++) {
		if (initGameState[i].axis == 'y') {
			printf("(%c): ((%d,%d),(0,%d)) offset = %d\n", initGameState[i].sign, initGameState[i].pos.x, initGameState[i].pos.y, initGameState[i].len, initGameState[i].offset);
		}
		else {
			printf("(%c): ((%d,%d),(%d,0)) offset = %d\n", initGameState[i].sign, initGameState[i].pos.x, initGameState[i].pos.y, initGameState[i].len, initGameState[i].offset);
		}
	}

	node* root = (node*)malloc(sizeof(node));
	root->children = NULL;
	root->color = 'w';
	root->numOfChildren = 0;
	root->parent = NULL;
	/*
	root->gameState = (car*)malloc(sizeof(car) * numOfCars);
	for(int i = 0; i < numOfCars; i++) {
		root->gameState[i] = initGameState[i];
	}
	*/
	root->gameState = initGameState;

	node* solution = bfs(root);
	
	//parse path of cars
	int depth = 1;
	node* tmp = solution;
	
	if ((solution->parent != NULL)) {
		while (tmp->parent != NULL) {
			depth++;
			tmp = tmp->parent;
		}

		printf("You need %d moves to solve the Puzzle\n", depth);

		int idx = depth * 3 - 1;

		char* path = (char*)malloc(sizeof(char) * depth * 3);

		path[idx] = '\0';
		idx--;
		path[idx] = 'R';
		idx--;
		path[idx] = 'X';
		idx--;
		path[idx] = ' ';
		idx--;

		while (solution->parent != NULL) {
			for (int i = 0; i < numOfCars; i++) {
				if (solution->gameState[i].pos.x != solution->parent->gameState[i].pos.x) {
					if (solution->gameState[i].pos.x > solution->parent->gameState[i].pos.x) {

						path[idx] = 'R',
							idx--;
						path[idx] = solution->gameState[i].sign;
						idx--;
					}
					else {
						path[idx] = 'L';
						idx--;
						path[idx] = solution->gameState[i].sign;
						idx--;
					}
					path[idx] = ' ';
					idx--;
				}
				if (solution->gameState[i].pos.y != solution->parent->gameState[i].pos.y) {
					if (solution->gameState[i].pos.y > solution->parent->gameState[i].pos.y) {
						path[idx] = 'D';
						idx--;
						path[idx] = solution->gameState[i].sign;
						idx--;
					}
					else {
						path[idx] = 'U';
						idx--;
						path[idx] = solution->gameState[i].sign;
						idx--;
					}
					path[idx] = ' ';
					idx--;
				}
			}
			solution = solution->parent;
		}

		//shortening result

		char* shortPath = (char*)malloc(sizeof(char) * depth * 4 + 1);

		shortPath[depth * 4] = '\0';

		idx = 0;

		for (int idx2 = 0; idx2 < depth * 4; idx2++) {
			if (idx >= depth * 3) {
				shortPath[idx2] = ' ';
				continue;
			}
			char sign = path[idx];
			idx++;
			char move = path[idx];
			idx += 2;
			int repeats = 1;
			while ((idx < depth * 3) && path[idx] == sign && path[idx + 1] == move) {
				idx += 3;
				repeats++;
			}
			shortPath[idx2] = sign;
			idx2++;
			shortPath[idx2] = move;
			idx2++;
			shortPath[idx2] = repeats + '0';
			idx2++;
			shortPath[idx2] = ' ';

		}

		printf("Solution: \n");
		printf("%s\n", shortPath);
		printf("\nNumber of Nodes: %lld\n", numOfNodes);
	}

	return 0;
}
