#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/wait.h>

Product catalog[NUM_PRODUCTS]; // Πίνακας προϊόντων
int success = 0; // Μετρητής επιτυχημένων παραγγελιών
int failure = 0; // Μετρητής αποτυχημένων παραγγελιών
float revenue = 0.0; // Συνολικά έσοδα
int unsatisfied_count[NUM_PRODUCTS] = {0}; // Πλήθος μη εξυπηρετούμενων παραγγελιών ανά προϊόν

// Αρχικοποίηση του καταλόγου προϊόντων
void initialize_catalog() {
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        snprintf(catalog[i].description, sizeof(catalog[i].description), "Product %d", i); // Ορισμός περιγραφής
        catalog[i].price = ((float)(rand() % 100) / 2.0) + 0.5; // Τυχαία τιμή προϊόντος
        catalog[i].item_count = 2; // Αρχικ΄ά διαθέσιμο απόθεμα 2
    }
}

// Διαχείριση παραγγελίας
void handle_order(int product_id, int *local_success, int *local_failure, float *local_revenue) {
    if (product_id >= 0 && product_id < NUM_PRODUCTS) { // Έλεγχος αν το προϊόν είναι έγκυρο
        if (catalog[product_id].item_count > 0) { // Έλεγχος αν υπάρχει διαθέσιμο απόθεμα
            catalog[product_id].item_count = catalog[product_id].item_count - 1; // Μείωση αποθέματος κατά 1
            (*local_success) = (*local_success) + 1; // Αύξηση επιτυχημένων παραγγελιών κατά 1
            *local_revenue = *local_revenue + catalog[product_id].price; // Αύξηση εσόδων
        } 
        else { // Αν δεν υπάρχει διαθέσιμο απόθεμα
            (*local_failure) = (*local_failure) + 1; // Αύξηση αποτυχημένων παραγγελιών κατά 1
            unsatisfied_count[product_id] = unsatisfied_count[product_id] + 1; // Αύξηση μη εξυπηρετημένων παραγγελιών
        }
    }
}

