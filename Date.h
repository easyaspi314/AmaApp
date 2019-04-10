#ifndef AMA_DATE_H
#define AMA_DATE_H
#include <stdio.h>
#include <stdbool.h>

enum {
    min_year = 2019,
    max_year = 2028,
    no_error = 0,
    error_year = 1,
    error_mon = 2,
    error_day = 3,
    error_invalid_operation = 4,
    error_input = 5
};

typedef struct {
    int year, month, day_of_the_month, Status;
} Date;

Date *Date_create(void);
Date *Date_createWithDate(int year, int month, int day);
int Date_status(const Date *c);
void Date_clearError(Date *c);
bool Date_isGood(const Date *c);
Date *Date_add(Date *d, int amt);
int Date_compare(const Date *lhs, const Date *rhs);
void Date_read(Date *c, FILE *in);
void Date_write(const Date *c, FILE *out);
#endif
