#define NUM_PRODUCTS 20 // Ο αριθμός των διαφορετικών προϊόντων
#define NUM_CLIENTS 5   // Ο αριθμός των πελατών
#define ORDERS_PER_CLIENT 10 // Ο αριθμός παραγγελιών ανά πελάτη
#define PORT 8080 // port για την επικοινωνία
#define SERVER_IP "127.0.0.1" // ip για την επικοινωνία


typedef struct {
    char description[100]; // Περιγραφή προϊόντος
    float price; // Τιμή προϊόντος
    int item_count; // Διαθέσιμη ποσότητα
} Product;


void initialize_catalog();
void handle_order(int product_id, int *local_success, int *local_failure, float *local_revenue);

