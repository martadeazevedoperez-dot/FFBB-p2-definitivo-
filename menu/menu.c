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

static void ShowCustomersMenu(SQLHDBC dbc);
static int ShowCustomersSubMenu(void);


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

    printf("Enter product code > ");
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

    printf("Enter product name > ");
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
                printf("Range\n\n");
                break;
            }

            case 3: {
                printf("Detail\n\n");
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
 * @brief Shows and processes the customers submenu.
 */
static void ShowCustomersMenu(SQLHDBC dbc) {
    int nChoice = 0;

    (void)dbc;

    do {
        nChoice = ShowCustomersSubMenu();

        switch (nChoice) {
            case 1: {
                printf("Find\n\n");
                break;
            }

            case 2: {
                printf("List Products\n\n");
                break;
            }

            case 3: {
                printf("Balance\n\n");
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
