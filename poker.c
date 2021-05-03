#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "poker.h"

/* Global instances of the two players */
struct player P1, P2;

/* Parse card from a 2 char input */
struct card parse(const char *card)
{
	struct card c;
	switch(card[0]) {
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': c.val = card[0] - '0'; break;
	case 'T': c.val = TEN; break;
	case 'J': c.val = JACK; break;
	case 'Q': c.val = QUEEN; break;
	case 'K': c.val = KING; break;
	case 'A': c.val = ACE; break;
	default: break;
	}
	switch(card[1]) {
	case 'S': c.suit = SPADE; break;
	case 'D': c.suit = DIAMOND; break;
	case 'C': c.suit = CLUB; break;
	case 'H': c.suit = HEART; break;
	default: break;
	}
	return c;
}

/* Count the number of occurrences of each card 2 through Ace */
void count_cards(struct hand *h)
{
	/* TASK 6: For each card in the hand, increment the card_count for that card. */
	int i;

	/* Set each count to 0 */
	for (i = 0; i < 13; i++) {h -> card_count[i] = 0;}

	/* For each card, increment the card count */
	for (i = 0; i < 5; i++) {
		unsigned int index = (int) h -> cards[i].val;
		h -> card_count[index - 2] += 1;
	}
	
	/* ref_count_cards(h); */ 
}

int is_flush(struct hand *h)
{
	/* TASK 9: Implement is_flush(). A hand is a flush if all the cards are of the same suit */
	/* Return 1 if flush, 0 otherwise */

	/* Save the suit of the first card */
	enum Suit card_suit = h -> cards[0].suit;
	int i;

	/* Match each suit with the suit saved above, return 0 if a mismatch */
	for (i = 0; i < 5; i++) {
		if (h -> cards[i].suit != card_suit) {
			return 0;
		}
	}

	/* Is flush if no mismatch found, so return 0 */
	return 1;
	
	/* return ref_is_flush(h); */

}

int is_straight(struct hand *h)
{
	int n = 0;
	int i;
	/* Increment n each time a card is equal to the next one */
	for (i = 0; i < 12; i++) {
		if ((h -> card_count[i] == 1) && 
			(h -> card_count[i+1] == 1)) {
			n++;
		}
	}

	/* If n is 4, that means all 5 cards match */
	if (n == 4) {
		return 1;
	}

	/* Handle exception case A2345 */

	if ((h -> card_count[12] == 1) && 
			(h -> card_count[0] == 1) &&
			(h -> card_count[1] == 1) &&
			(h -> card_count[2] == 1) &&
			(h -> card_count[3] == 1)) {
				h -> vector = h -> vector ^ (1 << 12);
				return 1;
			}

	return 0;
	/* return ref_is_straight(h); */

}
		
/* This important function converts a player's hand into weighted unsigned long number. Larger the number, stronger the hand. 
It is a bit vector as shown below (2 is the LSB and StraightFlush is the MSB) */
/* 	2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K, A, 
	22, 33, 44, 55, 66, 77, 88, 99, TT, JJ, QQ, KK, AA, 
	222, 333, 444, 555, 666, 777, 888, 999, TTT, JJJ, QQQ, KKK, AAA, Straight, Flush, Full House, 
	2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999, TTTT, JJJJ, QQQQ, KKKK, AAAA, StraightFlush */
/* The number of occurrences of each number in the hand is first calculated in count_cards(). 
Then, depending on the number of occurrences, the corresponding bit in the bit-vector is set. 
In order to find the winner, a simple comparison of the bit vectors (i.e., unsigned long integers) will suffice! */
void eval_strength(struct hand *h)
{
	/* TASK 5: Implement the eval_strength function */
	count_cards(h);
	
	int i;
	int pair_found = 0;
	int straight_flush_found = 1;
	int triple_found = 0;

	/* Set bits for cases where a card is single, pair, threes, or fours */
	for(i = 0; i < 13; i++){
		if(h->card_count[i] == 1){
			h->vector |= (1UL << i);	
		}
		else if(h->card_count[i] == 2){
			h->vector |= (1UL << (i+13));	
			pair_found = 1;
			straight_flush_found = 0;
		}
		else if(h->card_count[i] == 3){
			h->vector |= (1UL << (i+26));	
			triple_found = 1;
			straight_flush_found = 0;
		}
		else if(h->card_count[i] == 4){		
			h->vector |= (1UL << (i+42));	
			straight_flush_found = 0;
		}
	}

	/* Set bits for the remaining cases */
	if (straight_flush_found && is_flush(h)){		
		h->vector |= (1UL << 40);
	}
	if(straight_flush_found && is_straight(h)){	
		h->vector |= (1UL << 39);
	}
	if (triple_found && pair_found){				
	 	h->vector |= (1UL << 41); 		
	}

	/* Set bit for the straight flush case */
	if (is_flush(h) && is_straight(h)){	
	 	h->vector |= (1UL << 56);
	}
	/* ref_eval_strength(h); */
}

