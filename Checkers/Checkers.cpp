#include "checkers.h"


using namespace std;

int main() {
    game_state myGame;
    class move myMove;


    while (!myGame.is_game_over()) {

        if (myGame.is_red_turn()) {  //if its computer's turn, the computer plays the first move in a move_list
            myGame.computer_move();
        }
        if (myGame.is_green_turn()) {  //if its the player's turn turn
            myGame.display();
            myMove.get_move();
            myGame.make_move(myMove);
        }


        // defeat announcer
        if (myGame.find_moves() == NULL && myGame.is_green_turn()) {
            cout << "Game over, red player wins" << endl;
            break;
        }
        if (myGame.find_moves() == NULL && myGame.is_red_turn()) {
            cout << "Game over, green player wins" << endl;
            break;
        }
    }
    return 0;
}







