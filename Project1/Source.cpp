//I used some ideas from lecture 4.4 and from Wikipedia article about Dijkstra's algorithm

#include<iostream>
#include<vector>
#include<iterator>
#include<ctime>
#include<algorithm>
#include<random>
using namespace std;

default_random_engine  e(time(nullptr));

class graph { //graph class
public:
	graph(int hsize) { //graph class
		boardsize = hsize; //size of 1 dimension
		size = hsize * hsize; //size of the graph (number of nodes)
		for (int i = 0; i < size; ++i) stone.push_back(*empty); //create empty squares
		
	}

	vector<int> neighbors(int v) {
		vector<int> nb;
		for (int j = 0; j < 6; ++j) {
			int xCursor = v/boardsize +direct[j][0];
			int yCursor = v%boardsize + direct[j][1];
			if (inGraph(xCursor, yCursor))
				nb.push_back(xCursor * boardsize + yCursor);
		}
		return nb;
	}

	int search_min() { //search the position of next node with least distance in the Q set
		int min = 0; //put the position on zero by default
		for (int i = 0; i < Q.size(); ++i)
			if (dist[min] > dist[Q[i]]) min = i; //if the distance to the i-node is less - make it minimum
		return min; //return it
	}

	bool in_Q(int v) { //checks if the node v is in the Q set 
		for (int i = 0; i < Q.size(); ++i)
			if (Q[i] == v) return true;
		return false;
	}

	bool Dijkstra(int source, int target, const char* side) { //Dijkstra's algorithm
		dist.clear(); 
		Q.clear();  //create vertex set Q

		/* for each vertex v in Graph:             
          dist[v] ← INFINITY                                 
          add v to Q                      
      dist[source] ← 0 */

		for(int i=0;i<size;++i) {
			dist.push_back(9999);
			Q.push_back(i);
		}

		if (stone[source] == *side) dist[source] = 0;

		/*while Q is not empty:
          u ← vertex in Q with min dist[u]    
                                              
         remove u from Q */

		while (!Q.empty())
		{
			int u = Q[search_min()];
			Q.erase(Q.begin()+ search_min());
			if (u == target) break; //terminate the search if u=target
			if (dist[u] == 9999) break;
			/*for each neighbor v of u:           // only v that are still in Q
              alt ← dist[u] + length(u, v)
              if alt < dist[v]:               
                  dist[v] ← alt */

			vector<int> nb = neighbors(u);
			for(auto it: nb)
				if (stone[u] == *side && stone[it] == *side)
				{
					int v = it;
					if (in_Q(v)) {
						int alt = dist[u] + 1;
						if (alt < dist[v])
							dist[v] = alt;
					}
				}
			
		}
		if (dist[target] < 9999) return true; //if distance to the target node is
		                                      //less than 9999 then there's a path from source to it
		return false;
	}

	bool inGraph(int row, int column) { //checks if a square with these coordinates is in the graph
		return (row < boardsize&& row >= 0 && column < boardsize&& column >= 0);
	}
private:
	vector<int> Q, dist; //set of vertices of Q and set of distances dist
protected:
	int size; //size of the graph
	int boardsize; //1 dimension of the board
	vector< char > stone; //vector of squares (stones)
	const char* empty = "."; //constants for empty square, for Blue side's turn and for Red's turn
	const char* blue = "B";
	const char* red = "R";
	int direct[6][2] =
	{
	   {-1, 0}, {-1, 1}, {0,-1}, {0,1}, {1, -1}, {1, 0} // corners
	};
};

class Hex :public graph { //Hex class
private:
	char humanSide, compSide;
	int mode;
public:
	//Recomended size: 5x5
	Hex():graph(7) {} //Hex constructor. You can change the size of board here. 
	//It's really slow on 7x7...
	void PrintBoard() { //function which prints the board
		cout << "  0";
		for (int i = 1; i < boardsize; ++i)
			cout << " r " << i; cout << endl; //print the 0 string
		string between="\\";
		for (int i = 1; i < boardsize; ++i) between += " / \\"; //create a line between the stone strings
		cout << "0 "<<stone[0];
		for (int i = 1; i < boardsize; ++i)
			cout << " - " << stone[i]; cout << endl; //print the first string
		string indention=""; //string of spaces
		for (int i = 1; i < boardsize; ++i) { //loop for printing the rest of the board
			indention += " ";
			cout << indention << "b " << between<<endl; 
			if(i<10){
				indention += " ";
				cout << indention << i << " " << stone[i * boardsize];
			}
			else
			{
				cout << indention << i << " " << stone[i * boardsize];
				indention += " ";
			}
			for (int j = 1; j < boardsize; ++j)
				cout << " - " << stone[i * boardsize + j]; cout << endl;
		}
		cout << "________________________________________________________" << endl;
	}

	bool LegalMove(int row, int column) { //checks if a move is legal
		return (row<boardsize&&row>=0 && column<boardsize&&column>=0 && stone[row*boardsize+column]==*empty);
	}

	void place(int x, char side) { //place a square
		stone[x] = side;
	}

	bool Win(char side) { //checks if someone makes a win move
		if(side==*blue) 
			for (int i = 0; i < boardsize; ++i)
			for (int j = 0; j < boardsize; ++j)
				if (Dijkstra(i * boardsize, j * boardsize + boardsize - 1, blue)) 
					return true;

		if (side == *red)
			for (int i = 0; i < boardsize; ++i)
				for (int j = 0; j < boardsize; ++j)
					if (Dijkstra(i, boardsize * (boardsize - 1) + j, red)) 
						return true;
		return false;
	}

