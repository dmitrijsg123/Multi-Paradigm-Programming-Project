#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/****************************************** MACROS ******************************************************/

#define MAX_PRODUCTS_NUMBER         256
#define MAX_ORDER_PRODUCTS_NUMBER   256

#define SHOP_CSV_FILE_NAME          "stock.csv"

#define MAX_CSV_FILE_LINE_LENGTH   2048
#define MAX_PRODUCT_NAME_LENGTH    MAX_CSV_FILE_LINE_LENGTH
#define MAX_CUSTOMER_NAME_LENGTH   MAX_CSV_FILE_LINE_LENGTH

/****************************************** TYPES ******************************************************/

struct Product {
	char name[MAX_PRODUCT_NAME_LENGTH+1];
	double price;
};

struct ProductStock {
	struct Product product;
	int quantity;
};

struct Shop {
	double cash;
	struct ProductStock stock[MAX_PRODUCTS_NUMBER];
	int productsNum;
	int lifeMode;
};

struct Customer {
	char name[MAX_CUSTOMER_NAME_LENGTH+1];
	double budget;
	struct ProductStock shoppingList[MAX_ORDER_PRODUCTS_NUMBER];
	int productsNum;
};

/****************************************** PROTOTYPES ************************************************************/

void runLifeModeMenu();

/****************************************** GLOBAL VARIABLES ******************************************************/

struct Shop shop;

/****************************************** LOCAL PROCEDURES ******************************************************/

/* Print product info, only flag means that we are only displaying this struct, when 0 we are displaying a list of information, change formatting */

void printProduct(struct Product* p, int only)
{
	if (only)printf("-------------\n");
	printf("PRODUCT NAME: %s \nPRODUCT PRICE: %.2f %c \n", p->name, p->price, only?'\n':' ');
	if (only)printf("-------------\n");
}

/* Print product stock info, only flag means that we are only displaying this struct, when 0 we are displaying a list of information, change formatting */

void printProductStock(struct ProductStock* ps, int only)
{
	if(only)printf("-------------\n");
	if(only)printf("PRODUCT INFO:\n");
	printProduct(&ps->product, only);
	printf("QUANTITY: %d\n \n", ps->quantity);
	if(only)printf("-------------\n");
}

/* Print whole shop stock */

void printShopStock(struct Shop* s)
{
	printf("----------------\n");
	printf("SHOP STOCK INFO\n");
	printf("----------------\n");
	printf("CASH: %.2f\n", shop.cash);
	for (int i = 0; i < shop.productsNum; i++) {
		printProductStock(&s->stock[i], 0);
	}
	printf("----------------\n");
}

/* Print whole customer info */

void printCustomerOrder(struct Customer* c)
{
	printf("\n-------------------\n");
	printf("CUSTOMER ORDER INFO\n");
	printf("------------------------------------------------------\n");
	printf("NAME       : %s\n", c->name);
	printf("BUDGET     : %.2f\n", c->budget);
	printf("PRODUCTSNUM: %d\n", c->productsNum);
	for (int i = 0; i < c->productsNum; i++) {
		printf("PRODUCT: %s  QUANTITY: %d\n", c->shoppingList[i].product.name, c->shoppingList[i].quantity);
	}
	printf("-------------------------------------------------------\n");
}

/* Remove \r \n if present */

void trimNewLineChars(char* line)
{
	char* pos = strchr(line, '\r');
	if (pos) *pos = 0;
	pos = strchr(line, '\n');
	if (pos) *pos = 0;
}

/* Remove any leading or trailing space from string */

void trimSpaces(char* field)
{
	char* c;
	size_t fieldLength = strlen(field);
	size_t pos = (fieldLength - 1);

	// trim trailing
	while ((pos >=0) && (*(c=&field[pos]) == ' ')) {
		field[pos] = 0;
		pos++;
	}
	// trim leading
	pos = 0;
	while (field[pos] == ' ') {
		pos++;
	}
	// move now the string to the left when any leading space removed
	if (pos) {
		memmove(field, &field[pos], (fieldLength - pos) +1);
	}
}

/* Parse info present in a product line =>   Product, price, quantity   i.e.    "Coke Can, 1.10, 100" . Return 0 when success, 1 otherwise */

