#ifndef _POKER_H
#define _POKER_H


/* Maximum possible combinations of 5 cards from 2 player cards and 5 community cards is 7C5 == 21 */
#define MAX_COMBINATIONS 21

enum Value {
	ONE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN,
	EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE
};

enum Suit {
	CLUB = 0, DIAMOND, HEART, SPADE
};

struct card {
	enum Value val;
	enum Suit suit;
};

struct hand {
	struct card cards[5];
	unsigned int card_count[13]; /* 13 cards in all */
	unsigned long vector; /* This vector is a quantification of the strength of the hand */
};

struct player {
	/* TASK 2: Implement the player structure that contains: 
	 *	(1) an array 'hands' of MAX_COMBINATION number of hands, and 
	 *  (2) a pointer 'best_hand' that points to the strongest hand in the hands array.
	 */
	 struct hand hands[MAX_COMBINATIONS];
	 struct hand *best_hand;
};

/* Reference functions */
void ref_count_cards(struct hand *h);
int ref_is_flush(struct hand *h);
int ref_is_straight(struct hand *h);
void ref_eval_strength(struct hand *h);
void ref_eval_players_best_hand(struct player *p);
void ref_copy_card(struct card *dst, struct card *src);
void ref_initialize_player(struct player *p, struct card *player_cards, struct card *community_cards);

/* TASK 1: Provide a prototype for all the functions used in poker.c */
void count_cards(struct hand *h);
int is_flush(struct hand *h);
int is_straight(struct hand *h);
void eval_strength(struct hand *h);
void eval_players_best_hand(struct player *p);
void copy_card(struct card *dst, struct card *src);
void initialize_player(struct player *p, struct card *player_cards, struct card *community_cards);


#endif