void eval_players_best_hand(struct player *p)
{
	/* TASK 10: For each hand in the 'hands' array of the player, use eval_strength to evaluate the strength of the hand */
	/*       Point best_hand to the strongest hand. */
	/*       HINT: eval_strength will set the hands vector according to its strength. Larger the vector, stronger the hand. */
	
	/* Eval strength for each hand */
	int i;
	for (i = 0; i < 21; i++) {
		eval_strength(&(p -> hands[i]));
	}

	/* Compare each hands vector with the max vector, changing max accordingly */
	unsigned long max = p -> hands[0].vector;
	p -> best_hand = &(p -> hands[0]);
	for (i = 0; i < 21; i++) {
		if (p -> hands[i].vector > max) {
			max = p -> hands[i].vector;
			p -> best_hand = &(p -> hands[i]);
		}
	}
	
	/* ref_eval_players_best_hand(p); */ 
}

void copy_card(struct card *dst, struct card *src)
{
	/* TASK 3: Implement function copy_card that copies a card structure from src to dst. */
	dst -> val = src -> val;
	dst -> suit = src -> suit;

	/* ref_copy_card(dst, src); */ 
}

void initialize_player(struct player *p, struct card *player_cards, struct card *community_cards)
{
	memset(p, 0, sizeof(struct player));

	/* There are 21 possible 5 card combinations (i.e., 7C5) in Pc1 Pc2 Cc1 Cc2 Cc3 Cc4 Cc5 */
	/* TASK 4: Initialize the 'hands' array in the player structure with the cards. 
	 *       Each element of the hands array will contain one possible combination of the hand.
	 *       Use copy_card function.
	 */ 


	/* P0 P1 C0 C1 C2 */
	copy_card(&(p -> hands[0].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[0].cards[1]),&(player_cards[1]));
	copy_card(&(p -> hands[0].cards[2]),&(community_cards[0]));
	copy_card(&(p -> hands[0].cards[3]),&(community_cards[1]));
	copy_card(&(p -> hands[0].cards[4]),&(community_cards[2]));

	/* P0 P1 C0 C1 C3 */
	copy_card(&(p -> hands[1].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[1].cards[1]),&(player_cards[1]));
	copy_card(&(p -> hands[1].cards[2]),&(community_cards[0]));
	copy_card(&(p -> hands[1].cards[3]),&(community_cards[1]));
	copy_card(&(p -> hands[1].cards[4]),&(community_cards[3]));

	/* P0 P1 C0 C1 C4 */
	copy_card(&(p -> hands[2].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[2].cards[1]),&(player_cards[1]));
	copy_card(&(p -> hands[2].cards[2]),&(community_cards[0]));
	copy_card(&(p -> hands[2].cards[3]),&(community_cards[1]));
	copy_card(&(p -> hands[2].cards[4]),&(community_cards[4]));

	/* P0 P1 C0 C2 C3 */
	copy_card(&(p -> hands[3].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[3].cards[1]),&(player_cards[1]));
	copy_card(&(p -> hands[3].cards[2]),&(community_cards[0]));
	copy_card(&(p -> hands[3].cards[3]),&(community_cards[2]));
	copy_card(&(p -> hands[3].cards[4]),&(community_cards[3]));

	/* P0 P1 C0 C2 C4 */
	copy_card(&(p -> hands[4].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[4].cards[1]),&(player_cards[1]));
	copy_card(&(p -> hands[4].cards[2]),&(community_cards[0]));
	copy_card(&(p -> hands[4].cards[3]),&(community_cards[2]));
	copy_card(&(p -> hands[4].cards[4]),&(community_cards[4]));

	/* P0 P1 C0 C3 C4 */
	copy_card(&(p -> hands[5].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[5].cards[1]),&(player_cards[1]));
	copy_card(&(p -> hands[5].cards[2]),&(community_cards[0]));
	copy_card(&(p -> hands[5].cards[3]),&(community_cards[3]));
	copy_card(&(p -> hands[5].cards[4]),&(community_cards[4]));

	/* P0 P1 C1 C2 C3 */
	copy_card(&(p -> hands[6].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[6].cards[1]),&(player_cards[1]));
	copy_card(&(p -> hands[6].cards[2]),&(community_cards[1]));
	copy_card(&(p -> hands[6].cards[3]),&(community_cards[2]));
	copy_card(&(p -> hands[6].cards[4]),&(community_cards[3]));

	/* P0 P1 C1 C2 C4 */
	copy_card(&(p -> hands[7].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[7].cards[1]),&(player_cards[1]));
	copy_card(&(p -> hands[7].cards[2]),&(community_cards[1]));
	copy_card(&(p -> hands[7].cards[3]),&(community_cards[2]));
	copy_card(&(p -> hands[7].cards[4]),&(community_cards[4]));

	/* P0 P1 C1 C3 C4 */
	copy_card(&(p -> hands[8].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[8].cards[1]),&(player_cards[1]));
	copy_card(&(p -> hands[8].cards[2]),&(community_cards[1]));
	copy_card(&(p -> hands[8].cards[3]),&(community_cards[3]));
	copy_card(&(p -> hands[8].cards[4]),&(community_cards[4]));

	/* P0 P1 C2 C3 C4 */
	copy_card(&(p -> hands[9].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[9].cards[1]),&(player_cards[1]));
	copy_card(&(p -> hands[9].cards[2]),&(community_cards[2]));
	copy_card(&(p -> hands[9].cards[3]),&(community_cards[3]));
	copy_card(&(p -> hands[9].cards[4]),&(community_cards[4]));

	/* P0 C0 C1 C2 C3 */
	copy_card(&(p -> hands[10].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[10].cards[1]),&(community_cards[0]));
	copy_card(&(p -> hands[10].cards[2]),&(community_cards[1]));
	copy_card(&(p -> hands[10].cards[3]),&(community_cards[2]));
	copy_card(&(p -> hands[10].cards[4]),&(community_cards[3]));

	/* P0 C0 C1 C2 C4 */
	copy_card(&(p -> hands[11].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[11].cards[1]),&(community_cards[0]));
	copy_card(&(p -> hands[11].cards[2]),&(community_cards[1]));
	copy_card(&(p -> hands[11].cards[3]),&(community_cards[2]));
	copy_card(&(p -> hands[11].cards[4]),&(community_cards[4]));

	/* P0 C0 C1 C3 C4 */
	copy_card(&(p -> hands[12].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[12].cards[1]),&(community_cards[0]));
	copy_card(&(p -> hands[12].cards[2]),&(community_cards[1]));
	copy_card(&(p -> hands[12].cards[3]),&(community_cards[3]));
	copy_card(&(p -> hands[12].cards[4]),&(community_cards[4]));

	/* P0 C0 C2 C3 C4 */
	copy_card(&(p -> hands[13].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[13].cards[1]),&(community_cards[0]));
	copy_card(&(p -> hands[13].cards[2]),&(community_cards[2]));
	copy_card(&(p -> hands[13].cards[3]),&(community_cards[3]));
	copy_card(&(p -> hands[13].cards[4]),&(community_cards[4]));

	/* P0 C1 C2 C3 C4 */
	copy_card(&(p -> hands[14].cards[0]),&(player_cards[0]));
	copy_card(&(p -> hands[14].cards[1]),&(community_cards[1]));
	copy_card(&(p -> hands[14].cards[2]),&(community_cards[2]));
	copy_card(&(p -> hands[14].cards[3]),&(community_cards[3]));
	copy_card(&(p -> hands[14].cards[4]),&(community_cards[4]));

	/* P1 C0 C1 C2 C3 */
	copy_card(&(p -> hands[15].cards[0]),&(player_cards[1]));
	copy_card(&(p -> hands[15].cards[1]),&(community_cards[0]));
	copy_card(&(p -> hands[15].cards[2]),&(community_cards[1]));
	copy_card(&(p -> hands[15].cards[3]),&(community_cards[2]));
	copy_card(&(p -> hands[15].cards[4]),&(community_cards[3]));

	/* P1 C0 C1 C2 C4 */
	copy_card(&(p -> hands[16].cards[0]),&(player_cards[1]));
	copy_card(&(p -> hands[16].cards[1]),&(community_cards[0]));
	copy_card(&(p -> hands[16].cards[2]),&(community_cards[1]));
	copy_card(&(p -> hands[16].cards[3]),&(community_cards[2]));
	copy_card(&(p -> hands[16].cards[4]),&(community_cards[4]));

	/* P1 C0 C1 C3 C4 */
	copy_card(&(p -> hands[17].cards[0]),&(player_cards[1]));
	copy_card(&(p -> hands[17].cards[1]),&(community_cards[0]));
	copy_card(&(p -> hands[17].cards[2]),&(community_cards[1]));
	copy_card(&(p -> hands[17].cards[3]),&(community_cards[3]));
	copy_card(&(p -> hands[17].cards[4]),&(community_cards[4]));

	/* P1 C0 C2 C3 C4 */
	copy_card(&(p -> hands[18].cards[0]),&(player_cards[1]));
	copy_card(&(p -> hands[18].cards[1]),&(community_cards[0]));
	copy_card(&(p -> hands[18].cards[2]),&(community_cards[2]));
	copy_card(&(p -> hands[18].cards[3]),&(community_cards[3]));
	copy_card(&(p -> hands[18].cards[4]),&(community_cards[4]));

	/* P1 C1 C2 C3 C4 */
	copy_card(&(p -> hands[19].cards[0]),&(player_cards[1]));
	copy_card(&(p -> hands[19].cards[1]),&(community_cards[1]));
	copy_card(&(p -> hands[19].cards[2]),&(community_cards[2]));
	copy_card(&(p -> hands[19].cards[3]),&(community_cards[3]));
	copy_card(&(p -> hands[19].cards[4]),&(community_cards[4]));

	/* C0 C1 C2 C3 C4 */
	copy_card(&(p -> hands[20].cards[0]),&(community_cards[0]));
	copy_card(&(p -> hands[20].cards[1]),&(community_cards[1]));
	copy_card(&(p -> hands[20].cards[2]),&(community_cards[2]));
	copy_card(&(p -> hands[20].cards[3]),&(community_cards[3]));
	copy_card(&(p -> hands[20].cards[4]),&(community_cards[4]));

	
	/* ref_initialize_player(p, player_cards, community_cards); */
}

