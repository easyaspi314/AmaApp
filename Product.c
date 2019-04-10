#include "Product.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void setProductName(Product *pro, const char *product_name);

static void condensed(const Product *pro, char *);
static double priceAfterTax(const Product *pro);

void Product_message(Product *pro, const char *pText)
{
    free(pro->err);
    size_t len = strlen(pText) + 1;
    pro->err = (char *)malloc(len);
    memcpy(pro->err, pText, len);
}
bool Product_isClear(const Product *pro)
{
    return !pro->err;
}

Product *Product_createWithValues(char type, const char *sku, const char *name, const char *unit, double price_bt, int needed, int quantity, bool taxables)
{
    Product *pro = (Product *)calloc(1, sizeof(Product));
    pro->type = type;
    if (name != NULL && name[0] != '\0') {
        pro->name = NULL;
        strncpy(pro->sku, sku, max_length_sku);
        pro->sku[max_length_sku] = '\0';

        setProductName(pro, name);
        strncpy(pro->unit, unit, max_length_unit);
        pro->unit[max_length_unit] = '\0';
        pro->before_tax = price_bt;
        pro->quantity = quantity;
        pro->needed = needed;
        pro->taxable = taxables;
    }
    return pro;
}
Product *Product_create(char type)
{
    Product *pro = (Product *)calloc(1, sizeof(Product));
    pro->type = type;
    return pro;
}

void Product_destroy(Product *pro)
{
    free(pro->name);
    free(pro->err);
    free(pro->buffer);
}

void Product_copy(Product *pro, const Product *other)
{
    if (pro != other) {
        strncpy(pro->sku, other->sku, max_length_sku);

        if (other->name != NULL) {
            if (strlen(other->name) > max_length_name) {
                pro->name = (char *)malloc(max_length_name + 1);
                strncpy(pro->name, other->name, max_length_name);
            } else {
                pro->name = (char *)malloc(max_length_name + 1);
                strncpy(pro->name, other->name, max_length_name);
            }
        } else {
            pro->name = NULL;
        }
        strncpy(pro->unit, other->unit, max_length_unit);
        pro->type = other->type;
        pro->quantity = other->quantity;
        pro->needed = other->needed;
        pro->before_tax = other->before_tax;
        pro->taxable = other->taxable;
        pro->expiry = other->expiry;
    }
}

int Product_add(Product *pro, int cnt)
{
    if (cnt > 0) {
        pro->quantity += cnt;
    }
    return pro->quantity;
}

const char *Product_name(const Product *pro)
{
    return pro->name;
}

bool Product_equal(const Product *pro, const char *sku)
{
    return strcmp(pro->sku, sku) == 0;
}
bool Product_greater(const Product *pro, const char *sku)
{
    return strcmp(pro->sku, sku) > 0;
}
static int compareSku(const void *lhs_ptr, const void *rhs_ptr)
{
    const Product *lhs = *(const Product *const *)lhs_ptr;
    const Product *rhs = *(const Product *const *)rhs_ptr;
    long lhs_sku = strtol(lhs->sku, NULL, 10);
    long rhs_sku = strtol(rhs->sku, NULL, 10);
    return (lhs_sku > rhs_sku) - (lhs_sku < rhs_sku);
}

static int compareName(const void *lhs_ptr, const void *rhs_ptr)
{
    const Product *lhs = *(const Product *const *)lhs_ptr;
    const Product *rhs = *(const Product *const *)rhs_ptr;
    return strcmp(lhs->name, rhs->name);
}

static int compareUnit(const void *lhs_ptr, const void *rhs_ptr)
{
    const Product *lhs = *(const Product *const *)lhs_ptr;
    const Product *rhs = *(const Product *const *)rhs_ptr;
    return strcmp(lhs->unit, rhs->unit);
}

static int comparePrice(const void *lhs_ptr, const void *rhs_ptr)
{
    const Product *lhs = *(const Product *const *)lhs_ptr;
    const Product *rhs = *(const Product *const *)rhs_ptr;
    return (lhs->before_tax > rhs->before_tax) - (lhs->before_tax < rhs->before_tax);
}

static int compareQuantity(const void *lhs_ptr, const void *rhs_ptr)
{
    const Product *lhs = *(const Product *const *)lhs_ptr;
    const Product *rhs = *(const Product *const *)rhs_ptr;
    return (lhs->quantity > rhs->quantity) - (lhs->quantity < rhs->quantity);
}

