#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cctype>

using namespace std;
//---------------------------------------------------------------------
// Represents a position on the grid
//---------------------------------------------------------------------
class position {
    friend class game_state;
private:
    char text[3];
    int col, row;
public:
    position() { // Default constructor
        col = 0;
        row = 0;
    }
    void from_text(const char* str);  // Sets a position from a string
    char* to_text();   // Converts the internal representation to a string
    bool is_valid();    // Returns true if the position is valid
    void operator =(const position& p);   // Copies the contents of a position into this one
    void offset(const int x, const int y);    // Offsets a position by x in the alpha value and y in the numeric value.
    void from_num(int x, int y);    // Sets a position from two numbers
};
void position::from_text(const char* str) {
    col = str[0] - 65;
    row = 52 - str[1];
}
char* position::to_text() {
    text[0] = col + 65;
    text[1] = 52 - row;
    text[2] = '\0';

    return text;
}
bool position::is_valid() {
    if (((col >= 0) && (col <= 3)) && ((row >= 0) && (row <= 3)))
        return true;
    else
        return false;
}
void position::operator=(const position& p) {
    col = p.col;
    row = p.row;
    text[0] = p.text[0];
    text[1] = p.text[1];
    text[2] = p.text[2];
}
void position::offset(const int x, const int y) {
    col += x;
    row -= y;
}
void position::from_num(int x, int y) {
    col = x;
    row = y;
    to_text();
}
//---------------------------------------------------------------------
// Represents a move to be made
//---------------------------------------------------------------------
class move {
private:
    char text[6];
    position new_position, old_position;
public:
    move() { // Default constructor
        set_from(old_position);
        set_to(new_position);
    }
    move(position& from, position& to);   // construct from two positions
    void from_text(const char* str);  // Converts a string to a move,
    char* to_text();   // Converts the internal representation to a string
    void set_from(const position& from);  // Sets the from using a position
    void set_to(const position& to);  // Sets the to using a position
    position& get_from();   // Gets either the starting
    position& get_to(); // or ending position
    void get_move();    //Prompt user for a move
};
move::move(position& from, position& to) {
    old_position = from;
    new_position = to;
}
void move::from_text(const char* str) {
    char from[3], to[3];

    from[0] = str[0];
    from[1] = str[1];
    from[2] = '\0';
    to[0] = str[3];
    to[1] = str[4];
    to[2] = '\0';

    old_position.from_text(from);
    new_position.from_text(to);
}
char* move::to_text() {
    text[0] = old_position.to_text()[0];
    text[1] = old_position.to_text()[1];
    text[2] = '-';
    text[3] = new_position.to_text()[0];
    text[4] = new_position.to_text()[1];
    text[5] = '\0';

    return text;
}
void move::set_from(const position& from) {
    old_position = from;
}
void move::set_to(const position& to) {
    new_position = to;
}
position& move::get_from() {
    return old_position;
}
position& move::get_to() {
    return new_position;
}
void move::get_move() {
    char input[40];
    std::cout << "Enter a move: ";
    std::cin >> input;
    std::cout << "\n";

    for (int i = 0; i < 41; i++) { //Converts lowercase to uppercase
        if (input[i] == 97) {
            input[i] = toupper('a');
        }
        else if (input[i] == 98) {
            input[i] = toupper('b');
        }
        else if (input[i] == 99) {
            input[i] = toupper('c');
        }
        else if (input[i] == 100) {
            input[i] = toupper('d');
        }
    }
    from_text(input);
}

//---------------------------------------------------------------------
// Structure of a tree contains linked list of possible moves
//---------------------------------------------------------------------
class move_list {
    friend class game_state;
private:
    class move mv;     // The move in the list
    int score;         //score of the state
    move_list* next;
    move_list* child;

public:
    move_list(class move& m);  // Constructor
    ~move_list();     // Destructor – removes all items in the list.
    move_list* add(move_list* m);   // Inserts m into the start of the list, and returns the new start.
    move_list* best_move();
};
move_list::move_list(class move& m) {
    mv = m;
    this->next = NULL;
    this->child = NULL;
    score = 0;
}
move_list::~move_list() {
    if (next != NULL)
        delete next;

    if (child != NULL)
        delete child;
}
move_list* move_list::add(move_list* m) {
    m->next = this;

    return m;
}