/* Parse each hand in the input file, evaluate the strengths of hands and identify a winner by comparing the weighted vectors */
void process_input(FILE *fp)
{
	char p1[2][3];
	char p2[2][3];
	char comm[5][3];
	struct card p1_cards[2], p2_cards[2], community_cards[5];
	int i;

	while(fscanf(fp, "%s %s %s %s %s %s %s %s %s", 
		&p1[0][0], &p1[1][0], &p2[0][0], &p2[1][0], &comm[0][0], 
		&comm[1][0], &comm[2][0], &comm[3][0], &comm[4][0]) == 9) {

		memset(p1_cards, 0, sizeof(struct card) * 2);
		memset(p2_cards, 0, sizeof(struct card) * 2); 
		memset(community_cards, 0, sizeof(struct card) * 5);

		for(i = 0; i < 2; i++) {
			p1_cards[i] = parse(&p1[i][0]);
			p2_cards[i] = parse(&p2[i][0]);
		}

		for(i = 0; i < 5; i++) 
			community_cards[i] = parse(&comm[i][0]);

		initialize_player(&P1, &p1_cards[0], &community_cards[0]);
		initialize_player(&P2, &p2_cards[0], &community_cards[0]);

		eval_players_best_hand(&P1);
		eval_players_best_hand(&P2);

		/* TASK 11: Check which player has the strongest hand and print either "Player 1 wins" or "Player 2 wins" */
		if (P1.best_hand -> vector > P2.best_hand -> vector) {
			printf("Player 1 wins\n");
		}

		else if (P1.best_hand -> vector < P2.best_hand -> vector) {
			printf("Player 2 wins\n");
		}

		else {
			printf("No single winner\n");
		}
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;

	if(argc != 2 || (fp = fopen(argv[1], "r")) == NULL) {
		printf("Unable to open input file\n");
		exit(-1);
	}
	
	process_input(fp);

	return 0;
}
	