static int compareNeeded(const void *lhs_ptr, const void *rhs_ptr)
{
    const Product *lhs = *(const Product *const *)lhs_ptr;
    const Product *rhs = *(const Product *const *)rhs_ptr;
    return (lhs->needed > rhs->needed) - (lhs->needed < rhs->needed);
}


static int compareTaxable(const void *lhs_ptr, const void *rhs_ptr)
{
    const Product *lhs = *(const Product *const *)lhs_ptr;
    const Product *rhs = *(const Product *const *)rhs_ptr;
    if (lhs->taxable != rhs->taxable) {
        return lhs->taxable ? -1 : 1;
    }
    return compareSku(lhs_ptr, rhs_ptr);
}


static int compareExpiry(const void *lhs_ptr, const void *rhs_ptr)
{
    const Product *lhs = *(const Product *const *)lhs_ptr;
    const Product *rhs = *(const Product *const *)rhs_ptr;
    if (lhs->type != rhs->type) {
        return lhs->type == 'P' ? 1 : -1;
    } else if (lhs->type != 'P') {
        return compareSku(lhs_ptr, rhs_ptr);
    }
    return Date_compare(&lhs->expiry, &rhs->expiry);
}

int (*Product_comparators[])(const void *, const void *) = {
    &compareSku,
    &compareName,
    &compareUnit,
    &comparePrice,
    &compareTaxable,
    &compareQuantity,
    &compareNeeded,
    &compareExpiry
};

// todo: figure out the right name
//	bool Product::operator> (const iProduct& other) const
//	{
//		return strcmp(pro->name, other.name()) > 0;
//	}

int Product_qtyAvailable(const Product *pro)
{
    return pro->quantity;
}

int Product_qtyNeeded(const Product *pro)
{
    return pro->needed;
}

double Product_total_cost(const Product *pro)
{
    double temp = 0.0;
    if (pro->taxable == true) {
        temp = (pro->before_tax * pro->quantity) * (1 + tax_rate);
    } else {
        temp = pro->before_tax * pro->quantity;
    }

    return temp;
}

bool Product_isEmpty(const Product *pro)
{
    if (pro->name == NULL || pro->name[0] == '\0')
        return true;
    else
        return false;
}

static void setProductName(Product *pro, const char *product_name)
{
    free(pro->name);
    pro->name = (char *)malloc(max_length_name + 1);
    strncpy(pro->name, product_name, max_length_name);
    pro->name[max_length_name] = '\0';
}

static double priceAfterTax(const Product *pro)
{
    if (pro->taxable) {
        return pro->before_tax * (1 + tax_rate);
    } else {
        return pro->before_tax;
    }
}
static void condensed(const Product *pro, char *cname)
{
    int length;
    const int maxlength = 17;
    for (length = 0; pro->name[length] != '\0'; length++) {
        if (length < maxlength) {
            strncpy(cname, pro->name, maxlength);
            cname[maxlength - 1] = '\0';
        } else {
            strncpy(cname, pro->name, 13);
            cname[13] = '.';
            cname[14] = '.';
            cname[15] = '.';
            cname[16] = '\0';
        }
    }
}

static void prompter_nop(const char *msg)
{
    (void)msg;
}

static void prompter(const char *msg)
{
    printf("%*s", max_length_label, msg);
}

