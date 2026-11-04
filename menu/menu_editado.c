#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../odbc-examples/odbc.h"

/* Prototipos de las funciones */

static int ShowMainMenu(void);

static void ShowProductsMenu(SQLHDBC dbc);
static int ShowProductsSubMenu(void);
static void ProductStock(SQLHDBC dbc);
static void ProductFind(SQLHDBC dbc);

static void ShowOrdersMenu(SQLHDBC dbc);
static int ShowOrdersSubMenu(void);
static void OrderOpen(SQLHDBC dbc);
static void OrderRange(SQLHDBC dbc);
static void OrderDetail(SQLHDBC dbc);

static void ShowCustomersMenu(SQLHDBC dbc);
static int ShowCustomersSubMenu(void);
static void CustomerFind(SQLHDBC dbc);
static void CustomerListProducts(SQLHDBC dbc);
static void CustomerBalance(SQLHDBC dbc);


/**
 * @brief Entry point, shows and processes the main menu.
 *
 * @return EXIT_SUCCESS if no error, EXIT_FAILURE otherwise.
 * @author Marta y Yu Lin
 */
int main(void) {
    int nChoice = 0;
    SQLHENV env;
    SQLHDBC dbc;

    if (!SQL_SUCCEEDED(odbc_connect(&env, &dbc))) {
        printf("Error connecting to the database.\n");
        return EXIT_FAILURE;
    }

    printf("Connected to the database successfully.\n");

    do {
        nChoice = ShowMainMenu();

        switch (nChoice) {
            case 1: {
                ShowProductsMenu(dbc);
                break;
            }

            case 2: {
                ShowOrdersMenu(dbc);
                break;
            }

            case 3: {
                ShowCustomersMenu(dbc);
                break;
            }

            case 4: {
                printf("Bye Bye\n\n");
                break;
            }
        }

    } while (nChoice != 4);

    if (!SQL_SUCCEEDED(odbc_disconnect(env, dbc))) {
        printf("Error disconnecting from the database.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Prints the main menu and allows the user to select an option.
 *
 * @return Selected entry in the menu.
 * @author Marta
 */
static int ShowMainMenu(void) {
    int nSelected = 0;
    char buf[16];

    do {
        printf("\n");
        printf(" (1) Products\n"
               " (2) Orders\n"
               " (3) Customers\n"
               " (4) Exit\n\n"
               "Enter a number that corresponds to your choice > ");

        if (!fgets(buf, 16, stdin)) {
            nSelected = 0;
        }
        else {
            nSelected = atoi(buf);
        }

        printf("\n");

        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n");
        }

    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}


/**
 * @brief Shows and processes the products submenu.
 */
static void ShowProductsMenu(SQLHDBC dbc) {
    int nChoice = 0;

    do {
        nChoice = ShowProductsSubMenu();

        switch (nChoice) {
            case 1: {
                ProductStock(dbc);
                break;
            }

            case 2: {
                ProductFind(dbc);
                break;
            }

            case 3: {
                printf("Back\n\n");
                break;
            }
        }

    } while (nChoice != 3);
}


/**
 * @brief Prints the products submenu.
 *
 * @return Selected entry in the submenu.
 */
static int ShowProductsSubMenu(void) {
    int nSelected = 0;
    char buf[16];

    do {
        printf(" (1) Stock\n"
               " (2) Find\n"
               " (3) Back\n\n"
               "Enter a number that corresponds to your choice > ");

        if (!fgets(buf, 16, stdin)) {
            nSelected = 0;
        }
        else {
            nSelected = atoi(buf);
        }

        printf("\n");

        if ((nSelected < 1) || (nSelected > 3)) {
            printf("You have entered an invalid choice. Please try again\n\n");
        }

    } while ((nSelected < 1) || (nSelected > 3));

    return nSelected;
}


/**
 * @brief Implements the product stock functionality.
 */
static void ProductStock(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;
    char productCode[16];
    int stock;

    printf("Enter productcode > ");
    fgets(productCode, 16, stdin);
    productCode[strlen(productCode) - 1] = '\0';

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt,
               (SQLCHAR *)"SELECT quantityInStock FROM products WHERE productCode = ?",
               SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                     SQL_C_CHAR, SQL_VARCHAR, 15, 0,
                     productCode, 16, NULL);

    SQLBindCol(stmt, 1, SQL_C_SLONG, &stock, 0, NULL);

    ret = SQLExecute(stmt);

    if (SQL_SUCCEEDED(ret) && SQL_SUCCEEDED(SQLFetch(stmt))) {
        printf("Quantity in stock: %d\n\n", stock);
    }
    else {
        printf("Product not found.\n\n");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


static void ProductFind(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;
    char productName[51];
    char search[53];
    char productCode[16];
    char name[71];

    printf("Enter productname > ");
    fgets(productName, 51, stdin);
    productName[strcspn(productName, "\n")] = '\0';

    sprintf(search, "%%%s%%", productName);

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt,
               (SQLCHAR *)"SELECT productCode, productName "
                          "FROM products "
                          "WHERE productName LIKE ? "
                          "ORDER BY productCode",
               SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                     SQL_C_CHAR, SQL_VARCHAR, 52, 0,
                     search, 53, NULL);

    SQLBindCol(stmt, 1, SQL_C_CHAR,
               productCode, sizeof(productCode), NULL);

    SQLBindCol(stmt, 2, SQL_C_CHAR,
               name, sizeof(name), NULL);

    ret = SQLExecute(stmt);

    if (SQL_SUCCEEDED(ret)) {
        while (SQL_SUCCEEDED(SQLFetch(stmt))) {
            printf("%s - %s\n", productCode, name);
        }
        printf("\n");
    }
    else {
        printf("Error searching products.\n\n");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}



/**
 * @brief Shows and processes the orders submenu.
 */
static void ShowOrdersMenu(SQLHDBC dbc) {
    int nChoice = 0;

    (void)dbc;

    do {
        nChoice = ShowOrdersSubMenu();

        switch (nChoice) {
            case 1: {
                OrderOpen(dbc);
                break;
            }

            case 2: {
                OrderRange(dbc);
                break;
            }

            case 3: {
                OrderDetail(dbc);
                break;
            }

            case 4: {
                printf("Back\n\n");
                break;
            }
        }

    } while (nChoice != 4);
}


/**
 * @brief Prints the orders submenu.
 *
 * @return Selected entry in the submenu.
 */
static int ShowOrdersSubMenu(void) {
    int nSelected = 0;
    char buf[16];

    do {
        printf(" (1) Open\n"
               " (2) Range\n"
               " (3) Detail\n"
               " (4) Back\n\n"
               "Enter a number that corresponds to your choice > ");

        if (!fgets(buf, 16, stdin)) {
            nSelected = 0;
        }
        else {
            nSelected = atoi(buf);
        }

        printf("\n");

        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n");
        }

    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}


static void OrderOpen(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;
    int orderNumber;

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt,
               (SQLCHAR *)"SELECT orderNumber FROM orders "
                          "WHERE shippedDate IS NULL "
                          "ORDER BY orderNumber",
               SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_SLONG,
               &orderNumber, 0, NULL);

    ret = SQLExecute(stmt);

    if (SQL_SUCCEEDED(ret)) {
        printf("Open orders:\n");

        while (SQL_SUCCEEDED(SQLFetch(stmt))) {
            printf("%d\n", orderNumber);
        }

        printf("\n");
    }
    else {
        printf("Error searching orders.\n\n");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


/**
 * @brief Lists the orders placed between two dates given by the user.
 * @author Marta y Yu Lin
 */
static void OrderRange(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;
    char dates[32];
    char from[11];
    char to[11];
    int orderNumber;
    char orderDate[11];
    char shippedDate[11];
    SQLLEN shippedInd = 0;
    int found = 0;

    printf("Enter dates (YYYY-MM-DD - YYYY-MM-DD) > ");
    fgets(dates, 32, stdin);

    if (sscanf(dates, "%10s - %10s", from, to) != 2) {
        printf("Invalid dates.\n\n");
        return;
    }

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt,
               (SQLCHAR *)"SELECT orderNumber, orderDate, shippedDate "
                          "FROM orders "
                          "WHERE orderDate BETWEEN CAST(? AS DATE) "
                          "AND CAST(? AS DATE) "
                          "ORDER BY orderNumber",
               SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                     SQL_C_CHAR, SQL_VARCHAR, 10, 0,
                     from, 11, NULL);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
                     SQL_C_CHAR, SQL_VARCHAR, 10, 0,
                     to, 11, NULL);

    SQLBindCol(stmt, 1, SQL_C_SLONG,
               &orderNumber, 0, NULL);

    SQLBindCol(stmt, 2, SQL_C_CHAR,
               orderDate, sizeof(orderDate), NULL);

    SQLBindCol(stmt, 3, SQL_C_CHAR,
               shippedDate, sizeof(shippedDate), &shippedInd);

    ret = SQLExecute(stmt);

    if (SQL_SUCCEEDED(ret)) {
        while (SQL_SUCCEEDED(SQLFetch(stmt))) {
            if (shippedInd == SQL_NULL_DATA) {
                printf("%d %s Not shipped\n", orderNumber, orderDate);
            }
            else {
                printf("%d %s %s\n", orderNumber, orderDate, shippedDate);
            }
            found = 1;
        }

        if (!found) {
            printf("No orders found.\n");
        }

        printf("\n");
    }
    else {
        printf("Error searching orders: check that the dates exist.\n\n");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


/**
 * @brief Shows the date, status, total cost and products of an order.
 * @author Marta y Yu Lin
 */
static void OrderDetail(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;
    char buf[32];
    int orderNumber;
    char orderDate[11];
    char status[16];
    double total;
    char productCode[16];
    int quantity;
    double price;
    SQLLEN codeInd = 0;
    SQLLEN quantityInd = 0;
    SQLLEN priceInd = 0;
    int found = 0;

    printf("Enter ordernumber > ");
    fgets(buf, 32, stdin);
    orderNumber = atoi(buf);

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt,
               (SQLCHAR *)"SELECT o.orderDate, o.status, "
                          "COALESCE(SUM(d.quantityOrdered * d.priceEach) "
                          "OVER (), 0), "
                          "d.productCode, d.quantityOrdered, d.priceEach "
                          "FROM orders o "
                          "LEFT JOIN orderdetails d "
                          "ON d.orderNumber = o.orderNumber "
                          "WHERE o.orderNumber = ? "
                          "ORDER BY d.orderLineNumber",
               SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                     SQL_C_SLONG, SQL_INTEGER, 0, 0,
                     &orderNumber, 0, NULL);

    SQLBindCol(stmt, 1, SQL_C_CHAR,
               orderDate, sizeof(orderDate), NULL);

    SQLBindCol(stmt, 2, SQL_C_CHAR,
               status, sizeof(status), NULL);

    SQLBindCol(stmt, 3, SQL_C_DOUBLE,
               &total, 0, NULL);

    SQLBindCol(stmt, 4, SQL_C_CHAR,
               productCode, sizeof(productCode), &codeInd);

    SQLBindCol(stmt, 5, SQL_C_SLONG,
               &quantity, 0, &quantityInd);

    SQLBindCol(stmt, 6, SQL_C_DOUBLE,
               &price, 0, &priceInd);

    ret = SQLExecute(stmt);

    if (SQL_SUCCEEDED(ret)) {
        while (SQL_SUCCEEDED(SQLFetch(stmt))) {
            if (!found) {
                printf("%s %s\n", orderDate, status);
                printf("Total cost: %.2f\n", total);
                found = 1;
            }

            if (codeInd != SQL_NULL_DATA) {
                printf("%s %d %.2f\n", productCode, quantity, price);
            }
        }

        if (!found) {
            printf("Order not found.\n");
        }

        printf("\n");
    }
    else {
        printf("Error searching the order.\n\n");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}



/**
 * @brief Shows and processes the customers submenu.
 */
static void ShowCustomersMenu(SQLHDBC dbc) {
    int nChoice = 0;

    (void)dbc;

    do {
        nChoice = ShowCustomersSubMenu();

        switch (nChoice) {
            case 1: {
                CustomerFind(dbc);
                break;
            }

            case 2: {
                CustomerListProducts(dbc);
                break;
            }

            case 3: {
                CustomerBalance(dbc);
                break;
            }

            case 4: {
                printf("Back\n\n");
                break;
            }
        }

    } while (nChoice != 4);
}


/**
 * @brief Prints the customers submenu.
 *
 * @return Selected entry in the submenu.
 */
static int ShowCustomersSubMenu(void) {
    int nSelected = 0;
    char buf[16];

    do {
        printf(" (1) Find\n"
               " (2) List Products\n"
               " (3) Balance\n"
               " (4) Back\n\n"
               "Enter a number that corresponds to your choice > ");

        if (!fgets(buf, 16, stdin)) {
            nSelected = 0;
        }
        else {
            nSelected = atoi(buf);
        }

        printf("\n");

        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n");
        }

    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}


/**
 * @brief Lists, 10 rows per page, the customers whose contact first or last
 * name contains the text given by the user.
 * @author Marta y Yu Lin
 */
static void CustomerFind(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;
    char contactName[51];
    char search[53];
    char key[16];
    int number;
    char name[51];
    char firstName[51];
    char lastName[51];
    int offset = 0;
    int nRows;
    int hasNext;
    int more;

    printf("Enter customer name > ");
    fgets(contactName, 51, stdin);
    contactName[strcspn(contactName, "\n")] = '\0';

    sprintf(search, "%%%s%%", contactName);

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* LIMIT 11: the row after the page tells if there is a next page */
    SQLPrepare(stmt,
               (SQLCHAR *)"SELECT customerNumber, customerName, "
                          "contactFirstName, contactLastName "
                          "FROM customers "
                          "WHERE contactFirstName LIKE ? "
                          "OR contactLastName LIKE ? "
                          "ORDER BY customerNumber "
                          "LIMIT 11 OFFSET ?",
               SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                     SQL_C_CHAR, SQL_VARCHAR, 52, 0,
                     search, 53, NULL);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
                     SQL_C_CHAR, SQL_VARCHAR, 52, 0,
                     search, 53, NULL);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT,
                     SQL_C_SLONG, SQL_INTEGER, 0, 0,
                     &offset, 0, NULL);

    SQLBindCol(stmt, 1, SQL_C_SLONG,
               &number, 0, NULL);

    SQLBindCol(stmt, 2, SQL_C_CHAR,
               name, sizeof(name), NULL);

    SQLBindCol(stmt, 3, SQL_C_CHAR,
               firstName, sizeof(firstName), NULL);

    SQLBindCol(stmt, 4, SQL_C_CHAR,
               lastName, sizeof(lastName), NULL);

    do {
        more = 0;
        ret = SQLExecute(stmt);

        if (!SQL_SUCCEEDED(ret)) {
            printf("Error searching customers.\n\n");
            break;
        }

        nRows = 0;

        while (nRows < 10 && SQL_SUCCEEDED(SQLFetch(stmt))) {
            printf("%d %s %s %s\n", number, name, firstName, lastName);
            nRows++;
        }

        hasNext = (nRows == 10) && SQL_SUCCEEDED(SQLFetch(stmt));

        SQLCloseCursor(stmt);

        if (nRows == 0 && offset == 0) {
            printf("No customers found.\n");
        }

        printf("\n");

        if (hasNext || offset > 0) {
            printf("'>' next, '<' previous, Enter to go back > ");
            fgets(key, 16, stdin);

            if (key[0] == '>' && hasNext) {
                offset = offset + 10;
                more = 1;
            }
            else if (key[0] == '<' && offset > 0) {
                offset = offset - 10;
                more = 1;
            }

            printf("\n");
        }

    } while (more);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


/**
 * @brief Lists the products bought by a customer, with the total number of
 * units of each one across all of the customer's orders.
 * @author Marta y Yu Lin
 */
static void CustomerListProducts(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;
    char buf[32];
    int customerNumber;
    char name[71];
    int units;
    int found = 0;

    printf("Enter customer number > ");
    fgets(buf, 32, stdin);
    customerNumber = atoi(buf);

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt,
               (SQLCHAR *)"SELECT p.productName, SUM(d.quantityOrdered) "
                          "FROM orders o "
                          "JOIN orderdetails d "
                          "ON d.orderNumber = o.orderNumber "
                          "JOIN products p "
                          "ON p.productCode = d.productCode "
                          "WHERE o.customerNumber = ? "
                          "GROUP BY p.productCode, p.productName "
                          "ORDER BY p.productCode",
               SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                     SQL_C_SLONG, SQL_INTEGER, 0, 0,
                     &customerNumber, 0, NULL);

    SQLBindCol(stmt, 1, SQL_C_CHAR,
               name, sizeof(name), NULL);

    SQLBindCol(stmt, 2, SQL_C_SLONG,
               &units, 0, NULL);

    ret = SQLExecute(stmt);

    if (SQL_SUCCEEDED(ret)) {
        while (SQL_SUCCEEDED(SQLFetch(stmt))) {
            printf("%s %d\n", name, units);
            found = 1;
        }

        if (!found) {
            printf("No products found for this customer.\n");
        }

        printf("\n");
    }
    else {
        printf("Error searching products.\n\n");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


/**
 * @brief Shows the balance of a customer: payments minus purchases.
 * @author Marta y Yu Lin
 */
static void CustomerBalance(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;
    char buf[32];
    int customerNumber;
    double balance;

    printf("Enter customer number > ");
    fgets(buf, 32, stdin);
    customerNumber = atoi(buf);

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt,
               (SQLCHAR *)"SELECT COALESCE((SELECT SUM(p.amount) "
                          "FROM payments p "
                          "WHERE p.customerNumber = c.customerNumber), 0) "
                          "- COALESCE((SELECT "
                          "SUM(d.quantityOrdered * d.priceEach) "
                          "FROM orders o "
                          "JOIN orderdetails d "
                          "ON d.orderNumber = o.orderNumber "
                          "WHERE o.customerNumber = c.customerNumber), 0) "
                          "FROM customers c "
                          "WHERE c.customerNumber = ?",
               SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                     SQL_C_SLONG, SQL_INTEGER, 0, 0,
                     &customerNumber, 0, NULL);

    SQLBindCol(stmt, 1, SQL_C_DOUBLE,
               &balance, 0, NULL);

    ret = SQLExecute(stmt);

    if (SQL_SUCCEEDED(ret) && SQL_SUCCEEDED(SQLFetch(stmt))) {
        printf("Balance = %.2f\n\n", balance);
    }
    else {
        printf("Customer not found.\n\n");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}
