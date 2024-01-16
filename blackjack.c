//Sam Durigon Project 2
//CS 1550
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

//global game variables

//file descriptor for our deck
int deck;
//score and card holders
int playerScore, dealerScore, playerCard, dealerCard, playerAces, dealerAces, playerHand;
//boolean value
int gameInPlay, dealerBlackjack, playerBlackjack, bust, choice;

int main(){

    initialize_game();
    play_game();
    return 0;
}

//create the deck and output the tutorial message
void initialize_game(){
    //int count;
    deck = open("/dev/cards", O_RDONLY);
    //keep track of total score
    playerScore = 0;
    dealerScore = 0;
    //stores most recent card drawn
    playerCard = 0;
    dealerCard = 0;
    //tracks aces
    playerAces = 0;
    dealerAces = 0;
    //cards in player's hand
    playerHand = 0;
    //booleans
    gameInPlay = 1;
    dealerBlackjack = 0;
    bust = 1;
    choice = 0;


    printf("\nWelcome to the game of BlackJack! I am your dealer, John White.\n");
    printf("Let's get started! Your only opponent is me. I'll deal each of us two cards\n");
    printf("but you can only see one of mine.\n");
    printf("Face cards are worth 10, Numbered cards are worth their value, and \n");
    printf("Aces are worth either 1 or 11 depending on the situation. \n");
    printf("This can change at any time. The goal of the game is to get closer to 21 \n");
    printf("than I do without going over, or \"Busting\".After the initial deal, \n");
    printf("it is your turn first. You can choose to \"hit\" and draw another\n");
    printf("card, or you can \"Stand\" and draw no more cards, passing play to me.\n");
    printf("As the dealer, I must hit on any hand 16 and below, and must stand on \n");
    printf("any hand 17 and above.\nSince I am not real, and thus don't have real money, \n");
    printf("we will not bet on this game.\nBest of luck, let's get started!\n\n\n");
}


//main gameplay section
void play_game(){

    //deal the starting cards
    dealerScore = draw_card(dealerScore);
    playerScore = draw_card(playerScore);
    playerHand++;
    dealerCard = draw_card(dealerScore);

    //assign any dealer aces
    if (dealerScore == 11){
        dealerAces++;
    }
    if (dealerCard == 11){
        dealerAces++;
    }

    
    //print the dealer's current hand (not including the hidden card)
    printf("My hand is: \n ? + %d\n", dealerScore);

    if (playerScore == 11){
        playerAces++;
        printf("You drew an Ace!\n");
    }
    //check if dealer has blackjack
    if (dealerScore + dealerCard == 21){
        printf("My hidden card is a(n) %d, which brings me to 21.\n", dealerCard);
        dealerBlackjack = 1;
        blackjack_case(0);
        return;
    }

    //normal game loop
    while (bust){

        //draw a card for the player
        playerCard = draw_card(playerScore);
        playerHand++;
        //if the card is an ace, add to the ace counter
        if (playerCard == 11){
            playerAces++;
            printf("You drew an Ace!\n");
        }
        //print the new total for the player
        printf("Your hand is:\n %d + %d = %d\n", playerScore, playerCard, playerScore+playerCard);
        playerScore += playerCard;

        //if player's hand is bust
        if (playerScore > 21){
            //determine if aces should be reevaluated
            if (playerAces > 0){
                playerScore -= 10;
                playerAces--;
                printf("You busted, but an Ace can be treated as a 1!\n");
                printf("Your score is now: %d\n", playerScore);

                //give player choice to hit or stand
                choice = player_choice();
                if(choice == 1){ //player hits
                    printf("Here's another card...\n\n");
                    printf("\n<----------------------------------------------------->\n\n");

                    continue;
                }else if (choice == 2){ //player stands
                    printf("You've chosen to stand, let's see where your luck takes you...\n");
                    printf("\n<----------------------------------------------------->\n\n");
                    break;
                }
            //else if they don't have aces, proceed to dealer's turn
            }else{
                gameInPlay = 0;
                bust = 0;
                break;
            }

        //else if player has a blackjack
        }else if (playerHand == 2 && playerScore == 21){
            blackjack_case(1);
            return;

        //else if player's hand is not busted
        }else{
            choice = player_choice();
            //if player choice is 1: HIT
            if (choice == 1){
                printf("Here's another card...\n\n");
                printf("\n<----------------------------------------------------->\n\n");
            //else, STAND
            }else if(choice == 2){
                printf("You've chosen to stand, let's see where your luck takes you...\n");
                printf("\n<----------------------------------------------------->\n\n");
                bust = 0;
                break;
            }
        }

    }//end while

    //dealer plays until he busts or is forced to stand

    //dealer shows hand
    printf("\n<----------------------------------------------------->\n\n");
    printf("My hand is:\n %d + %d = %d.\n", dealerCard, dealerScore, dealerScore+dealerCard);
    dealerScore += dealerCard;
    if (gameInPlay == 0){
        check_win();
        return;
    }
    while(1){
        printf("\n\n");
        //if the dealer has a 16 or lower, they hit
        if (dealerScore < 17){
            printf("Since my hand is under 17, I have to hit.\n");
            dealerCard = draw_card(dealerScore);
            if (dealerCard == 11){
                dealerAces++;
                printf("I drew an Ace!\n");
            }
            printf("\n<----------------------------------------------------->\n\n");
            printf("My new total is:\n %d + %d = %d\n", dealerScore, dealerCard, dealerScore+dealerCard);
            dealerScore += dealerCard;
            continue;

        //else if the dealer has a 17 or higher without busting, they stand
        }else if(dealerScore >= 17 && dealerScore <= 21){
            printf("My hand is over 17, so I stand! Let's check scores.\n");
            printf("\n<----------------------------------------------------->\n\n");
            check_win();
            return;
        //else if dealer busts, check if they have an ace and change the score accordingly
        }else if (dealerScore > 21){
            if (dealerAces > 0){
                dealerScore -= 10;
                dealerAces--;
                printf("I busted, but an Ace can be treated as a 1!\n");
                printf("My score is now: %d\n", dealerScore);
            //else if they don't have aces, check scores
            }else{
                printf("\n<----------------------------------------------------->\n\n");
                check_win();
                return;
            }
        }//end if
    }//end while
}//end method

