#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/random.h>
#include <asm/uaccess.h>

//global variable for the deck of cards
static char deck[52];
static int top; //top card of the deck

// randomization method from project pdf
unsigned char get_random_byte(int max){
	unsigned char c;
	get_random_bytes(&c, 1);
	return c%max;
}

//create and initialize a deck of 52 cards
void create_deck(void){
	int i = 1;
	while (i < 52){
		//give each 'card' a value from 1 to 52
		deck[i] = i;
		i++;
	}
}

//shuffle the deck of cards 'amt' number of times
void shuffle(int amt){
	int i, j;
	char temp;
	unsigned char random;
	while (amt > 0){
		//for each card in the deck, switch it with the card at a random index
		for (i = 0; i < 52; i++){
			//get a random byte
			random = get_random_byte(52);
			j = random%52;
			//make sure that the index cannot go out of bounds
			if (j==52) j = 0;

			//swap the cards
			temp = deck[i];
			deck[i] = deck[random];
			deck[random] = temp;
		}
		amt--;
	}
}

// void get_cards(int num){
// 	int i;
// 	char card;
// 	for (i = 0; i < num; i++){
// 		card = deck[top];
// 		top++;
// 		printk(KERN_ALERT "%c", card);
// 		printk(KERN_ALERT "\n");
// 	}
// }

//buf is the output where we place the cards we found
//count is th enumber of card deals we've been asked to produce
//ppos is the file pointer to keep track of the next read
static ssize_t card_read(struct file * file, char * buf, size_t count, loff_t *ppos)
{
	char card;
	int numCards;
	for (numCards = 0; numCards < count; numCards++){
		//get a card from the deck and increment the top
		card = deck[top];
		top++;
		copy_to_user(buf + numCards, &card, 1);

		//increment the ppos
		*ppos = *ppos + 1;

	}

	return count;
}

/*
 * The only file operation we care about is read.
 */

static const struct file_operations card_fops = {
	.owner		= THIS_MODULE,
	.read		= card_read,
};

static struct miscdevice card_driver = {
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	/*
	 * Name ourselves /dev/cards.
	 */
	"cards",
	/*
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	&card_fops
};

static int __init card_init(void)
{
	int ret;
	//create the deck
	create_deck();
	//shuffle the new deck
	shuffle(30);
	//point to the top of the deck
	top = 0;

	/*
	 * Create the "card_driver" device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/cards device using
	 * the default rules.
	 */
	ret = misc_register(&card_driver);
	if (ret){ 
		printk(KERN_ERR "Unable to register \"card_driver\" misc device\n");
	}
	return ret;
}

module_init(card_init);

static void __exit card_exit(void)
{
	misc_deregister(&card_driver);
}

module_exit(card_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sam Durigon");
MODULE_DESCRIPTION("\"Card Dealer!\" minimal module");
MODULE_VERSION("dev");