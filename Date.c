#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Date.h"

static int mdays(int, int);
static bool isEmpty(const Date *);

Date *Date_create(void) //default constructor
{
    return (Date *)calloc(1, sizeof(Date));
}

Date *Date_createWithDate(int year, int month, int day) //3 arg constructor
{
    Date *c = (Date *)calloc(1, sizeof(Date));
    if (year < min_year || year > max_year) {
        c->Status = error_year;
    } else if (month < 1 || month > 12) {
        c->Status = error_mon;
    } else if (day < 1 || day > mdays(month, year)) {
        c->Status = error_day;
    } else {
        c->year = year;
        c->month = month;
        c->day_of_the_month = day;
        c->Status = no_error;
    }
    return c;
}

//mdays function
static int mdays(int mon, int year)
{
    static const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, -1 };
    int month = mon >= 1 && mon <= 12 ? mon : 13;
    month--;
    return days[month] + (int)((month == 1) * ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
}

//status private
int Date_status(const Date *c)
{
    return c->Status;
}
//clear error function
void Date_clearError(Date *c)
{
    c->Status = no_error;
}

bool Date_isGood(const Date *c)
{
    return ((c->year >= min_year && c->year <= max_year) && (c->month >= 1 && c->month <= 12) && (c->day_of_the_month >= 1 && c->day_of_the_month <= mdays(c->month, c->year)) && (c->Status == no_error));
}

Date *Date_add(Date *c, int days)
{
    if (c->Status != no_error || isEmpty(c))
        c->Status = error_invalid_operation;
    else if (c->day_of_the_month + days > mdays(c->month, c->year))
        c->Status = error_invalid_operation;
    else if (c->day_of_the_month + days < 1)
        c->Status = error_invalid_operation;
    else
        c->day_of_the_month += days;
    return c;
}

int Date_compare(const Date *lhs, const Date *rhs)
{
    if (lhs->year != rhs->year) {
        return (lhs->year > rhs->year) - (lhs->year < rhs->year);
    }
    if (lhs->month != rhs->month) {
        return (lhs->month > rhs->month) - (lhs->month < rhs->month);
    }
    if (lhs->day_of_the_month != rhs->day_of_the_month) {
        return (lhs->day_of_the_month > rhs->day_of_the_month) - (lhs->day_of_the_month < rhs->day_of_the_month);
    }
    return 0;
}

static bool isEmpty(const Date *c)
{
    return c->year == 0 && c->month == 0 && c->day_of_the_month == 0 && c->Status == no_error;
}

void Date_write(const Date *c, FILE *out)
{
    fprintf(out, "%04d/%02d/%02d", c->year, c->month, c->day_of_the_month);
}

void Date_read(Date *c, FILE *in)
{
    if (ferror(in) || feof(in)) {
        c->year = c->month = c->day_of_the_month = 0;
        c->Status = error_input;
    } else if (fscanf(in, "%d/%d/%d", &c->year, &c->month, &c->day_of_the_month) < 3) {
        c->year = c->month = c->day_of_the_month = 0;
        c->Status = error_input;
    } else if (c->year < min_year || c->year > max_year) {
        c->year = c->month = c->day_of_the_month = 0;
        c->Status = error_year;
    } else if (c->month < 1 || c->month > 12) {
        c->year = c->month = c->day_of_the_month = 0;
        c->Status = error_mon;
    } else if (c->day_of_the_month < 1 || c->day_of_the_month > mdays(c->month, c->year)) {
        c->year = c->month = c->day_of_the_month = 0;
        c->Status = error_day;
    } else
        c->Status = no_error;
}