int parseShopProductLine(char* line)
{
	char* token;

	// shortcut for next product to store
	int id = shop.productsNum;

	// we expect 3 tokens:  Product name, price, quantity
	// Token 1 - Product name
	token = strtok(line,  ",");
	if (!token) {
		printf("\nERROR: Invalid product line [%s] at shop file (%s). Missing product name\n", line, SHOP_CSV_FILE_NAME);
		return 1;
	}
	trimSpaces(token);
	// Store it in shop
	snprintf(shop.stock[id].product.name, MAX_PRODUCT_NAME_LENGTH, "%s", token);

	// Token 2 - Product price
	token = strtok(NULL, ",");
	if (!token) {
		printf("\nERROR: Invalid product line [%s] at shop file (%s). Missing product price\n", line, SHOP_CSV_FILE_NAME);
		return 1;
	}
	trimSpaces(token);
	// Store it in shop
	shop.stock[id].product.price = atof(token);

	// Token 3 - Product quantity
	token = strtok(NULL, ",");
	if (!token) {
		printf("\nERROR: Invalid product line [%s] at shop file (%s). Missing product quantity\n", line, SHOP_CSV_FILE_NAME);
		return 1;
	}
	trimSpaces(token);
	// Store it in shop
	shop.stock[id].quantity = atoi(token);

	// Product info successfully parsed, update stock products number
	shop.productsNum++;

	return 0;
}

/* Parse info present in a product line (inside customer orders file)  =>   Product, quantity   i.e.    "Coke Can,10" . Return 0 when success, 1 otherwise */

int parseCustomerProductLine(char* line, struct Customer* customer)
{
	char* token;

	// shortcut for next product to store
	int id = customer->productsNum;

	// we expect 2 tokens:  Product name,quantity
	// Token 1 - Product name
	token = strtok(line, ",");
	if (!token) {
		printf("\nERROR: Invalid product line [%s] at customer file. Missing product name\n", line);
		return 1;
	}
	trimSpaces(token);
	// Store it in shop
	snprintf(customer->shoppingList[id].product.name, MAX_PRODUCT_NAME_LENGTH, "%s", token);

	// Token 2 - Product quantity
	token = strtok(NULL, ",");
	if (!token) {
		printf("\nERROR: Invalid product line [%s] at customer file. Missing product quantity\n", line);
		return 1;
	}
	trimSpaces(token);
	// Store it in shop
	customer->shoppingList[id].quantity = atoi(token);

	// More tokens -> error
	token = strtok(NULL, ",");
	if (token) {
		printf("\nERROR: Invalid product line [%s] at customer file. Unexpected fields number\n", line);
		return 1;
	}

	// Product info successfully parsed, update customer products number
	customer->productsNum++;

	return 0;
}

/* Initialize shop stock from input csv file, we expect a file next to the program with name SHOP_CSV_FILE_NAME */

void initializeShop()
{
	// reset shop data
	memset(&shop, 0, sizeof(struct Shop));

	// default working mode is life mode (no customer file supplied)
	shop.lifeMode = 1;

	// read initial shop stock from file
	FILE* fp;

	fp = fopen(SHOP_CSV_FILE_NAME, "r");
	if (fp == NULL) {
		printf("\nERROR: Could not read input shop file (%s)\n", SHOP_CSV_FILE_NAME);
		exit(EXIT_FAILURE);
	}

	// First line contains the cash value
	char cashLine[MAX_CSV_FILE_LINE_LENGTH];
	memset(cashLine, 0, MAX_CSV_FILE_LINE_LENGTH);
	char* result = fgets(cashLine, MAX_CSV_FILE_LINE_LENGTH, fp);

	if (!result) {
		printf("\nERROR: Could not read cash info (first line) from shop file (%s)\n", SHOP_CSV_FILE_NAME);
		fclose(fp);
		exit(EXIT_FAILURE);
	}
	// trim \r or \n chars if present
	trimNewLineChars(cashLine);

	// Parse cash value
	if (sscanf(cashLine, "%lf", &shop.cash) != 1) {
		printf("\nERROR: Invalid cash info (first line) at shop file (%s)\n", SHOP_CSV_FILE_NAME);
		fclose(fp);
		exit(EXIT_FAILURE);
	}

	// Read product lines
	char productLine[MAX_CSV_FILE_LINE_LENGTH];
	memset(productLine, 0, MAX_CSV_FILE_LINE_LENGTH);

	while (fgets(productLine, MAX_CSV_FILE_LINE_LENGTH, fp)) {
		// shorcut for current stock index
		int id = shop.productsNum;
		trimNewLineChars(productLine);

		if (parseShopProductLine(productLine) != 0) {
			fclose(fp);
			exit(EXIT_FAILURE);
		}

		// clean line buffer to read next one
		memset(productLine, 0, MAX_CSV_FILE_LINE_LENGTH);
	}


	// close file once we are done
	fclose(fp);
}

