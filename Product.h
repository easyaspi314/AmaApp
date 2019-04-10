#ifndef AMA_PRODUCT_H
#define AMA_PRODUCT_H
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "Date.h"

enum {
    max_length_label = 30,
    max_length_sku = 7,
    max_length_name = 75,
    max_length_unit = 10,
    write_condensed = 0,
    write_table = 1,
    write_human = 2,
#define tax_rate 0.13
};

typedef struct product Product;

struct product {
    char type;
    char sku[max_length_sku + 1];
    char unit[max_length_unit + 1];
    char *name;
    int quantity;
    int needed;
    double before_tax;
    bool taxable;
    char *err;
    char *buffer;
    size_t buffer_size;
    Date expiry;
};

void Product_message(Product *pro, const char *pText); //done
bool Product_isClear(const Product *pro); //done

void Product_copy(Product *pro, const Product *other); //done
Product *Product_create(char type);
Product *Product_createWithValues(char type, const char *sku, const char *name, const char *unit, double price_bt, int needed, int quantity, bool taxables);
void Product_destroy(Product *prod);
int Product_add(Product *pro, int cnt);
const char *Product_name(const Product *pro_ptr);
bool Product_equal(const Product *pro, const char *sku);
bool Product_greater(const Product *pro, const char *sku);
int Product_qtyAvailable(const Product *pro);
int Product_qtyNeeded(const Product *pro);
double Product_total_cost(const Product *pro);
bool Product_isEmpty(const Product *pro);
bool Product_read(Product *pro, FILE *in, bool interractive);
bool Product_write(const Product *pro, FILE *out, int writeMode);
extern int (*Product_comparators[8])(const void *, const void *);

#endif