int main() {
    srand(time(NULL)); // Παραγωγή τυχαίων αριθμών
    initialize_catalog(); // Συνάρτηση για γέμισμα καταλόγων προϊόντων

    int server_fd; // File descriptor του server socket
    int new_socket; // File descriptor για σύνδεση με κάθε πελάτη
    struct sockaddr_in address; // Δομή για την αποθήκευση της διεύθυνσης του server
    int addrlen = sizeof(address);
    int i,j;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { // Δημιουργία socket
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Αποδοχή συνδέσεων από οποιαδήποτε διεύθυνση
    address.sin_port = htons(PORT); // Ορισμός θύρας επικοινωνίας

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { // Δέσμευση του socket στη διεύθυνση και θύρα
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Ορίζει τον server να ακούει για εισερχόμενες συνδέσεις
if (listen(server_fd, NUM_CLIENTS) < 0) {
    perror("Listen failed"); // Εκτυπώνει μήνυμα σφάλματος αν η listen αποτύχει
    exit(EXIT_FAILURE); // Τερματίζει το πρόγραμμα με αποτυχία
}

// Εκτυπώνει μήνυμα ότι ο server τρέχει
printf("Server is running!\n");

// Εκτυπώνει το port στο οποίο ακούει ο server
printf("Server is listening on port %d\n", PORT);

int pipefd[2]; // Δημιουργεί έναν πίνακα 2 θέσεων για την επικοινωνία μέσω pipe

// Δημιουργεί ένα pipe για την επικοινωνία μεταξύ διεργασιών
if (pipe(pipefd) == -1) {
    perror("Pipe failed"); // Αν η δημιουργία του pipe αποτύχει, εκτυπώνει μήνυμα σφάλματος
    exit(EXIT_FAILURE); // Τερματίζει το πρόγραμμα με αποτυχία
}

// Βρόχος για την αποδοχή συνδέσεων από πελάτες
for (i = 0; i < NUM_CLIENTS; i++) {
    // Αποδέχεται μια εισερχόμενη σύνδεση και επιστρέφει ένα νέο socket
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed"); // Αν η accept αποτύχει, εκτυπώνει μήνυμα σφάλματος
        exit(EXIT_FAILURE); // Τερματίζει το πρόγραμμα με αποτυχία
    }


        pid_t pid = fork();
        if (pid == 0) {  
            close(server_fd);
            close(pipefd[0]); // Κλείσμο άκρου ανάγωνσης

            int local_success = 0; // Τοπική μεταβλητή μέτρησης επιτυχιών
            int local_failure = 0; // Τοπική μεταβλητή μέτρησης αποτυχιών
            float local_revenue = 0.0; // Τοπική μεταβλητή μέτρησης εσόδων
            
            for (j = 0; j < ORDERS_PER_CLIENT; j++) {
                int product_id;
                if (recv(new_socket, &product_id, sizeof(int), 0) <= 0) { // Λήψη παραγγελίας από κάθε πελάτη
                    perror("Receive failed");
                    close(new_socket);
                    exit(EXIT_FAILURE);
                }

                handle_order(product_id, &local_success, &local_failure, &local_revenue); // Επεξεργασία παραγγελίας

                char response[100];
                
                if (catalog[product_id].item_count >= 0) {
                    snprintf(response, sizeof(response), "Order for Product %d: Success", product_id); // Μήνυμα όταν υπάρχει διαθέσιμο απόθεμα και η παραγγελία πραγματοποιείται με επιτυχία
                } 
                else {
                    snprintf(response, sizeof(response), "Order for Product %d: Out of stock", product_id); // Μήνυμα όταν δεν υπάρχει διαθέσιμο απόθεμα και η παραγγελία δεν πραγματοποιείται
                }


                if (send(new_socket, response, strlen(response), 0) < 0) {
                    perror("Send failed");
                    close(new_socket);
                    exit(EXIT_FAILURE);
                }

                sleep(1);
            }

            close(new_socket);

            write(pipefd[1], &local_success, sizeof(int)); // Καταγραφή επιτυχιών
            write(pipefd[1], &local_failure, sizeof(int)); // Καταγραφή αποτυχιών
            write(pipefd[1], &local_revenue, sizeof(float)); // Καταγραφή εσόδων

            close(pipefd[1]); // Κλείσιμο άκρου καταγραφής
            exit(0);
        }

        close(new_socket); 
    }

    close(pipefd[1]);  // Κλείσιμο του άκρου εγγραφής του pipe, καθώς δεν χρειάζεται πλέον να γράφουμε δεδομένα

    wait(NULL);  // Αναμονή για να τερματίσουν όλες οι διεργασίες-παιδιά πριν συνεχίσουμε

    int temp_success;  // Μεταβλητή για αποθήκευση των επιτυχημένων παραγγελιών
    int temp_failure;  // Μεταβλητή για αποθήκευση των αποτυχημένων παραγγελιών 
    float temp_revenue; // Μεταβλητή για αποθήκευση των τοπικών εσόδων από μια διεργασία-παιδί

    // Διαβάζουμε δεδομένα από το pipe μέχρι να μην υπάρχει άλλο διαθέσιμο
    while (read(pipefd[0], &temp_success, sizeof(int)) > 0) {  
        read(pipefd[0], &temp_failure, sizeof(int));  // Διαβάζουμε τον αριθμό των αποτυχημένων παραγγελιών
        read(pipefd[0], &temp_revenue, sizeof(float)); // Διαβάζουμε τα τοπικά έσοδα 

        // Προσθέτουμε τις τιμές που διαβάσαμε στις συνολικές μετρήσεις του server
        success = success + temp_success;  // Αύξηση του συνολικού αριθμού επιτυχημένων παραγγελιών
        failure = failure + temp_failure;  // Αύξηση του συνολικού αριθμού αποτυχημένων παραγγελιών
        revenue = revenue + temp_revenue;  // Αύξηση του συνολικού τζίρου
    }

    close(pipefd[0]);  // Κλείσιμο του άκρου ανάγνωσης του pipe

    // Εκτύπωση των τελικών στατιστικών του server μετά την ολοκλήρωση όλων των παραγγελιών
    printf("\nFinal Report:\n");
    printf("Total requests: %d\n", success + failure); // Συνολικός αριθμός αιτημάτων
    printf("Successful requests: %d\n", success);  // Συνολικός αριθμός επιτυχημένων παραγγελιών
    printf("Failed requests: %d\n", failure);  // Συνολικός αριθμός αποτυχημένων παραγγελιών
    printf("Total revenue: %.2f\n", revenue);  // Συνολικά έσοδα από τις πωλήσεις

    return 0;  

}