bool Product_read(Product *pro, FILE *in, bool interractive)
{
    ssize_t count;
    void (*prompt_func)(const char *) = interractive ? prompter : prompter_nop;
    int delim = interractive ? '\n' : ',';

#define prompt(message)                                                            \
    do {                                                                           \
        prompt_func(message);                                                      \
        if ((count = getdelim(&pro->buffer, &pro->buffer_size, delim, in)) <= 0) { \
            Product_message(pro, "IO error");                                      \
            return false;                                                          \
        }                                                                          \
        pro->buffer[count - 1] = '\0';                                             \
    } while (0)

    prompt("Sku: ");
    strncpy(pro->sku, pro->buffer, max_length_sku + 1);
    pro->sku[max_length_sku] = '\0';
    prompt("Name: ");
    setProductName(pro, pro->buffer);
    prompt("Unit: ");
    strncpy(pro->unit, pro->buffer, max_length_unit + 1);
    pro->unit[max_length_unit] = '\0';
    int c, status;

    if (interractive) {
        prompt_func("Taxed? (y/n): ");
        c = getc(in);
        if (c == EOF) {
            Product_message(pro, "IO error");
            return false;
        } else if (tolower(c) == 'y') {
            pro->taxable = true;
        } else if (tolower(c) == 'n') {
            pro->taxable = false;
        } else {
            Product_message(pro, "Only (Y)es or (N)o are acceptable!");
            return false;
        }
        while ((c = getc(in)) != EOF && c != '\n')
            ;
        prompt("Price: ");
        if ((status = sscanf(pro->buffer, "%lf", &pro->before_tax)) != 1) {
            Product_message(pro, "Invalid price entry");
            return false;
        }
    } else {
        prompt("");
        if ((status = sscanf(pro->buffer, "%lf", &pro->before_tax)) != 1) {
            Product_message(pro, "Invalid price entry");
            return false;
        }
        prompt("");
        pro->taxable = (tolower(pro->buffer[0]) == 'y' || pro->buffer[0] == '1');
    }
    prompt_func("Quantity on hand: ");
    if ((status = sscanf(pro->buffer, "%d", &pro->quantity)) != 1) {
        Product_message(pro, "Invalid quantity");
        return false;
    }

    while ((c = getc(in)) != EOF && c != delim)
        ;

    prompt_func("Quantity needed: ");
    if ((status = fscanf(in, "%d", &pro->needed)) != 1) {
        Product_message(pro, "IO error");
        return false;
    }
    if (pro->type == 'P' && getc(in) != EOF) {
        prompt_func("Expiry date (YYYY/MM/DD): ");
        Date_read(&pro->expiry, in);
        if (Date_status(&pro->expiry) != no_error) {
            if (Date_status(&pro->expiry) == error_year) {
                Product_message(pro, "Invalid Year in Date Entry");
                return false;
            } else if (Date_status(&pro->expiry) == error_mon) {
                Product_message(pro, "Invalid Month in Date Entry");
                return false;
            } else if (Date_status(&pro->expiry) == error_day) {
                Product_message(pro, "Invalid Day in Date Entry");
                return false;
            } else {
                Product_message(pro, "Invalid Date Entry");
                return false;
            }
        }
        getc(in);
    }
#undef prompt
    return true;
}

bool Product_write(const Product *pro, FILE *out, int writeMode)
{
    if (!Product_isClear(pro)) {
        fputs(pro->err, out);
    } else if (!Product_isEmpty(pro)) {
        const int print_length_sku = max_length_sku;
        const int print_length_name = 16;
        const int print_length_unit = 10;
        const int print_length_price = 7;
        const int print_length_tax = 3;
        const int print_length_Quantityonhand = 6;
        const int print_length_QNeeded = 6;
        char temp_taxable[4] = { '\0' };
        if (pro->taxable) {
            strncpy(temp_taxable, "yes", 3);
            temp_taxable[3] = '\0';
        } else {
            strncpy(temp_taxable, "no", 3);
            temp_taxable[2] = '\0';
        }

        switch (writeMode) {
        case write_condensed:
            fprintf(out, "%c,%s,%s,%s,%.2lf,%d,%d,%d",
                pro->type,
                pro->sku,
                pro->name,
                pro->unit,
                pro->before_tax,
                pro->taxable,
                pro->quantity,
                pro->needed);
            if (pro->type == 'P') {
                putc(',', out);
                Date_write(&pro->expiry, out);
            }
            break;

        case write_table: {
            char sName[17];
            condensed(pro, sName);
            fprintf(out, " %*s | %-*s | %-*s | %*.2lf | %*s | %*d | %*d |",
                print_length_sku, pro->sku,
                print_length_name, sName,
                print_length_unit, pro->unit,
                print_length_price, pro->before_tax,
                print_length_tax, temp_taxable,
                print_length_Quantityonhand, pro->quantity,
                print_length_QNeeded, pro->needed);
            if (pro->type == 'P') {
                putc(' ', out);
                Date_write(&pro->expiry, out);
                fputs(" |", out);
            } else {
                fputs("            |", out);
            }
            break;
        }
        case write_human:
            fprintf(out, "%*s%s\n", max_length_label, "Sku: ", pro->sku);
            fprintf(out, "%*s%s\n", max_length_label, "Name: ", pro->name);
            fprintf(out, "%*s%.2lf\n", max_length_label, "Price: ", pro->before_tax);
            fprintf(out, "%*s%.2lf\n", max_length_label, "Priceafter Tax: ", priceAfterTax(pro));
            fprintf(out, "%*s%d %s\n", max_length_label, "Quantity Available: ", pro->quantity, pro->unit);
            fprintf(out, "%*s%d %s\n", max_length_label, "Quantity Needed: ", pro->needed, pro->unit);
            if (pro->type == 'P') {
                fprintf(out, "%-*s", max_length_label, "Expiry Date: ");
                Date_write(&pro->expiry, out);
            }
            break;
        }
    }
    return true;
}