/* Show program usage */

void showUsage(char* arg0)
{
	printf("\nUSAGE: %s [-f customerFile.csv] | [-h]\n", arg0);
	printf("\nWhere:\n");
	printf("[-f customerFile.csv] Place an order using an input CSV file supplied by the user (life mode otherwise)\n");
	printf("[-h]                  Displays this help\n");
}

/* Validate products names and quantities against stock available */

int validateOrderProducts(struct Customer* customer)
{
	int found = 0;

	for (int i = 0; i < customer->productsNum; i++) {
		for (int j = 0; j < shop.productsNum; j++) {
			if (!strcmp(customer->shoppingList[i].product.name, shop.stock[j].product.name)) {
				// Product name found -> verify quantity
				if (customer->shoppingList[i].quantity <= shop.stock[j].quantity)
				{
					found = 1;
					break; // found and valid - check next product requested by customer
				}
				// else { stop - product name with invalid quantity }
				printf("\nERROR: Product [%s] present in stock but wrong quantity requested, available [%d], ordered [%d]!!!\n",
					customer->shoppingList[i].product.name,
					shop.stock[j].quantity,
					customer->shoppingList[i].quantity);

				return 1;
			}
		}

		if (!found)
		{
			printf("\nERROR: Product [%s] is out of stock!!!\n", customer->shoppingList[i].product.name);
			return 1;
		}
	}

	return 0;
}

/* Validate products names and quantities against stock available */

int validateBudget(struct Customer* customer, double* totalCost)
{
	double total = 0.0;

	// names and quantities previously validated - just do the maths and compare

	for (int i = 0; i < customer->productsNum; i++) {
		for (int j = 0; j < shop.productsNum; j++) {
			if (!strcmp(customer->shoppingList[i].product.name, shop.stock[j].product.name)) {
				// add cost
				total += (customer->shoppingList[i].quantity * shop.stock[j].product.price);
				break;
			}
		}
	}

	if (total > customer->budget)
	{
		printf("\nERROR: Budget is too low, total costs [%.2f], current budget [%.2f]!!!\n",
			total,
			customer->budget);

		return 1;
	}

	*totalCost = total;

	return 0;
}

/* Clean customer shopping list */

void cleanCustomerShoppingList(struct Customer* customer)
{
	for (int i = 0; i < customer->productsNum; i++) {
		memset(&customer->shoppingList[i], 0, sizeof(struct ProductStock));
	}
	customer->productsNum = 0;
}

/* Update shop stock once the order has been verified and can be completed */

void updateShopStock(struct Customer* customer, double totalCost)
{
	for (int i = 0; i < customer->productsNum; i++) {
		for (int j = 0; j < shop.productsNum; j++) {
			if (!strcmp(customer->shoppingList[i].product.name, shop.stock[j].product.name)) {
				// Reduce number of products in stock
				shop.stock[j].quantity -= customer->shoppingList[i].quantity;
			}
		}
	}

	// Increase current shop cash
	shop.cash += totalCost;

	// decrease customer budget
	customer->budget -= totalCost;

	// Clean shopping list
	cleanCustomerShoppingList(customer);

	// show new shop stock
	printShopStock(&shop);
}

/* Validate a given customer order and accept the purchase */

void validateOrderAndDoPurchase(struct Customer* customer)
{
	if (customer->name[0] == 0)
	{
		printf("\nERROR: Empty customer name, could not accept the order!!!\n");
	}
	else if(customer->budget <= 0.0)
	{
		printf("\nERROR: Invalid customer budget [%.2f], could not accept the order!!!\n", customer->budget);
	}
	else if (customer->productsNum <= 0)
	{
		printf("\nERROR: Invalid customer products number [%d], could not accept the order!!!\n", customer->productsNum);
	}
	else
	{
		// Validate products and quantities requested
		if (validateOrderProducts(customer) != 0) return;

		// Validate budget
		double totalCost = 0.0;
		if (validateBudget(customer, &totalCost) != 0) return;

		printf("\n\nCustomer order IS VALID  (SHOP STOCK UPDATED)!!!\n\n");

		// Purchase can be completed -> Update shop stock after doing the purchase
		updateShopStock(customer, totalCost);
	}
}