//read a card, interpret its value, and return it to the game
int draw_card(int score){
    char card[1];
    int cardValue;

    read(deck, card, 1);
    cardValue = card[0]%13;

    switch (cardValue){
        //card is an ace
        case 0:
            cardValue = 11;
            break;
        //card is a jack
        case 10:
            cardValue = 10;
            break;
        //card is a queen
        case 11:
            cardValue = 10;
            break;
        //card is a kings
        case 12:
            cardValue = 10;
            break;
        //card is a normal number
        default:
            cardValue += 1;
            break;
    }
    return cardValue;

}

//only runs if either player or dealer busts or stands
void check_win(){
    
    //if player busts, dealer wins automatically
    if(playerScore > 21){
        printf("You busted, so the game is over! Better luck next time!\n");
        return;
    //else if dealer busts, player wins automatically
    }else if(dealerScore > 21){
        printf("I busted, so you win with a %d! Great job!\n", playerScore);
        return;
    //else if both players stand, check higher score
    }else{
        //if player is higher, player wins
        if(playerScore > dealerScore){
            printf("Damn, your %d is higher than my %d, so you win! Great job!\n", playerScore, dealerScore);
            return;
        //else if dealer is higher, dealer wins
        }else if (dealerScore > playerScore){
            printf("My %d is higher than your %d, so I win! Better luck next time!\n", dealerScore, playerScore);
            return;
        //else if player and dealer are the same, its a PUSH
        }else if (dealerScore == playerScore){
            printf("We got the same score, so it's a PUSH! Good game!\n");
            return;

        }
    }
}

//returns a boolean
int player_choice(){
    char input;
    while(1){
        printf("\nPlease type \"H\" to Hit and draw a card, or \"S\" to Stand and keep your cards.\n");
        scanf(" %c", &input);
        input = toupper(input);
        //player chooses to hit
        if(input == 'H'){
            return 1;
        //player chooses to stand
        }else if (input == 'S'){
            return 2;
        }else{
            printf("Not an option.\n");
        }
    }

}

//either player gets a blackjack, game is over
void blackjack_case(int scene){

    //dealer has a blackjack
    if (scene == 0){
        printf("Since I have a Blackjack, you also need one to win...\n");
        playerCard = draw_card(playerScore);
        //print the new total for the player
        printf("Your hand is:\n %d + %d = %d\n", playerScore, playerCard, playerScore+playerCard);
        playerScore += playerCard;
        //if both have blackjack, then its a PUSH
        if (playerScore == 21){
            printf("We both have a Blackjack, so neither of us win. Good game!\n");
            return;
        }else{
            printf("You didn't get a Blackjack, so I win. Better luck next time!\n");
            return;
        }
    //player has a blackjack
    }else if (scene == 1){
        printf("You got a Blackjack!\n");
        printf("My hand is: \n%d + %d = %d\nYou won! Great Job!\n", dealerScore, dealerCard, dealerScore+dealerCard);
        return;
    }
}
