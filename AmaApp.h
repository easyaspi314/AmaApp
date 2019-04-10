#ifndef AMA_APP_H
#define AMA_APP_H
#include "Product.h"

enum {
    max_file_size = 256,
    max_product_size = 100
};
void initialize(const char *c);
void cleanup(void);
int run(void);

#endif