/* Open customer's order file and process it */

void processCustomerOrderFile(char* fileName)
{
	struct Customer customer;
	char* token = NULL;

	memset(&customer, 0, sizeof(struct Customer));

	// We assume that file is next to the program - no paths involved
	FILE* fp;

	fp = fopen(fileName, "r");
	if (fp == NULL) {
		printf("\nERROR: Could not read input customer file (%s)\n", SHOP_CSV_FILE_NAME);
		exit(EXIT_FAILURE);
	}

	// First line contains, customer name and budget:  NAME,XXXX
	char firstLine[MAX_CSV_FILE_LINE_LENGTH];
	memset(firstLine, 0, MAX_CSV_FILE_LINE_LENGTH);
	char* result = fgets(firstLine, MAX_CSV_FILE_LINE_LENGTH, fp);

	if (!result) {
		printf("\nERROR: Could not read customer info (first line) from customer file (%s)\n", fileName);
		fclose(fp);
		exit(EXIT_FAILURE);
	}
	// trim \r or \n chars if present
	trimNewLineChars(firstLine);

	// Token 1 - Customer name
	token = strtok(firstLine, ",");
	if (!token) {
		printf("\nERROR: Invalid customer line [%s] at customer file (%s). Missing customer name\n", firstLine, fileName);
		fclose(fp);
		exit(EXIT_FAILURE);
	}
	trimSpaces(token);
	// Store it in shop
	snprintf(customer.name, MAX_CUSTOMER_NAME_LENGTH, "%s", token);

	// Token 2 - Customer budget
	token = strtok(NULL, ",");
	if (!token) {
		printf("\nERROR: Invalid customer line [%s] at customer file (%s). Missing customer budget\n", firstLine, fileName);
		fclose(fp);
		exit(EXIT_FAILURE);
	}
	trimSpaces(token);
	// Store it in shop
	customer.budget = atof(token);

	// more tokens -> error
	token = strtok(NULL, ",");
	if (token) {
		printf("\nERROR: Invalid customer line [%s] at customer file (%s). Unexpected fields number\n", firstLine, fileName);
		fclose(fp);
		exit(EXIT_FAILURE);
	}

	// Read product lines
	char productLine[MAX_CSV_FILE_LINE_LENGTH];
	memset(productLine, 0, MAX_CSV_FILE_LINE_LENGTH);

	while (fgets(productLine, MAX_CSV_FILE_LINE_LENGTH, fp)) {
		// shorcut for current stock index
		int id = customer.productsNum;
		trimNewLineChars(productLine);

		if (parseCustomerProductLine(productLine, &customer) != 0) {
			fclose(fp);
			exit(EXIT_FAILURE);
		}

		// clean line buffer to read next one
		memset(productLine, 0, MAX_CSV_FILE_LINE_LENGTH);
	}

	// Show order
	printCustomerOrder(&customer);

	// Validate it
	validateOrderAndDoPurchase(&customer);

	// close file once we are done
	fclose(fp);
}

/* Run order menu in life mode, user selects name, budget and products/quantities to buy */

