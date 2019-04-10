/*************************************************
*Name: Alexander Dumouchel
*ID:157607177
*adumouchel@myseneca.ca
*milestone 5
**************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "AmaApp.h"
#include "Utilities.h"

static char filename[max_file_size];
static Product *products[max_product_size];
static int num_products;

static void pause(void);
static int menu(void);
static void loadProductRecords(void);
static void saveProductRecords(void);
static void listProducts(void);
static Product *find(const char *sku);
static void addQty(Product *product);
static void addProduct(char tag);

void initialize(const char *c)
{
    if (c == NULL || c[0] == '\0') {
        filename[0] = '\0';
    } else {
        strncpy(filename, c, max_file_size);
    }

    for (int i = 0; i < max_product_size; i++) {
        products[i] = NULL;
    }
    num_products = 0;
}

void cleanup(void)
{
    for (int i = 0; i < max_product_size; i++) {
        if (products[i] != NULL) {
            Product_destroy(products[i]);
            free(products[i]);
            products[i] = NULL;
        }
    }
}

static void ignore(FILE *f)
{
    int c;
    while ((c = getc(f)) != EOF && c != '\n')
        ;
}

static void pause(void)
{
    puts("Press Enter to continue...");
    int c;
    while ((c = getchar()) != EOF && c != '\n')
        ;
}

static int menu(void)
{
    int menuselect = 0;
    while (!ferror(stdin) && !feof(stdin)) {
        puts("Disaster Aid Supply Management Program");
        puts("--------------------------------------");
        puts("1- List products");
        puts("2- Search product");
        puts("3- Add non-perishable product");
        puts("4- Add perishable product");
        puts("5- Add to product quantity");
        puts("6- Delete product");
        puts("7- Sort products");
        puts("0- Exit program");
        fputs("> ", stdout);
        int status = scanf("%i", &menuselect);
        ignore(stdin);
        if (status == 1) {
	    return (menuselect >= 0 && menuselect <= 7) ? menuselect : -1;
	} else if (status < 0) {
            // ^D most likely, assume an exit
            return 0;
        } else {
            puts("invalid input");
        }
    }
    return -1;
}

static void loadProductRecords(void)
{
    int i = 0;
    for (i = 0; i < num_products; i++) {
        free(products[i]);
        products[i] = NULL;
    }
    i = 0;

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        num_products = i;
        return;
    }
    while (!ferror(f) && !feof(f)) {
        int temp;
        if ((temp = getc(f)) != EOF) {
            if (temp == 'N' || temp == 'n') {
                getc(f);
                products[i] = createInstance('N'); //this creates a non perishable
                if (Product_read(products[i], f, false)) {
                    i++;
                } else {
                    puts("Cancelled");
                }
            }

            else if (temp == 'P' || temp == 'p') {
                getc(f);
                products[i] = createInstance('P'); //this creates a perishable
                if (Product_read(products[i], f, false)) {
                    i++;
                } else {
                    puts("Cancelled");
                }
            }
        }
    }
    num_products = i;
    fclose(f);
}

static void saveProductRecords(void)
{
    FILE *f = fopen(filename, "w");
    if (f == NULL) return;

    for (int i = 0; i < num_products; i++) {
        Product_write(products[i], f, write_condensed);
        putc('\n', f);
    }
    fclose(f);
}

static void listProducts(void)
{
    puts("------------------------------------------------------------------------------------------------");
    puts("| Row |     SKU | Product Name     | Unit       |   Price | Tax |   QtyA |   QtyN | Expiry     |");
    puts("|-----|---------|------------------|------------|---------|-----|--------|--------|------------|");
    double totalcost = 0.0;

    for (int i = 0; i < num_products; i++) {
        printf("|%4d |", i + 1);
        Product_write(products[i], stdout, write_table);
        putchar('\n');
        totalcost += products[i]->before_tax;
    }

    puts("------------------------------------------------------------------------------------------------");
    printf("|                                                      Total cost of support ($): | %10.2lf |\n", totalcost);
    puts("------------------------------------------------------------------------------------------------\n");
    pause();
}

static int find_idx(const char *arr)
{
    int i;
    for (i = 0; i < num_products; i++) {
        if (Product_equal(products[i], arr)) {
            break;
        }
    }
    return i;
}

static Product *find(const char *arr)
{
    int idx = find_idx(arr);
    if (idx >= num_products)
        return NULL;
    return products[idx];
}

static void addQty(Product *product)
{
    int quantity = 0;
    Product_write(product, stdout, write_human);
    fputs("\n\nPlease enter the number of purchased items: ", stdout);

    if (scanf("%d", &quantity) == 1) {
        if (product->needed - product->quantity >= quantity) {
            product->before_tax += quantity;
            saveProductRecords();
        } else {
            int quantityneeded = product->needed;
            int quantityavailable = product->quantity;
            product->before_tax += quantityneeded - quantityavailable;

            printf("Too many items; only %d is needed. Please return the extra %d items.\n", quantityneeded - quantityavailable, (quantity - (quantityneeded - quantityavailable)));
        }
        fputs("\nUpdated!\n", stdout);
    } else {
        ignore(stdin);
        puts("Invalid quantity value!");
    }
}

static void addProduct(char tag)
{
    if (num_products == max_product_size) {
        puts("Cannot add more products!");
        return;
    }
    Product *product = createInstance(tag);
    if (Product_read(product, stdin, true)) {
        products[num_products] = product;
        num_products++;
        saveProductRecords();
        puts("\nSuccess!\n");
    } else {
        puts("\nCancelled.");
        // thonk...
        // Product_write(product, stdout, write_human);
        ignore(stdin);
        Product_destroy(product);
        free(product);
    }
}

static void removeProduct(const char *query)
{
    int toRemove;
    if ((toRemove = find_idx(query)) < num_products) //this traverses the array of find to see if temp exsists as a product
    {
         Product_write(products[toRemove], stdout, write_human);
         puts("\nRemove this product? (Y)es/(N)o");
         int c = getchar();
         if (c == EOF) {
             puts("IO error");
             return;
         } else if (tolower(c) == 'y') {
            Product_destroy(products[toRemove]);
            free(products[toRemove]);

            // TODO: use memmove
            // if (toRemove + 1 < num_products) {
            //     memmove(&products[toRemove], &products[toRemove + 1], (char *)&products[num_products-1] - (char *)&products[toRemove + 1]);
            // }
            // products[num_products--] = NULL;

            while (toRemove + 1 < num_products) {
                products[toRemove] = products[toRemove + 1];
                ++toRemove;
            }
            products[toRemove] = NULL;
            --num_products;
            saveProductRecords();
        } else {
            puts("Cancelled");
        }
    } else {
        fputs("No such product!", stdout);
    }
    putchar('\n');
    pause();
}
static int sortMenu(void)
{
    int menuselect = 0;
    while (!ferror(stdin) && !feof(stdin)) {
        puts("Which field would you like to sort by?");
        puts("--------------------------------------");
        puts("1- SKU");
        puts("2- Product Name");
        puts("3- Unit");
        puts("4- Price");
        puts("5- Taxable");
        puts("6- Quantity Available");
        puts("7- Quantity Needed");
        puts("8- Expiry Date");
        puts("0- Cancel");
        fputs("> ", stdout);

        int status = scanf("%i", &menuselect);
        ignore(stdin);
        if (status == 1) {
	    return (menuselect >= 0 && menuselect <= 8) ? menuselect : -1;
	} else if (status < 0) {
            return 0;
        } else {
            puts("invalid input");
        }
    }
    return -1;
}

static void sortProducts(void)
{
    int idx = sortMenu();
    if (idx > 0) {
        // We have an array of function pointers in Product.c
        // which contains the comparators in the menu order.
        qsort(products, num_products, sizeof(Product *), Product_comparators[idx - 1]);
    } else {
        puts("Cancelled");
    }
}
int run(void)
{
    bool running = true;
    char *temps = NULL;
    size_t len = 0;
    ssize_t count;
    Product *temp = NULL;
    loadProductRecords();

    while (running) {
        switch (menu()) {
        case 1:
            listProducts();
            break;

        case 2:
            fputs("Please enter the product SKU: ", stdout);
            if ((count = getline(&temps, &len, stdin)) < 0 || count > max_length_sku) {
                puts("Error");
                free(temps);
                return 1;
            }
            temps[count - 1] = '\0';

            if ((temp = find(temps)) != NULL) //this traverses the array of find to see if temp exsists as a product
            {
                Product_write(temp, stdout, write_human);
            } else {
                fputs("No such product!", stdout);
            }
            putchar('\n');
            pause();
            break;

        case 3:
            addProduct('N');
            ignore(stdin);
            break;

        case 4:
            addProduct('P');
            ignore(stdin);
            break;

        case 5:
            fputs("Please enter the product SKU: ", stdout);
            if ((count = getline(&temps, &len, stdin)) < 0 || count > max_length_sku) {
                puts("Error");
                free(temps);
                return 1;
            }
            temps[count - 1] = '\0';

            if ((temp = find(temps)) != NULL) {
                addQty(temp);
            } else {
                puts("No such product!");
            }
            break;

        case 6:
            fputs("Please enter the product SKU to delete: ", stdout);
            if ((count = getline(&temps, &len, stdin)) < 0 || count > max_length_sku) {
                puts("Error");
                free(temps);
                return 1;
            }
            temps[count - 1] = '\0';
            removeProduct(temps);
            break;

        case 7:
            sortProducts();
            break;

        case 0:
            puts("Goodbye!");
            running = false;
            break;

        default:
            puts("~~~Invalid selection, try again!~~~");
            pause();
            break;
        }
    }
    free(temps);
    return 0;
}