//---------------------------------------------------------------------
// The types of pieces
enum piece { EMPTY, RED_PAWN, GREEN_PAWN, RED_KING, GREEN_KING };

//---------------------------------------------------------------------
// Represents the state of a game
//---------------------------------------------------------------------
class game_state {
private:
    piece grid[4][4];
    bool gameOver, turn;
    int turns, red_pieces, green_pieces;

public:
    game_state() { new_game(); };  // Default constructor
    game_state(game_state& g);    // Copy constructor

    void new_game();    // Initialises state for the start of a new game
    bool is_game_over();    // Check the game status
    bool is_red_turn();     //Returns true of its red's turn
    bool is_green_turn();   //Returns true if its green's turn
    int get_move_number();  // How many moves have been played?
    void increment_move();  // Increment move number (and state)
    piece get_piece(const position& p); // What piece is at the specified position
    void display();     //displays user interface
    bool check_move(class move& m); //Returns true if a move is legal
    void make_move(class move& m);  //Makes the move
    move_list* find_moves();       //Returns a linked list of all possible moves
    void print_list(move_list* p);   //Prints a linked list
    int minimax(game_state& state, int depth, bool maximizing_player, int alpha, int beta);
    int  evaluate();                //Evaluates a game state based on features - Type of pieces on the board, Leap opportunities, Blocked
    void computer_move();       //computer plays a move, no trees implemented
    move_list* find_pos_moves(position* p);  //returns list of possible moves for a position
    bool  check_move_evaluate(class move& m);    //this function is used in find_pos_moves it checks for illegal moves
};
game_state::game_state(game_state& g) {
    gameOver = g.gameOver;
    turn = g.turn;
    turns = g.turns;
    red_pieces = g.red_pieces;
    green_pieces = g.green_pieces;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) { //O(n^2)
            grid[j][i] = g.grid[j][i];
        }
    }
}
void  game_state::new_game() {
    turns = 0;
    green_pieces = 2;
    red_pieces = 2;
    gameOver = false;


    // random starting turns for fairness
    int first_turn;
    srand(time(NULL));
    first_turn = rand() % 2;

    switch (first_turn) {
    case 1: turn = true;
        break;
    case 0: turn = false;
        break;
    }



    for (int i = 0; i < 4; i++) {
        for (int j = 0; j <= 3; j++) {  // Builds the grid O(n^2)
            grid[j][i] = EMPTY;
            if ((i == 3 && j == 1) || (i == 3 && j == 3)) {
                grid[j][i] = GREEN_PAWN;
            }
            else if ((i == 0 && j == 0) || (i == 0 && j == 2)) {
                grid[j][i] = RED_PAWN;
            }
        }
    }
}
bool  game_state::is_game_over() {
    int greenp{ 0 }, redp{ 0 };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j <= 3; j++) {  //counts number of pieces on the grid O(n^2)
            if (grid[j][i] == GREEN_PAWN || grid[j][i] == GREEN_KING) {
                greenp++;
            }
            else if (grid[j][i] == RED_PAWN || grid[j][i] == RED_KING) {
                redp++;
            }


        }
    }

    green_pieces = greenp;
    red_pieces = redp;

    if (turns > 40 || greenp == 0 || redp == 0) { // Checks if a player is out of pieces
        return true;
    }
    else {
        return false;
    }


}
bool  game_state::is_red_turn() {
    if (turn == true)
        return true;
    else
        return false;

}
bool  game_state::is_green_turn() {
    if (turn == false)
        return true;
    else
        return false;

}
int   game_state::get_move_number() {
    return turns;
}
void  game_state::increment_move() {
    if (turns <= 40) {
        turns++;
        gameOver = false;
    }
    else
        gameOver = true;


}
piece game_state::get_piece(const position& p) {
    int col = p.col;
    int row = p.row;

    return grid[col][row];
}
void  game_state::display() {
    cout << "\n-*-*-*-*-*-*-*-*-\n";
    std::cout << "Green pieces: " << green_pieces << endl;
    std::cout << "Red pieces: " << red_pieces << endl;
    std::cout << "Total number of moves: " << get_move_number() << std::endl;
    std::cout << "Game over: ";
    if (is_game_over() == false)
        cout << "No." << endl;
    if (is_game_over() == true)
        cout << "Yes" << endl;
    cout << "-*-*-*-*-*-*-*-*-\n";
    std::cout << "Turn: ";
    if (is_red_turn())
        std::cout << "Red.\n";
    if (is_green_turn())
        std::cout << "Green.\n";
    cout << "-*-*-*-*-*-*-*-*-\n";

    std::cout << "  A " << "B " << "C " << "D \n";
    int p = 4;
    for (int i = 0; i < 4; i++) {
        std::cout << p << " ";
        p--;
        for (int j = 0; j < 4; j++) {
            switch (grid[j][i]) {
            case EMPTY: std::cout << "- ";
                break;
            case RED_PAWN: std::cout << "r ";
                break;
            case RED_KING: std::cout << "R ";
                break;
            case GREEN_PAWN: std::cout << "g ";
                break;
            case GREEN_KING: std::cout << "G ";
                break;
            }
        }
        cout << endl;
    }
    cout << "-*-*-*-*-*-*-*-*-\n";


}
bool  game_state::check_move(class move& m) {
    position o; o = m.get_from();
    position n; n = m.get_to();

    int old_col = o.col;
    int old_row = o.row;

    int new_col = n.col;
    int new_row = n.row;

    int col_midpoint = (o.col + n.col) / 2;
    int row_midpoint = (o.row + n.row) / 2;


    int x_offset = n.col - o.col;
    int y_offset = n.row - o.row;

    if (is_game_over() == true)
        //        cout << "ERROR gameover" << endl;
        return false;
    if (!o.is_valid() || !n.is_valid()) //if either positions is not valid return false
        //        cout << "old or new position is not valid" << endl;
        return false;
    if (grid[old_col][old_row] == EMPTY)  //if from is empty return false
        //        cout << "old position is empty" << endl;
        return false;
    if (grid[new_col][new_row] != EMPTY) //if "to position" is not empty return false
        //        cout << "new position is occupied" << endl;
        return false;
    if (!is_green_turn() && (grid[old_col][old_row] == GREEN_PAWN || grid[old_col][old_row] == GREEN_KING))
        //        cout << "u cant play a GREEN piece, play RED" << endl;
        return false;
    if (!is_red_turn() && (grid[old_col][old_row] == RED_PAWN || grid[old_col][old_row] == RED_KING))
        //        cout << "u cant play a RED piece, play GREEN" << endl;
        return false;
    if (((grid[old_col][old_row] == GREEN_PAWN) || (grid[old_col][old_row] == RED_PAWN)) && (y_offset == 0))
        //        cout << "pawns cant move horizontally" << endl;
        return false;
    if (((grid[old_col][old_row] == GREEN_PAWN) || (grid[old_col][old_row] == RED_PAWN)) && (x_offset == 0))
        //        cout << "pawns cant move vertically" << endl;
        return false;
    if (((grid[old_col][old_row] == GREEN_KING) || (grid[old_col][old_row] == RED_KING)) && (y_offset == 0))
        //        cout << "kings cant move horizontally" << endl;
        return false;
    if (((grid[old_col][old_row] == GREEN_KING) || (grid[old_col][old_row] == RED_KING)) && (x_offset == 0))
        //        cout << "kings cant move vertically" << endl;
        return false;
    if (((grid[old_col][old_row] == GREEN_PAWN) && (x_offset == 1 || x_offset == -1) && (y_offset == 1)) || ((grid[old_col][old_row] == RED_PAWN) && (x_offset == 1 || x_offset == -1) && (y_offset == -1)))
        //        cout << "pawns wont step backwards" << endl;
        return false;
    if (((grid[old_col][old_row] == GREEN_PAWN) && (x_offset == 2 || x_offset == -2) && (y_offset == 2)) || ((grid[old_col][old_row] == RED_PAWN) && (x_offset == 2 || x_offset == -2) && (y_offset == -2)))
        //        cout << "pawns wont leap backwards" << endl;
        return false;
    if (((x_offset > 2 || x_offset < -2) || (y_offset > 2 || y_offset < -2)) || ((x_offset == 1 || x_offset == -1) && (y_offset == 2 || y_offset == -2)) || ((x_offset == 2 || x_offset == -2) && (y_offset == 1 || y_offset == -1)))
        //        cout << "maximum steps exceeded" << endl;
        return false;
    if (((x_offset == 2) || (x_offset == -2)) && (grid[col_midpoint][row_midpoint] == EMPTY))
        //        cout << "cant leap over an empty position" << endl;
        return false;
    if (((x_offset == 2) || (x_offset == -2)) && ((grid[col_midpoint][row_midpoint] == GREEN_PAWN) || (grid[col_midpoint][row_midpoint] == GREEN_KING)) && ((grid[old_col][old_row] == GREEN_PAWN) || (grid[old_col][old_row] == GREEN_KING)))
        //        cout << "green team friendly fire " << endl;
        return false;
    if (((x_offset == 2) || (x_offset == -2)) && ((grid[col_midpoint][row_midpoint] == RED_PAWN) || (grid[col_midpoint][row_midpoint] == RED_KING)) && ((grid[old_col][old_row] == RED_PAWN) || (grid[old_col][old_row] == RED_KING)))
        //        cout << "red team friendly fire " << endl;
        return false;

    return true;
}
bool  game_state::check_move_evaluate(class move& m) {
    position o; o = m.get_from();
    position n; n = m.get_to();

    int old_col = o.col;
    int old_row = o.row;

    int new_col = n.col;
    int new_row = n.row;

    int col_midpoint = (o.col + n.col) / 2;
    int row_midpoint = (o.row + n.row) / 2;


    int x_offset = n.col - o.col;
    int y_offset = n.row - o.row;

    if (is_game_over() == true)
        return false;
    if (!o.is_valid() || !n.is_valid())
        //if either positions is not valid return false
        return false;
    if (grid[old_col][old_row] == EMPTY)
        //if from is empty return false
        return false;
    if (grid[new_col][new_row] != EMPTY)
        //if "to position" is not empty return false
        return false;
    if (((grid[old_col][old_row] == GREEN_PAWN) || (grid[old_col][old_row] == RED_PAWN)) && (y_offset == 0))
        //        cout << "pawns cant move horizontally" << endl;
        return false;
    if (((grid[old_col][old_row] == GREEN_PAWN) || (grid[old_col][old_row] == RED_PAWN)) && (x_offset == 0))
        //        cout << "pawns cant move vertically" << endl;
        return false;
    if (((grid[old_col][old_row] == GREEN_KING) || (grid[old_col][old_row] == RED_KING)) && (y_offset == 0))
        //        cout << "kings cant move horizontally" << endl;
        return false;
    if (((grid[old_col][old_row] == GREEN_KING) || (grid[old_col][old_row] == RED_KING)) && (x_offset == 0))
        //        cout << "kings cant move vertically" << endl;
        return false;
    if (((grid[old_col][old_row] == GREEN_PAWN) && (x_offset == 1 || x_offset == -1) && (y_offset != -1)) || ((grid[old_col][old_row] == RED_PAWN) && (x_offset == 1 || x_offset == -1) && (y_offset != 1)))
        //        cout << "pawns wont step back" << endl;
        return false;
    if (((x_offset > 2 || x_offset < -2) || (y_offset > 2 || y_offset < -2)) || ((x_offset == 1 || x_offset == -1) && (y_offset == 2 || y_offset == -2)) || ((x_offset == 2 || x_offset == -2) && (y_offset == 1 || y_offset == -1)))
        //        cout << "maximum steps exceeded" << endl;
        return false;
    if (((x_offset == 2) || (x_offset == -2)) && (grid[col_midpoint][row_midpoint] == EMPTY))
        //        cout << "cant leap over an empty position" << endl;
        return false;
    if (((x_offset == 2) || (x_offset == -2)) && ((grid[col_midpoint][row_midpoint] == GREEN_PAWN) || (grid[col_midpoint][row_midpoint] == GREEN_KING)) && ((grid[old_col][old_row] == GREEN_PAWN) || (grid[old_col][old_row] == GREEN_KING)))
        //        cout << "green team friendly fire " << endl;
        return false;
    if (((x_offset == 2) || (x_offset == -2)) && ((grid[col_midpoint][row_midpoint] == RED_PAWN) || (grid[col_midpoint][row_midpoint] == RED_KING)) && ((grid[old_col][old_row] == RED_PAWN) || (grid[old_col][old_row] == RED_KING)))
        //        cout << "red team friendly fire " << endl;
        return false;

    return true;
}
void  game_state::make_move(class move& m) {
    position o, n;
    o = m.get_from();
    n = m.get_to();

    int x_offset = n.col - o.col;

    int old_col = o.col;
    int old_row = o.row;

    int new_col = n.col;
    int new_row = n.row;

    int col_midpoint = (o.col + n.col) / 2;
    int row_midpoint = (o.row + n.row) / 2;

    if (!check_move(m))
        return;
    else if (((x_offset == 2) || (x_offset == -2))) { // if the move is a leap
        increment_move();

        grid[new_col][new_row] = grid[old_col][old_row]; //Update the position
        grid[col_midpoint][row_midpoint] = EMPTY;  //clear the attacked position
        grid[old_col][old_row] = EMPTY; //clear the old position
    }
    else { // if its a normal move
        increment_move();
        grid[new_col][new_row] = grid[old_col][old_row];
        grid[old_col][old_row] = EMPTY;
    }

    for (int i = 0; i < 4; i++) {   // this loop promotes pawns to kings if they reach the end
        for (int j = 0; j < 4; j++) { //O(n^2)

            if (grid[j][0] == GREEN_PAWN)
                grid[j][0] = GREEN_KING;
            else if (grid[j][3] == RED_PAWN)
                grid[j][3] = RED_KING;
        }
    }

    if (turn == true) //switch the turns
        turn = false;
    else
        turn = true;

}
move_list* game_state::find_moves() {
    class move  possible_move;
    position current, possible_pos;
    move_list* head = NULL;
    move_list* m;

    if (is_green_turn()) {
        for (int i = 0; i < 4; i++) { //Loop to find green pieces, constructs possibles moves based on their positions
            for (int j = 0; j < 4; j++) { // O(n^2)
                if ((grid[j][i] != EMPTY) && ((grid[j][i] == GREEN_PAWN) || (grid[j][i] == GREEN_KING))) {
                    current.col = j;
                    current.row = i;
                    possible_move.set_from(current);
                    for (int r = 0; r < 4; r++) {
                        for (int e = 0; e < 4; e++) {
                            if (grid[e][r] == EMPTY) {
                                possible_pos.col = e;
                                possible_pos.row = r;
                                possible_move.set_to(possible_pos);
                            }
                            if (check_move(possible_move) == true) {
                                m = new move_list(possible_move);
                                if (head == NULL)
                                    head = m;
                                else
                                    head = head->add(m);
                            }
                        }
                    }
                }
            }
        }
    }
    else {
        for (int i = 0; i < 4; i++) { //Loop to find red pieces, constructs possibles moves based on their positions
            for (int j = 0; j < 4; j++) { //O(n^2)
                if ((grid[j][i] != EMPTY) && ((grid[j][i] == RED_PAWN) || (grid[j][i] == RED_KING))) {
                    current.col = j;
                    current.row = i;
                    possible_move.set_from(current);
                    for (int r = 0; r < 4; r++) {
                        for (int e = 0; e < 4; e++) {
                            if (grid[e][r] == EMPTY) {
                                possible_pos.col = e;
                                possible_pos.row = r;
                                possible_move.set_to(possible_pos);
                            }
                            if (check_move(possible_move)) {
                                m = new move_list(possible_move);
                                if (head == NULL)
                                    head = m;
                                else
                                    head = head->add(m);
                            }
                        }
                    }
                }
            }
        }
    }

    return head;
}
void  game_state::print_list(move_list* p) {
    //cout << "printting list: " << endl;
    while (p != NULL) {
        cout << p->mv.to_text() << " " << endl;
        p = p->next;
    }
}
int  game_state::evaluate() {

    //king = +6
    //pawn = +4
    //Leap = +4
    //blocked = -2

    int score = 0;
    position p_from, p_to;
    class move p_move;


    if (is_green_turn()) {

        //This loop scans the grid and looks for features that increases/decreases the player's position in the game.
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {

                p_from.from_num(j, i);
                piece top_right = grid[j + 1][i - 1];
                piece top_left = grid[j - 1][i - 1];
                piece bot_right = grid[j + 1][i + 1];
                piece bot_left = grid[j - 1][i + 1];

                //look for green pawn
                if (grid[j][i] == GREEN_PAWN)
                    score += 4;

                // look for a green king
                else if (grid[j][i] == GREEN_KING)
                    score += 6;
                //check for block threats
                if (find_pos_moves(&p_from) == NULL && ((grid[j][i] == GREEN_PAWN) || (grid[j][i] == GREEN_KING)))
                    score -= 4;

                //check surroundings for leaps
                if (top_left != EMPTY && top_left != GREEN_KING && top_left != GREEN_PAWN && (top_left == RED_PAWN || top_left == RED_KING)) {
                    p_from.col = j;
                    p_from.row = i;
                    if (p_from.is_valid()) {
                        p_move.set_from(p_from);
                        p_to.col = j - 2;
                        p_to.row = i - 2;
                        if (p_to.is_valid()) {
                            p_move.set_to(p_to);
                            if (check_move(p_move))
                                score += 4;

                        }
                    }
                }
                if (top_right != EMPTY && top_right != GREEN_KING && top_right != GREEN_PAWN && (top_right == RED_PAWN || top_right == RED_KING)) {
                    p_from.col = j;
                    p_from.row = i;
                    if (p_from.is_valid()) {
                        p_move.set_from(p_from);
                        p_to.col = j + 2;
                        p_to.row = i + 2;
                        if (p_to.is_valid()) {
                            p_move.set_to(p_to);
                            if (check_move(p_move))
                                score += 4;
                        }
                    }
                }
                if (bot_left != EMPTY && bot_left != GREEN_KING && bot_left != GREEN_PAWN && (bot_left == RED_PAWN || bot_left == RED_KING) && grid[j][i] == GREEN_KING) {
                    p_from.col = j;
                    p_from.row = i;
                    if (p_from.is_valid()) {
                        p_move.set_from(p_from);
                        p_to.col = j - 2;
                        p_to.row = i + 2;
                        if (p_to.is_valid()) {
                            p_move.set_to(p_to);
                            if (check_move(p_move))
                                score += 4;

                        }
                    }
                }
                if (bot_right != EMPTY && bot_right != GREEN_KING && bot_right != GREEN_PAWN && (bot_right == RED_PAWN || bot_right == RED_KING) && grid[j][i] == GREEN_KING) {
                    p_from.col = j;
                    p_from.row = i;
                    if (p_from.is_valid()) {
                        p_move.set_from(p_from);
                        p_to.col = j + 2;
                        p_to.row = i + 2;
                        if (p_to.is_valid()) {
                            p_move.set_to(p_to);
                            if (check_move(p_move))
                                score += 4;
                        }
                    }
                }
            }
        }

    }
    else {  //if its red's turn

        //This loop scans the grid and looks for features that increases/decreases the player's position in the game.
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {

                p_from.from_num(j, i);

                piece top_right = grid[j + 1][i - 1];
                piece top_left = grid[j - 1][i - 1];
                piece bot_right = grid[j + 1][i + 1];
                piece bot_left = grid[j - 1][i + 1];

                //look for a red pawn
                if (grid[j][i] == RED_PAWN)
                    score += 4;

                //look for a red king
                else if (grid[j][i] == RED_KING)
                    score += 6;

                //Check if blocked
                if (find_pos_moves(&p_from) == NULL && ((grid[j][i] == RED_PAWN) || (grid[j][i] == RED_KING)))
                    score -= 4;

                //check surroundings for leaps
                if (top_left != EMPTY && top_left != RED_KING && top_left != RED_PAWN && (top_left == GREEN_PAWN || top_left == GREEN_KING) && grid[j][i] == RED_KING) {
                    p_from.col = j;
                    p_from.row = i;
                    if (p_from.is_valid()) {
                        p_move.set_from(p_from);
                        p_to.col = j - 2;
                        p_to.row = i - 2;
                        if (p_to.is_valid()) {
                            p_move.set_to(p_to);
                            if (check_move(p_move))
                                score += 4;
                        }
                    }
                }
                if (top_right != EMPTY && top_right != RED_KING && top_right != RED_PAWN && (top_right == GREEN_PAWN || top_right == GREEN_KING) && grid[j][i] == RED_KING) {
                    p_from.col = j;
                    p_from.row = i;
                    if (p_from.is_valid()) {
                        p_move.set_from(p_from);
                        p_to.col = j + 2;
                        p_to.row = i - 2;
                        if (p_to.is_valid()) {
                            p_move.set_to(p_to);
                            if (check_move(p_move))
                                score += 4;
                        }
                    }
                }
                if (bot_left != EMPTY && bot_left != RED_KING && bot_left != RED_PAWN && (bot_left == GREEN_PAWN || bot_left == GREEN_KING)) {
                    p_from.col = j;
                    p_from.row = i;
                    if (p_from.is_valid()) {
                        p_move.set_from(p_from);
                        p_to.col = j - 2;
                        p_to.row = i + 2;
                        if (p_to.is_valid()) {
                            p_move.set_to(p_to);
                            if (check_move(p_move))
                                score += 4;
                        }
                    }
                }
                if (bot_right != EMPTY && bot_right != RED_KING && bot_right != RED_PAWN && (bot_right == GREEN_PAWN || bot_right == GREEN_KING)) {
                    p_from.col = j;
                    p_from.row = i;
                    if (p_from.is_valid()) {
                        p_move.set_from(p_from);
                        p_to.col = j + 2;
                        p_to.row = i + 2;
                        if (p_to.is_valid()) {
                            p_move.set_to(p_to);
                            if (check_move(p_move))
                                score += 4;
                        }
                    }
                }
            }
        }
    }



    return score;

}
move_list* game_state::find_pos_moves(position* p) {
    class move  possible_move;
    position  possible_pos;
    move_list* head = NULL;
    move_list* m;

    possible_move.set_from(*p);

    for (int r = 0; r < 4; r++) {  //this loop finds the possible new position
        for (int e = 0; e < 4; e++) {
            if (grid[e][r] == EMPTY) { // if the position is empty
                possible_pos.col = e;
                possible_pos.row = r;
                possible_move.set_to(possible_pos);
            }
            if (check_move_evaluate(possible_move)) {
                m = new move_list(possible_move);
                if (head == NULL)
                    head = m;
                else
                    head = head->add(m);
            }
        }
    }
    return head;
}
int game_state::minimax(game_state& state, int depth, bool maximizing_player, int alpha, int beta) {
    if (depth == 0 || state.is_game_over()) {
        int score = state.evaluate();
        std::cout << "Score at depth " << depth << ": " << score << std::endl;  // Print score
        return score;

    }

    int value;
    move_list* moves = state.find_moves();
    move_list* current_move = moves;

    if (maximizing_player) {
        value = INT_MIN;
        while (current_move != NULL) {
            game_state child_state = state;
            child_state.make_move(current_move->mv);
            int child_value = minimax(child_state, depth - 1, false, alpha, beta);
            value = max(value, child_value);
            alpha = max(alpha, value);
            if (alpha >= beta) {
                break; // beta cut-off
            }
            current_move = current_move->next;
        }
    }
    else {
        value = INT_MAX;
        while (current_move != NULL) {
            game_state child_state = state;
            child_state.make_move(current_move->mv);
            int child_value = minimax(child_state, depth - 1, true, alpha, beta);
            value = min(value, child_value);
            beta = min(beta, value);
            if (alpha >= beta) {
                break; // alpha cut-off
            }
            current_move = current_move->next;
        }
    }

    return value;
}

void game_state::computer_move() {
    int depth = 4; // Adjust the search depth as needed
    move_list* moves = find_moves();
    move_list* best_move = NULL;
    int best_value = INT_MIN;

    while (moves != NULL) {
        game_state child_state = *this;
        child_state.make_move(moves->mv);
        int value = minimax(child_state, depth - 1, false, INT_MIN, INT_MAX);

        if (value > best_value) {
            best_value = value;
            best_move = moves;
        }

        moves = moves->next;
    }

    if (best_move != NULL) {
        make_move(best_move->mv);
    }
}