void runOrderMenu()
{
	int lastProductAddedIndex = 0;
	struct Customer customer;
	char newline = 0;
	char option = 0;

	memset(&customer, 0, sizeof(struct Customer));

	do {
		printf("\n\nSelect your choice:\n\n");
		printf("[1]-Enter customer name [current: %s]\n", customer.name);
		printf("[2]-Enter budget [current: %.2f]\n", customer.budget);
		printf("[3]-Enter product name [current: %s]\n", customer.shoppingList[lastProductAddedIndex].product.name);
		printf("[4]-Enter quantity [current: %d]\n", customer.shoppingList[lastProductAddedIndex].quantity);
		printf("[5]-Show current order info\n");
		printf("[6]-Order products\n");
		printf("[7]-Clean shopping list\n");
		printf("[p]-Previous menu\n");
		printf("\nOption selected: ");

		// get single char and ignore the rest
		scanf("%c%*c", &option);

		switch (option)
		{
			case '1':
			{
				//  Enter customer name
				printf("\nCustomer name: ");
				fgets(customer.name, MAX_CUSTOMER_NAME_LENGTH, stdin);
				trimNewLineChars(customer.name);
				break;
			}

			case '2':
			{
				if (customer.name[0] == 0)
				{
					printf("\nERROR: Enter a customer name first!!!");
					continue;
				}
				//  Enter customer budget
				printf("\nCurrent budget: ");
				scanf("%lf%*c", &customer.budget);
				printf("\nCustomer budget read: [%.2f]", customer.budget);
				break;
			}

			case '3':
			{
				if (customer.name[0] == 0)
				{
					printf("\nERROR: Enter a customer name first!!!");
					continue;
				}
				if (customer.budget == 0.0)
				{
					printf("\nERROR: Enter a customer budget first!!!");
					continue;
				}
				//  Enter product name
				printf("\nProduct name: ");
				fgets(customer.shoppingList[customer.productsNum].product.name, MAX_PRODUCT_NAME_LENGTH, stdin);
				trimNewLineChars(customer.shoppingList[customer.productsNum].product.name);
				//scanf("%s%c", customer.shoppingList[customer.productsNum].product.name, &newline);
				break;
			}

			case '4':
			{
				if (customer.name[0] == 0)
				{
					printf("\nERROR: Enter a customer name first!!!");
					continue;
				}
				if (customer.budget == 0.0)
				{
					printf("\nERROR: Enter a customer budget first!!!");
					continue;
				}
				if (customer.shoppingList[customer.productsNum].product.name[0] == 0)
				{
					printf("\nERROR: Enter a product name first!!!");
					continue;
				}
				//  Enter product quantity
				printf("\nProduct quantity: ");
				scanf("%d%*c", &customer.shoppingList[customer.productsNum].quantity);
				// once we have the quantity we can increase number . the rest of info has been previously filled
				lastProductAddedIndex = customer.productsNum;
				customer.productsNum++;
				break;
			}

			case '5':
			{
				// Print on screen current order content
				printCustomerOrder(&customer);
				break;
			}

			case '6':
			{
				//  Validate the order and purchase the products if feasible
				validateOrderAndDoPurchase(&customer);
				break;
			}


			case '7':
			{
				// Remove any product from order
				cleanCustomerShoppingList(&customer);
				break;
			}

			case 'p':
			{
				// Let the menu return to previous
				option = 0;
				break;
			}

			default:
			{
				// error
				printf("\n\nERROR: Invalid option (try again)\n\n");
				scanf("%*s%c", &newline);
				break;
			}
		}

	} while (option != 0);
}

/* Run life mode, user selects his order using menus */

void runLifeModeMenu()
{
	char option = 0;

	do {
		printf("\n\nSelect your choice:\n\n");
		printf("[1]-Enter new order\n");
		printf("[2]-Show shop stock\n");
		printf("[3]-Exit application\n\n");
		printf("\nOption selected: ");

		// get single char and ignore the rest
		scanf("%c%*c", &option);

		switch (option)
		{
			case '1':
			{
				//  Enter new order
				runOrderMenu();
				break;
			}

			case '2':
			{
				// Show shop stock
				printShopStock(&shop);
				break;
			}

			case '3':
			{
				// Show shop stock
				printf("Application finished!");
				exit(EXIT_SUCCESS);
				break;
			}

			default:
			{
				// error
				printf("\nERROR: Invalid option\n\n");
				break;
			}
		}

	} while (option != 0);
}

/****************************************** MAIN PROCEDURE  *******************************************************/

int main(int argc, char** argv)
{
    // Initialize shop info from file
	initializeShop();

	// show current shop content
	printShopStock(&shop);

	// Get selected mode: file mode (-f xxxx) or life mode (no parameters)
	if (argc >= 2) {
		if (!strcmp(argv[1], "-h")) {
			showUsage(argv[0]);
			exit(EXIT_SUCCESS);
		}
		else if (!strcmp(argv[1], "-f")) {
			// Get file info - we need a second parameter = file name
			if (argc == 3) {
				shop.lifeMode = 0;
				processCustomerOrderFile(argv[2]);
			}
			else {
				printf("\nERROR: Missing order file name\n");
				showUsage(argv[0]);
				exit(EXIT_FAILURE);
			}
		}
		else {
			printf("\nERROR: Wrong options\n");
			showUsage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	// When no customer file supplied - assume life mode and display menu
	if (shop.lifeMode) {
		runLifeModeMenu();
	}
}