	//2 functions for 2 player mode

	void Player1_turn() {
		int row, column;
		cout << "Blue side's turn." << endl
			<<"Where are you putting your square? (row column = )"<<endl;
		//make your move
		cin >> row >> column;
		cin.clear();
		while (!LegalMove(row, column)) { //check if it's legal
			cout << "Your move is illegal. Try again:" << endl;
			cin >> row >> column;
			cin.clear();
		}
		place(row * boardsize + column, *blue);
			PrintBoard(); //print the board after the move
			if (Win(*blue)) //check if the game is over
				cout << "Blue side wins!" << endl
					<< "Reload the program to try again." << endl; 
			else Player2_turn(); //if no - invite the 2nd player to do his/her move
	}

	void Player2_turn() { //almost the same but for red side
		int row, column;
		cout << "Red side's turn." << endl
			<< "Where are you putting your square? (row column = )" << endl;
		cin >> row >> column;
		cin.clear();
		while (!LegalMove(row, column)) {
			cout << "Your move is illegal. Try again:" << endl;
			cin >> row >> column;
			cin.clear();
		}
		place(row * boardsize + column, *red);
			PrintBoard();
			if (Win(*red)) cout << "Red side wins!" << endl
				<< "Reload the program to try again." << endl;
			else Player1_turn(); //invite the first player to make his/her move
	}

	void beginGame() //beginning of the game
	{
		cout << "Which mode do you want to choose? 2 players or 1 player? (2 or 1)" << endl;
		cin >> mode;
		cin.clear();
		while (mode != 2 && mode != 1) {
			cout << "Wrong value. Try again." << endl;
			cin >> mode;
			cin.clear();
		}
		if (mode == 2) Player1_turn();
		if (mode == 1) {
			cout << "Which side do you want to choose? Blue or Red? (B or R)" << endl;
			cin >> humanSide;
			cin.clear();
			while (humanSide != *blue && humanSide != *red) {
				cout << "Wrong value. Try again." << endl;
				cin.clear();
			}
			if (humanSide == *blue) {
				compSide = *red;
				HumanPlayer();
			}
			if (humanSide == *red) {
				compSide = *blue;
				CompPlayer();
			}
		}
	}

	//for 1 player mode
	void HumanPlayer() //human's turn
	{
		int row, column;
		cout << "Your turn." << endl
			<< "Where are you putting your square? (row column = )" << endl;
		//make your move
		cin >> row >> column;
		cin.clear();
		while (!LegalMove(row, column)) { //check if it's legal
			cout << "Your move is illegal. Try again:" << endl;
			cin >> row >> column;
			cin.clear();
		}
		place(row*boardsize+column, humanSide);
		PrintBoard(); //print the board after the move
		if (Win(humanSide)) //check if the game is over
			cout << "You win!" << endl
			<< "Reload the program to try again." << endl;
		else CompPlayer(); //if no - invite the computer player to do its move
	}

	void CompPlayer() //computer's turn
	{
		cout <<endl<< "Computer is making its move..." << endl;
		int move = CompMove();
		place(move, compSide);
		cout << move / boardsize << " " << move % boardsize<<endl;
		PrintBoard();
		if (Win(compSide))
			cout << "Computer wins! Reload the program to try again." << endl;
		else HumanPlayer();
	}

	vector<int> GetEmpty() { //returns blank squares on the board
		vector<int> blankSpots;
		for (int i = 0; i < size; ++i)
			if (stone[i] == *empty) blankSpots.push_back(i);
		return blankSpots;
	}

	double GetWins() { //evaluate the probability of winning, helps monte carlo evaluation
		auto blank = GetEmpty();
		int WinCount = 0;
		vector<int> perm(blank.size());
		for (int i = 0; i < perm.size(); i++)
			perm[i] = i;
		for (int n = 0; n < 1000; n++) //you may do 1000 evaluations with 5x5 board
		{
			bool turn = (compSide==*blue ? 0:1);
			for (int i = perm.size(); i > 1; i--)
			{
				int swap = rand() % i;
				int temp = perm[i - 1];
				perm[i - 1] = perm[swap];
				perm[swap] = temp; // prand the permutation
			}
			for (int i = 0; i < perm.size(); i++)
			{
				turn = !turn; //easy bool turn tracking
				int x = blank[perm[i]];
				if (turn)
				{
					place(x, *red);
				}
				else
				{
					place(x, *blue);
				}
			}
			if (Win(compSide))
				WinCount++;
			for (auto it = blank.begin(); it != blank.end(); ++it)
				BadMove(*it);
		}
		return (1.0 * WinCount) / 1000;
	}

	bool BadMove(int x) { //retracts random moves
		if (inGraph(x/boardsize, x%boardsize)) {
			stone[x] = *empty;
				return true; 
		}
		return false;
	}

	int CompMove() { //monte carlo implementation
		auto blank = GetEmpty();
		double BestMove = 0;
		int move = blank[0];

		for (int i = 0; i < blank.size(); ++i)
		{
			int x = blank[i];
			place(x, compSide);

			double MoveValue = GetWins();
			if (MoveValue > BestMove)
			{
				move = blank[i];
				BestMove = MoveValue;
			}
			BadMove(x);
		}
		return move;
	}
};

int main() { //main function
	Hex h;
	h.PrintBoard(); //print the empty board
	srand(time(0));
	h.beginGame(); //launch the game
	return 0;
}